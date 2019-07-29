#include "rtos.h"
#include "board_conf.h"
#include "board.h"
#include "logger.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

#define RTOS_LCD_FROM_ON_TO_DIMMING_MS		pdMS_TO_TICKS(LCD_FROM_ON_TO_DIMMING_MS)
#define RTOS_LCD_FROM_DIMMING_TO_OFF_MS		pdMS_TO_TICKS(LCD_FROM_DIMMING_TO_OFF_MS)

/* 150ms delay on LCD wake up */
#define RTOS_LCD_IDLE_TIME					pdMS_TO_TICKS(150)

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Main GUI task */
TaskHandle_t xGuiTask;

/* Task for turning on/off the LCD */
TaskHandle_t xBacklightTask;

/* Backlight timer */
TimerHandle_t xBacklightTimer;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

bool RTOS_TimersCreate(void)
{
	/* Backlight timer */
	xBacklightTimer = xTimerCreate(TIMER_BACKLIGHT_NAME, RTOS_LCD_FROM_ON_TO_DIMMING_MS, pdFALSE, NULL, vTimerBacklight);

	if (xBacklightTimer == NULL) {
		return false;
	}

	xTimerStart(xBacklightTimer, portMAX_DELAY); /* The scheduler is not started. Just wait for timer command queue free space */

	return true;
}

bool RTOS_TasksCreate(void)
{
	/* emWin task */
	if (xTaskCreate(vTaskGUI, TASK_GUI_NAME, TASK_GUI_STACK, NULL, TASK_GUI_PRIO, &xGuiTask) != pdPASS) {
		LOGGER_WRITELN(CRITICAL_LOG, LOG_ERROR, "GUI (emWin) task creation failed");
		return false;
	}

	/* Backlight task */
	if (xTaskCreate(vTaskBacklight, TASK_BACKLIGHT_NAME, TASK_BACKLIGHT_STACK, NULL, TASK_BACKLIGHT_PRIO, &xBacklightTask) != pdPASS) {
		LOGGER_WRITELN(CRITICAL_LOG, LOG_ERROR, "Backlight task creation failed");
		return false;
	}

	return true;
}

/* ----------------------------------------------------------------------------- */
/* ---------------------------- API FUNCTIONS (TASKS)--------------------------- */
/* ----------------------------------------------------------------------------- */

void vTaskGUI(void *pvParameters)
{
	/* Variable indicating touch event presence */
	TouchInfo_t touchInfo;

    while (1) {
    	BOARD_TouchEvent(&touchInfo);

    	if (touchInfo.touchPanelStatus == TOUCH_PANEL_STATUS_OK) {

    		if (touchInfo.touchEvent) {
        		/* Touch event occurred. Send notification to the backlight task */
        		xTaskNotifyGive(xBacklightTask);

        		/* Prevent emWin for executing when LCD is not active */
        		if (touchInfo.eventLCDState != LCD_ON) {
        			vTaskDelay(RTOS_LCD_IDLE_TIME);
        		}
    		}

        	/* emWin execute */
            GUI_MULTIBUF_Begin();
            WM_Exec();
            GUI_MULTIBUF_End();
    	}
    }
}

void vTaskBacklight(void *pvParameters)
{
	while (1) {
		if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 0) {
			switch (LCD_GetState()) {
			case LCD_ON:
				/* In the worst case the timer command queue will be full and LCD will be turned off faster */
				xTimerReset(xBacklightTimer, 0);
				break;
			case LCD_DIMMING:
			case LCD_OFF:
			default:
				/* In the worst case the timer command queue will be full and LCD will change the state from ON to DIMMING faster */
				xTimerChangePeriod(xBacklightTimer, RTOS_LCD_FROM_ON_TO_DIMMING_MS, 0);
				LCD_SetState(LCD_ON);
			break;
			}
		}
	}
}

/* ----------------------------------------------------------------------------- */
/* --------------------------- API FUNCTIONS (TIMERS) -------------------------- */
/* ----------------------------------------------------------------------------- */

void vTimerBacklight(TimerHandle_t xTimer)
{
	switch (LCD_GetState()) {
	case LCD_ON:
		/* In the worst case the timer command queue will be full and LCD will not turn off */
		if (xTimerChangePeriod(xBacklightTimer, RTOS_LCD_FROM_DIMMING_TO_OFF_MS, 0) != pdPASS) {
			LCD_SetState(LCD_ON);
			break;
		}
		LCD_SetState(LCD_DIMMING);
		break;
	case LCD_DIMMING:
		LCD_SetState(LCD_OFF);
		break;
	default:
		break;
	}
}
