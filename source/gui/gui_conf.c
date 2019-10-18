#include "gui_conf.h"
#include "gui_startup.h"
#include "logger.h"
#include "board_conf.h"
#include "virtual_keyboard.h"
#include "confirm.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task for turning on/off the LCD */
TaskHandle_t xBacklightTask;

/* Backlight timer */
TimerHandle_t xBacklightTimer;

/* Variable indicating touch event presence */
BOARD_TouchInfo_t touchInfo;

/* Blocking dialogs queue */
QueueHandle_t blockingDialogQueue;

/* ----------------------------------------------------------------------------- */
/* --------------------------------- RTOS TASKS -------------------------------- */
/* ----------------------------------------------------------------------------- */

static void GUI_MainTask(void *pvParameters)
{
	GUI_BlockingDialogInfo_t *blockingDialogInfo;

	while (1) {

		/* Check if any dialog window is pending */
		if (xQueueReceive(blockingDialogQueue, &blockingDialogInfo, 0) == pdPASS) {
			switch (blockingDialogInfo->dialog) {
				case DIALOG_VK: { /* Virtual keyboard */
					VK_Params_t *params = (VK_Params_t *)blockingDialogInfo->data;
					VK_InputStatus_t res = VK_GetInput(params);
					WM_MESSAGE feedback;
					feedback.MsgId = MSG_VK;
					feedback.Data.v = res;
					WM_SendMessage(blockingDialogInfo->winSrc, &feedback);
					break;
				}
				case DIALOG_CONFIRM: { /* Confirmation */
					/* Exec confirm dialog */
					CONFIRM_Params_t *params = (CONFIRM_Params_t *)blockingDialogInfo->data;
					CONFIRM_Status_t res = CONFIRM_Exec(params);
					/* Fill feedback values */
					CONFIRM_Feedback_t confirmRes;
					confirmRes.status = res;
					confirmRes.opCode = params->opCode;
					/* Send message to the calling window */
					WM_MESSAGE feedback;
					feedback.MsgId = MSG_CONFIRM;
					feedback.Data.p = &confirmRes;
					WM_SendMessage(blockingDialogInfo->winSrc, &feedback);
					break;
				}
			}
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

				/* Prevent emWin from executing when LCD is not active */
				if (touchInfo.eventLCDState != LCD_ON) {
					vTaskDelay(GUI_LCD_IDLE_TIME);
				}
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
				xTimerChangePeriod(xBacklightTimer, GUI_LCD_FROM_ON_TO_DIMMING_MS, 0);
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
		if (xTimerChangePeriod(xBacklightTimer, GUI_LCD_FROM_DIMMING_TO_OFF_MS, 0) != pdPASS) {
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

static bool GUI_QueuesCreate(void)
{
	blockingDialogQueue = xQueueCreate(1, sizeof(const GUI_BlockingDialogInfo_t *));
	return blockingDialogQueue != NULL;
}

static bool GUI_TimersCreate(void)
{
	/* Backlight timer */
	xBacklightTimer = xTimerCreate(GUI_TIMER_BACKLIGHT_NAME, GUI_LCD_FROM_ON_TO_DIMMING_MS, pdFALSE, NULL, GUI_BacklightTimer);

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
	if (xTaskCreate(GUI_MainTask, GUI_TASK_GUI_NAME, GUI_TASK_GUI_STACK, NULL, GUI_TASK_GUI_PRIO, NULL) != pdPASS) {
		res = false;
	}

	/* Touch task */
	if (xTaskCreate(GUI_TouchTask, GUI_TASK_TOUCH_NAME, GUI_TASK_TOUCH_STACK, NULL, GUI_TASK_TOUCH_PRIO, NULL) != pdPASS) {
		res =  false;
	}

	/* Backlight task */
	if (xTaskCreate(GUI_BacklightTask, GUI_TASK_BACKLIGHT_NAME, GUI_TASK_BACKLIGHT_STACK, NULL, GUI_TASK_BACKLIGHT_PRIO, &xBacklightTask) != pdPASS) {
		res =  false;
	}

	return (res == pdPASS);
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool GUI_RTOSInit(void)
{
	return GUI_TimersCreate() && GUI_TasksCreate() && GUI_QueuesCreate();
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

bool GUI_RequestBlockingDialog(const GUI_BlockingDialogInfo_t *info)
{
	return (xQueueSendToBack(blockingDialogQueue, &info, 0) == pdPASS);
}
