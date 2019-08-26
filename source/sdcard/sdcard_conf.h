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
#include "queue.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Data block count accessed in card */
#define SDCARD_DATA_BLOCK_COUNT (5U)
/* Data buffer size. */
#define SDCARD_DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * SDCARD_DATA_BLOCK_COUNT)

#define SDCARD_IO_WAIT		pdMS_TO_TICKS(20)

/* The file in which settings will be stored */
#define SDCARD_SETTINGS_PATH	"/settings.dat"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief IO operation type
 */
typedef enum {
	SDCARD_SAVE,	//!< Save operation
	SDCARD_LOAD 	//!< Load operation
} SDCARDIO_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API_FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Search and mount SD card
 * @return True if succeed, false otherwise
 */
bool SDCARD_Init(void);

/*!
 * \brief Init SD card tasks and event objects
 *
 * \return True if creation passed, false otherwise
 */
bool SDCARD_RTOSInit(void);

/*!
 * \brief Start save/load task
 *
 * \param operation : Chosen operation. See SDCARD_IO_t for more information
 * \return True if operation succeed, false otherwise
 * \note This is only helper function. Use SDCARD_ExportSettings and SDCARD_ImportSettings functions for convenience
 */
bool SDCARD_IOGeneric(SDCARDIO_t operation);

/*!
 * \brief Save current settings on SD card
 *
 * \return True if operation succeed, false otherwise
 */
static inline bool SDCARD_ExportSettings(void)
{
	return SDCARD_IOGeneric(SDCARD_SAVE);
}

/*!
 * \brief Load settings from SD card
 *
 * \return True if operation succeed, false otherwise
 * \note On successful load current settings will be replaced with new ones
 */
static inline bool SDCARD_ImportSettings(void)
{
	return SDCARD_IOGeneric(SDCARD_LOAD);
}

#endif /* SDCARD_SDCARD_CONF_H_ */
