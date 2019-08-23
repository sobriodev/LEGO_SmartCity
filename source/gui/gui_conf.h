#ifndef GUI_GUI_CONF_H_
#define GUI_GUI_CONF_H_

#include "stdbool.h"
#include "lcd.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* RTOS */
#define TASK_GUI_NAME			"Task-GUI"
#define TASK_GUI_STACK			0x2710
#define TASK_GUI_PRIO			(tskIDLE_PRIORITY + 1)

#define TASK_BACKLIGHT_NAME		"TASK-BACKLIGHT"
#define TASK_BACKLIGHT_STACK	0x3E8
#define TASK_BACKLIGHT_PRIO		(tskIDLE_PRIORITY + 2)

#define TASK_TOUCH_NAME			"TASK-TOUCH"
#define TASK_TOUCH_STACK		0x1F4
#define TASK_TOUCH_PRIO			(tskIDLE_PRIORITY + 1)

/* Timer properties */
#define TIMER_BACKLIGHT_NAME	"TIMER_BACKLIGHT"

#define RTOS_LCD_FROM_ON_TO_DIMMING_MS		pdMS_TO_TICKS(LCD_FROM_ON_TO_DIMMING_MS)
#define RTOS_LCD_FROM_DIMMING_TO_OFF_MS		pdMS_TO_TICKS(LCD_FROM_DIMMING_TO_OFF_MS)

/* 150ms delay on LCD wake up */
#define RTOS_LCD_IDLE_TIME					pdMS_TO_TICKS(150)

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


#endif /* GUI_GUI_CONF_H_ */
