#include "gui_conf.h"
#include "gui_startup.h"
#include "logger.h"
#include "board_conf.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task for turning on/off the LCD */
TaskHandle_t xBacklightTask;

/* Backlight timer */
TimerHandle_t xBacklightTimer;

/* Variable indicating touch event presence */
TouchInfo_t touchInfo;

/* ----------------------------------------------------------------------------- */
/* --------------------------------- RTOS TASKS -------------------------------- */
/* ----------------------------------------------------------------------------- */

static void GUI_MainTask(void *pvParameters)
{
	while (1) {
		/* Prevent emWin for executing when LCD is not active */
		if (touchInfo.eventLCDState != LCD_ON) {
			vTaskDelay(RTOS_LCD_IDLE_TIME);
		}
		/* emWin execute */
		WM_Exec();
	}
}

static void GUI_TouchTask(void *pvParameters)
{
	while (1) {
		BOARD_TouchEvent(&touchInfo);

		if (touchInfo.touchPanelStatus == TOUCH_PANEL_STATUS_OK) {

			if (touchInfo.touchEvent) {
				/* Touch event occurred. Send notification to the backlight task */
				xTaskNotifyGive(xBacklightTask);
			}
		}
	}
}

static void GUI_BacklightTask(void *pvParameters)
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

static void GUI_BacklightTimer(TimerHandle_t xTimer)
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

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

static bool GUI_TimersCreate(void)
{
	/* Backlight timer */
	xBacklightTimer = xTimerCreate(TIMER_BACKLIGHT_NAME, RTOS_LCD_FROM_ON_TO_DIMMING_MS, pdFALSE, NULL, GUI_BacklightTimer);

	if (xBacklightTimer == NULL) {
		return false;
	}

	xTimerStart(xBacklightTimer, portMAX_DELAY); /* The scheduler is not started. Just wait for timer command queue free space */

	return true;
}

static bool GUI_TasksCreate(void)
{
	BaseType_t res = pdPASS;

	/* emWin task */
	if (xTaskCreate(GUI_MainTask, TASK_GUI_NAME, TASK_GUI_STACK, NULL, TASK_GUI_PRIO, NULL) != pdPASS) {
		res = false;
	}

	/* Touch task */
	if (xTaskCreate(GUI_TouchTask, TASK_TOUCH_NAME, TASK_TOUCH_STACK, NULL, TASK_TOUCH_PRIO, NULL) != pdPASS) {
		res =  false;
	}

	/* Backlight task */
	if (xTaskCreate(GUI_BacklightTask, TASK_BACKLIGHT_NAME, TASK_BACKLIGHT_STACK, NULL, TASK_BACKLIGHT_PRIO, &xBacklightTask) != pdPASS) {
		res =  false;
	}

	return (res == pdPASS);
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool GUI_RTOSInit(void)
{
	return GUI_TimersCreate() && GUI_TasksCreate();
}

void GUI_Start(void)
{
	/* emWin start */
	GUI_Init();

	/* Enable auto-buffering */
	WM_MULTIBUF_Enable(1);

	/* Set size for the background window */
	WM_SetSize(WM_HBKWIN, LCD_WIDTH, LCD_HEIGHT);

	/* Set callback for the background window */
	WM_SetCallback(WM_HBKWIN, GUI_StartupCallback);

	/* All buttons should react on level */
	BUTTON_SetReactOnLevel();

	/* Start GUI */
	WM_Exec();
}

void GUI_FailedHook(void)
{
	LOGGER_WRITELN(("emWin widget creation error. Check stack and heap usage"));
}
