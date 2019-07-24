#ifndef RTOS_H_
#define RTOS_H_

#include "stdbool.h"
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task properties */
#define TASK_GUI_NAME			"Task-GUI"
#define TASK_GUI_STACK			0x2710
#define TASK_GUI_PRIO			(tskIDLE_PRIORITY + 1)

#define TASK_BACKLIGHT_NAME		"TASK-BACKLIGHT"
#define TASK_BACKLIGHT_STACK	0x3E8
#define TASK_BACKLIGHT_PRIO		(tskIDLE_PRIORITY + 1)

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Create all application tasks
 *
 * \return True if tasks creation succeed, false otherwise
 */
bool RTOS_TasksCreate(void);


#endif /* RTOS_H_ */
