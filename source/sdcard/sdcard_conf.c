#include "sdcard_conf.h"
#include "logger.h"
#include "settings.h"

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
		.cdTimeOut_ms = (~0U)
};

/* Card semaphores  */
static SemaphoreHandle_t sdcardAccessSemaphore;

/* FatFS related */
static const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
static FATFS fatfsFileSystem; /* File system object */
static FRESULT fsResult;

/* Queue for returning results from load/save tasks */
static QueueHandle_t taskResQueue;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

static bool SDCARD_FSInit(void)
{
	/* Mount filesystem */
	fsResult = f_mount(&fatfsFileSystem, driverNumberBuffer, 0U);
    if (fsResult) {
    	return false;
    }

    /* Change drive */
    fsResult = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (fsResult) {
        return false;
    }

    return true;
}

/* Function for detecting and mounting sd card. It is called only once on startup */
static void SDCARD_Detect(void)
{
	g_sd.host.base = SD_HOST_BASEADDR;
	g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
	g_sd.usrParam.cd = &s_sdCardDetect;

	bool result = false;

	/* SD host init */
	if (SD_HostInit(&g_sd) == kStatus_Success) {

		if (SD_IsCardPresent(&g_sd)) {

			/* Reset SD power */
			SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
			SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);

			/* Init card */
			if (SD_CardInit(&g_sd)) {
				LOGGER_WRITELN(("SD card init failed"));
				xQueueSendToBack(taskResQueue, &result, 0);
				return;
			}

			/* FatFS init */
			if (!SDCARD_FSInit()) {
				LOGGER_WRITELN(("SD card filesystem error"));
				xQueueSendToBack(taskResQueue, &result, 0);
				return;
			}

			LOGGER_WRITELN(("SD card ready"));

			/* Everything OK. Give access semaphore */
			xSemaphoreGive(sdcardAccessSemaphore);

		} else {
			LOGGER_WRITELN(("SD card not found"));
			xQueueSendToBack(taskResQueue, &result, 0);
			return;
		}

	}
	else {
		LOGGER_WRITELN(("SD host init failed"));
		xQueueSendToBack(taskResQueue, &result, 0);
		return;
	}

	result = true;
	xQueueSendToBack(taskResQueue, &result, 0);
}

/* Function for loading settings from sd card */
static void SDCARD_Load(void)
{
	FIL readFile;
	Settings_t settingsBuff;
	uint32_t bytesRead;

	bool taskResult = false;

	/* Take main sd semaphore. If pdFAIL is returned sd card is not inserted or another I/O operation is executing */
	if (xSemaphoreTake(sdcardAccessSemaphore, SDCARD_IO_WAIT) != pdPASS) {
		LOGGER_WRITELN(("SD card not active"));

		xQueueSendToBack(taskResQueue, &taskResult, 0);
		return;
	}

	/* Open file */
	fsResult = f_open(&readFile, SDCARD_SETTINGS_PATH, FA_READ);

	if (fsResult) {
		LOGGER_WRITELN(("Settings file not found"));

		xQueueSendToBack(taskResQueue, &taskResult, 0);
		xSemaphoreGive(sdcardAccessSemaphore);
		return;
	}

	/* Read file */
	fsResult = f_read(&readFile, &settingsBuff, sizeof(Settings_t), &bytesRead);

	if (fsResult || bytesRead != sizeof(Settings_t)) {
		LOGGER_WRITELN(("Unable to read settings. Corrupted file"));
	} else {
		LOGGER_WRITELN(("Settings successfully loaded"));

		SETTINGS_Replace(&settingsBuff); /* Load succeed. Replace settings */
		taskResult = true;
	}

	xQueueSendToBack(taskResQueue, &taskResult, 0);

	f_close(&readFile);
	xSemaphoreGive(sdcardAccessSemaphore);
}

/* Function for saving settings on sd card */
static void SDCARD_Save(void)
{
	FIL writeFile;
	uint32_t bytesWritten;
	Settings_t *settings = SETTINGS_GetInstance();

	bool taskResult = false;

	/* Take main sd semaphore. If pdFAIL is returned sd card is not inserted or another I/O operation is executing */
	if (xSemaphoreTake(sdcardAccessSemaphore, SDCARD_IO_WAIT) != pdPASS) {
		LOGGER_WRITELN(("SD card not active"));

		xQueueSendToBack(taskResQueue, &taskResult, 0);
		return;
	}

	/* Open file */
	fsResult = f_open(&writeFile, SDCARD_SETTINGS_PATH, FA_CREATE_ALWAYS | FA_WRITE);

	if (fsResult) {
		LOGGER_WRITELN(("Error while creating settings file"));

		xQueueSendToBack(taskResQueue, &taskResult, 0);
		xSemaphoreGive(sdcardAccessSemaphore);
		return;
	}

	/* Write file */
	fsResult = f_write(&writeFile, settings, sizeof(Settings_t), &bytesWritten);

	if (fsResult || bytesWritten != sizeof(Settings_t)) {
		LOGGER_WRITELN(("Unable to write settings"));
	} else {
		LOGGER_WRITELN(("Settings successfully saved"));
		taskResult = true;
	}

	xQueueSendToBack(taskResQueue, &taskResult, 0);

	f_close(&writeFile);
	xSemaphoreGive(sdcardAccessSemaphore);
}

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API_FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

bool SDCARD_Init(void)
{
	bool res;
	SDCARD_Detect();
	xQueueReceive(taskResQueue, &res, portMAX_DELAY); /* Wait for response */
	return res;
}

bool SDCARD_RTOSInit(void)
{
	/* Result queue */
	taskResQueue = xQueueCreate(1, sizeof(bool));

	if (taskResQueue == NULL) {
		return false;
	}

	/* Semaphores */
	sdcardAccessSemaphore = xSemaphoreCreateBinary();

	return (sdcardAccessSemaphore != NULL);
}

bool SDCARD_IOGeneric(SDCARDIO_t operation)
{
	bool res;

	switch (operation) {
	case SDCARD_SAVE:
		SDCARD_Save();
		break;
	case SDCARD_LOAD:
		SDCARD_Load();
		break;
	}

	xQueueReceive(taskResQueue, &res, portMAX_DELAY); /* Wait for response */
	return res;
}
