#include "sdcard_conf.h"
#include "logger.h"
#include "settings.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Card descriptor (located in FatFS files) */
extern sd_card_t g_sd;

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

static void SDCARD_IO(const SDCARDIOParams_t *params)
{
	bool res = false;

	/* Take main sd semaphore. If pdFAIL is returned sd card is not inserted or another I/O operation is executing */
	if (xSemaphoreTake(sdcardAccessSemaphore, SDCARD_IO_WAIT) != pdPASS) {
		LOGGER_WRITELN(("SD card not active"));

		xQueueSendToBack(taskResQueue, &res, 0);
		return;
	}

	/* Open settings file */
	FIL settingsFile;
	fsResult = f_open(&settingsFile, SDCARD_SETTINGS_PATH, params->openMode);

	if (fsResult) {
		LOGGER_WRITELN(("Settings file not found"));

		xQueueSendToBack(taskResQueue, &res, 0);
		xSemaphoreGive(sdcardAccessSemaphore);
		return;
	}

	/* Save/load operation */
	if (!params->opFn(&settingsFile) || f_close(&settingsFile)) {
		LOGGER_WRITELN(("SD card IO error"));
	}

	LOGGER_WRITELN(("Settings import/export succeed"));

	xQueueSendToBack(taskResQueue, &res, 0);
	xSemaphoreGive(sdcardAccessSemaphore);
}

/* Function for loading settings from sd card */
static bool SDCARD_Load(FIL *readFile)
{
	Settings_t settingsBuff;
	uint32_t bytesRead;

	/* Read file */
	fsResult = f_read(readFile, &settingsBuff, sizeof(Settings_t), &bytesRead);

	if (fsResult || bytesRead != sizeof(Settings_t)) {
		return false;
	}

	SETTINGS_Replace(&settingsBuff); /* Load succeed. Replace settings */
	return true;
}

/* Function for saving settings on sd card */
static bool SDCARD_Save(FIL *writeFile)
{
	Settings_t *settings = SETTINGS_GetInstance();
	uint32_t bytesWritten;

	/* Write file */
	fsResult = f_write(writeFile, settings, sizeof(Settings_t), &bytesWritten);
	return (!fsResult && bytesWritten == sizeof(Settings_t));
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
	/* Access semaphore */
	sdcardAccessSemaphore = xSemaphoreCreateBinary();

	return (taskResQueue != NULL && sdcardAccessSemaphore != NULL);
}

bool SDCARD_IOGeneric(SDCARDIO_t operation)
{
	bool res;
	SDCARDIOParams_t params;

	switch (operation) {
	case SDCARD_SAVE:
		params.openMode = FA_CREATE_ALWAYS | FA_WRITE;
		params.opFn = SDCARD_Save;
		break;
	case SDCARD_LOAD:
		params.openMode = FA_READ;
		params.opFn = SDCARD_Load;
		break;
	}

	SDCARD_IO(&params);

	xQueueReceive(taskResQueue, &res, portMAX_DELAY); /* Wait for response */
	return res;
}
