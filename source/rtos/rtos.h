#ifndef RTOS_H_
#define RTOS_H_

#include "stdbool.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task properties */
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

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Create application timers
 *
 * \return True if timers creation succeed, false otherwise
 */
bool RTOS_TimersCreate(void);

/*!
 * \brief Create all application tasks
 *
 * \return True if tasks creation succeed, false otherwise
 */
bool RTOS_TasksCreate(void);

/* ----------------------------------------------------------------------------- */
/* ---------------------------- API FUNCTIONS (TASKS) -------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Main emWin GUI task
 *
 * \param pvParameters : Task parameter
 */
void vTaskGUI(void *pvParameters);

/*!
 * \brief Backlight task
 *
 * \param pvParameters : Task parameter
 */
void vTaskBacklight(void *pvParameters);


/* ----------------------------------------------------------------------------- */
/* --------------------------- API FUNCTIONS (TIMERS) -------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Backlight timer callback function
 *
 * \param xTimer : Instance of TimerHandle_t
 */
void vTimerBacklight(TimerHandle_t xTimer);


#endif /* RTOS_H_ */
