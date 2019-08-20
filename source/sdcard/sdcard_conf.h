#ifndef SDCARD_SDCARD_CONF_H_
#define SDCARD_SDCARD_CONF_H_

/* FatFS/sdmmc*/
#include "stdio.h"
#include "fsl_sd.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "stdbool.h"
#include "fsl_iocon.h"
#include "ff.h"
#include "diskio.h"
#include "board_conf.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Data block count accessed in card */
#define SDCARD_DATA_BLOCK_COUNT (5U)
/* Data buffer size. */
#define SDCARD_DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * SDCARD_DATA_BLOCK_COUNT)

#define TASK_SDDETECT_NAME	"TASK_SDDETECT"
#define TASK_SDDETECT_STACK	(SDCARD_DATA_BUFFER_SIZE + 1000U)
#define TASK_SDDETECT_PRIO 	(configMAX_PRIORITIES - 1U)

#define TASK_SDLOAD_NAME	"TASK_SDLOAD"
#define TASK_SDLOAD_STACK	0x1F4
#define TASK_SDLOAD_PRIO	(configMAX_PRIORITIES - 2U)

#define TASK_SDSAVE_NAME	"TASK_SDSAVE"
#define TASK_SDSAVE_STACK	0x1F4
#define TASK_SDSAVE_PRIO	(configMAX_PRIORITIES - 2U)

#define SDCARD_IO_WAIT		pdMS_TO_TICKS(20)

#define SDCARD_SETTINGS_PATH	"/conf.dat"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

// todo docs
typedef struct {
	int value1;
	int value2;
} SDCARD_Conf_h;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API_FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

bool SDCARD_RTOSInit(void);

void SDCARD_LoadSettings(void);

void SDCARD_SaveSettings(void);


#endif /* SDCARD_SDCARD_CONF_H_ */
