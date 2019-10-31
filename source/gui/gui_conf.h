#ifndef GUI_GUI_CONF_H_
#define GUI_GUI_CONF_H_

#include "stdbool.h"
#include "lcd.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* RTOS */
#define GUI_TASK_GUI_NAME			"TASK-GUI"
#define GUI_TASK_GUI_STACK			0x2710
#define GUI_TASK_GUI_PRIO			(tskIDLE_PRIORITY + 1)

#define GUI_TASK_TOUCH_NAME			"TASK-TOUCH"
#define GUI_TASK_TOUCH_STACK		0x1F4
#define GUI_TASK_TOUCH_PRIO			(tskIDLE_PRIORITY + 1)

#define GUI_TASK_BACKLIGHT_NAME		"TASK-BACKLIGHT"
#define GUI_TASK_BACKLIGHT_STACK	0x3E8
#define GUI_TASK_BACKLIGHT_PRIO		(tskIDLE_PRIORITY + 2)

/* Timer properties */
#define GUI_TIMER_BACKLIGHT_NAME	"TIMER_BACKLIGHT"
#define GUI_LCD_FROM_DIMMING_TO_OFF_MS		pdMS_TO_TICKS(LCD_FROM_DIMMING_TO_OFF_MS)

/* 200ms delay on LCD wake up */
#define GUI_LCD_IDLE_TIME					pdMS_TO_TICKS(200)

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Possible blocking dialogs
 */
typedef enum {
	DIALOG_VK,		//!< Virtual keyboard dialog
	DIALOG_CONFIRM	//!< Confirmation dialog
} GUI_BlockingDialog_t;

/*!
 * \brief Messages sent back when dialog window returned
 */
typedef enum {
	MSG_VK = WM_USER,	//!< Virtual keyboard dialog returned
	MSG_CONFIRM			//!< Confirmation/alert dialog returned
} GUI_UserMsgId_t;

/*!
 * \brief Blocking dialog info
 */
typedef struct {
	WM_HWIN srcWin; 				//!< Calling window
	GUI_BlockingDialog_t dialog;	//!< Desired dialog. See GUI_BlockingDialog_t for more information
	const void *data; 				//!< Optional data pointer. It has to be valid until dialog returns
} GUI_BlockingDialogInfo_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Create GUI tasks
 *
 * \return True if creation succeed, false otherwise
 */
bool GUI_RTOSInit(void);

/*!
 * \brief Main emWin initialization function
 */
void GUI_Start(void);

/*!
 * \brief Called after emWin cannot create widget
 */
void GUI_FailedHook(void);

/*!
 * \brief Execute blocking dialog
 *
 * \param info : Base address of GUI_BlockingDialogInfo_t. The pointer has to be valid until dialog returns
 * \return True if request was stored in dialog queue, false otherwise
 */
bool GUI_RequestBlockingDialog(const GUI_BlockingDialogInfo_t *info);

/*!
 * \brief Reload LCD timer after changing settings
 */
void GUI_UpdateLcdBlankingStatus(void);

#endif /* GUI_GUI_CONF_H_ */
