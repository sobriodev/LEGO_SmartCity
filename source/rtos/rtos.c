#include "rtos.h"
#include "task_def.h"

#include "FreeRTOS.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Main GUI task */
TaskHandle_t guiTask;

/* Task for turning on/off the LCD */
TaskHandle_t backlightTask;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

bool RTOS_TasksCreate(void)
{
	BaseType_t res;

	/* GUI task */
	res = xTaskCreate(vTaskGUI, TASK_GUI_NAME, TASK_GUI_STACK, NULL, TASK_GUI_PRIO, &guiTask);

	if (res != pdPASS) {
		//todo change to warning
		return false;
	}

	/* Backlight task */
	res = xTaskCreate(vTaskBacklight, TASK_BACKLIGHT_NAME, TASK_BACKLIGHT_STACK, NULL, TASK_BACKLIGHT_PRIO, &backlightTask);

	if (res != pdPASS) {
		return false;
	}

	return true;
}
