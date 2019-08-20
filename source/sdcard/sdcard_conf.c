#include "sdcard_conf.h"
#include "logger.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------- PRIVATE FUNCTIONS HEADERS ------------------------- */
/* ----------------------------------------------------------------------------- */

static void SDCARD_DetectCallBack(bool isInserted, void *userData);
static void SDCARD_DetectTask(void *pvParameters);
static void SDCARD_LoadTask(void *pvParameterts);
static void SDCARD_SaveTask(void *pvParameterts);

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */


/* Card descriptor (located in FatFS files) */
extern sd_card_t g_sd;

/* Data written to the card */
SDK_ALIGN(uint8_t g_dataWrite[SDK_SIZEALIGN(SDCARD_DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
		MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/* Data read from the card */
SDK_ALIGN(uint8_t g_dataRead[SDK_SIZEALIGN(SDCARD_DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
		MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

/* SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
		.cdType = BOARD_SD_DETECT_TYPE,
		.cdTimeOut_ms = (~0U),
		.cardInserted = SDCARD_DetectCallBack,
		.cardRemoved  = SDCARD_DetectCallBack,
};

/* SD card detect flag  */
static volatile bool sdcardInserted = false;
static volatile bool sdcardInsertStatus = false;

/* Card semaphores  */
static SemaphoreHandle_t sdcardAccessSemaphore = NULL;
static SemaphoreHandle_t sdcardDetectSemaphore = NULL;

static SemaphoreHandle_t sdcardLoadSemaphore = NULL;
static SemaphoreHandle_t sdcardSaveSemaphore = NULL;

/* FatFS related */
static const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
static FATFS fatfsFileSystem; /* File system object */
static FRESULT fsResult;

/* RTOS handles */
static TaskHandle_t xSDLoadTask;
static TaskHandle_t xSDSaveTask;

static SDCARD_Conf_h settings;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */


static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
	sdcardInsertStatus = isInserted;
	xSemaphoreGiveFromISR(sdcardDetectSemaphore, NULL);
}

static void SDCARD_FSInit(void)
{
	fsResult = f_mount(&fatfsFileSystem, driverNumberBuffer, 0U);
    if (fsResult) {
    	LOGGER_WRITELN(("FS mount volume failed"));
    }

    fsResult = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (fsResult) {
        LOGGER_WRITELN(("FS change drive failed"));
    }

}

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API_FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

bool SDCARD_RTOSInit(void)
{
	if (xTaskCreate(SDCARD_DetectTask, TASK_SDDETECT_NAME, TASK_SDDETECT_STACK, NULL, TASK_SDDETECT_PRIO, NULL) != pdPASS) {
		return false;
	}

	if (xTaskCreate(SDCARD_LoadTask, TASK_SDLOAD_NAME, TASK_SDLOAD_STACK, NULL, TASK_SDLOAD_PRIO, &xSDLoadTask) != pdPASS) {
		return false;
	}

	if (xTaskCreate(SDCARD_SaveTask, TASK_SDSAVE_NAME, TASK_SDSAVE_STACK, NULL, TASK_SDSAVE_PRIO, &xSDSaveTask) != pdPASS) {
		return false;
	}

	sdcardAccessSemaphore = xSemaphoreCreateBinary();
	sdcardDetectSemaphore = xSemaphoreCreateBinary();

	sdcardLoadSemaphore = xSemaphoreCreateBinary();
	sdcardSaveSemaphore = xSemaphoreCreateBinary();

	return (sdcardAccessSemaphore != NULL && sdcardDetectSemaphore != NULL);
}

void SDCARD_LoadSettings(void)
{
	xSemaphoreGive(sdcardLoadSemaphore);
}

void SDCARD_SaveSettings(void)
{
	xSemaphoreGive(sdcardSaveSemaphore);
}

/* ----------------------------------------------------------------------------- */
/* ------------------------------ FREERTOS TASKS ------------------------------- */
/* ----------------------------------------------------------------------------- */

static void SDCARD_DetectTask(void *pvParameters)
{
		g_sd.host.base = SD_HOST_BASEADDR;
		g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
		g_sd.usrParam.cd = &s_sdCardDetect;

		/* SD host init function */
		if (SD_HostInit(&g_sd) == kStatus_Success) {
			while (true) {

				/* Take card semaphores */
				xSemaphoreTake(sdcardDetectSemaphore, portMAX_DELAY);

				if (sdcardInserted != sdcardInsertStatus) {
					sdcardInserted = sdcardInsertStatus;

					/* Power off card */
					SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);

					if (sdcardInserted) {
						PRINTF("SD card inserted.\r\n");
						/* Power on the card */
						SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
						/* Init card */
						if (SD_CardInit(&g_sd)) {
							PRINTF("SD card init failed.\r\n");
							continue;
						}

						/* FatFS init */
						SDCARD_FSInit();
						xSemaphoreGive(sdcardAccessSemaphore);
					}
				}

				if (!sdcardInserted) {
					PRINTF("SD card removed. IO operations not supported now.\r\n");
					xSemaphoreTake(sdcardAccessSemaphore, portMAX_DELAY);
				}
			}
		}
		else {
			PRINTF("SD host init fail\r\n");
		}

		vTaskSuspend(NULL);
}

static void SDCARD_LoadTask(void *pvParameterts)
{
	FIL readFile;
	SDCARD_Conf_h settingsBuff;
	uint32_t bytesRead;

	while (true) {
		/* Wait until settings load is needed */
		xSemaphoreTake(sdcardLoadSemaphore, portMAX_DELAY);

		if (xSemaphoreTake(sdcardAccessSemaphore, SDCARD_IO_WAIT) != pdPASS) {
			LOGGER_WRITELN(("SD card not present"));
			continue;
		}

		fsResult = f_open(&readFile, SDCARD_SETTINGS_PATH, FA_READ);

		if (fsResult) {
			LOGGER_WRITELN(("Settings file not found"));
			xSemaphoreGive(sdcardAccessSemaphore);
			continue;
		}

		fsResult = f_read(&readFile, (void *)&settingsBuff, sizeof(settingsBuff), &bytesRead);

		if (fsResult || bytesRead != sizeof(settingsBuff)) {
			LOGGER_WRITELN(("Unable to read settings. Corrupted file"));
		} else {
			LOGGER_WRITELN(("Read Succeed"));
		}

		f_close(&readFile);
		xSemaphoreGive(sdcardAccessSemaphore);
	}
}

static void SDCARD_SaveTask(void *pvParameterts)
{
	FIL writeFile;
	uint32_t bytesWritten;

	while (true) {
		/* Wait until settings save is needed */
		xSemaphoreTake(sdcardSaveSemaphore, portMAX_DELAY);

		if (xSemaphoreTake(sdcardAccessSemaphore, SDCARD_IO_WAIT) != pdPASS) {
			LOGGER_WRITELN(("SD card not present"));
			continue;
		}

		fsResult = f_open(&writeFile, SDCARD_SETTINGS_PATH, FA_CREATE_ALWAYS | FA_WRITE);

		if (fsResult) {
			LOGGER_WRITELN(("Error while creating settings file"));
			xSemaphoreGive(sdcardAccessSemaphore);
			continue;
		}

		fsResult = f_write(&writeFile, (const void *)&settings, sizeof(settings), &bytesWritten);

		if (fsResult || bytesWritten != sizeof(settings)) {
			LOGGER_WRITELN(("Unable to write settings"));
		} else {
			LOGGER_WRITELN(("Write Succeed"));
		}

		f_close(&writeFile);
		xSemaphoreGive(sdcardAccessSemaphore);
	}
}
