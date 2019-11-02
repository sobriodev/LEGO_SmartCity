#include "gui_settings.h"
#include "gui_conf.h"
#include "virtual_keyboard.h"
#include "confirm.h"
#include "settings.h"
#include "conv.h"
#include "board_conf.h"
#include "lcd.h"
#include "sdcard_conf.h"
#include "logger.h"

#include "string.h"

/* emWin */
#include "MULTIPAGE.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "TEXT.h"
#include "DROPDOWN.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Dimensions */
#define GUI_SETT_X						0
#define GUI_SETT_Y						0
#define GUI_SETT_WIDTH					LCD_WIDTH
#define GUI_SETT_HEIGHT					LCD_HEIGHT
#define GUI_SETT_BTN_WIDTH				60
#define GUI_SETT_BTN_HEIGHT				30
#define GUI_SETT_MPAGE_HEIGHT			222
#define GUI_SETT_MPAGE_TAB_HEIGHT		30
#define GUI_SETT_DIALOG_HEIGHT			(GUI_SETT_MPAGE_HEIGHT - GUI_SETT_MPAGE_TAB_HEIGHT)
#define GUI_SETT_EDIT_WIDTH				120
#define GUI_SETT_EDIT_HEIGHT 			30
#define GUI_SETT_TEXT_WIDTH 			80
#define GUI_SETT_TEXT_HEIGHT 			30
#define GUI_SETT_SLIDER_WIDTH			120
#define GUI_SETT_SLIDER_HEIGHT			30
#define GUI_SETT_DROPDOWN_WIDTH			120
#define GUI_SETT_DROPDOWN_HEIGHT		80 /* Expanded */
#define GUI_SETT_DROPDOWN_TEXT_HEIGHT	25 /* Closed */

/* GUI IDs */
#define GUI_SETT_MPAGE					GUI_ID_MULTIPAGE0
#define GUI_SETT_SAVE					GUI_ID_BUTTON0
#define GUI_SETT_SERVER_IP				GUI_ID_EDIT0
#define GUI_SETT_SERVER_SM				GUI_ID_EDIT1
#define GUI_SETT_SERVER_GW				GUI_ID_EDIT2
#define GUI_SETT_SERVER_DNS				GUI_ID_EDIT3
#define GUI_SETT_LCD_BRIGHTNESS			GUI_ID_SLIDER0
#define GUI_SETT_LCD_BLANKING			GUI_ID_DROPDOWN0
#define GUI_SETT_MISC_RESTORE			GUI_ID_BUTTON0

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE DATA TYPES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Dialog entity */
typedef struct {
	const GUI_WIDGET_CREATE_INFO *info;
	uint8_t widgetsCount;
	WM_CALLBACK *callback;
	const char *txt;
} GUI_SettingsDialog_t;

/* LCD dimming dropdown structure */
typedef struct {
	const char *txt;
	LCD_DimmingTime_t time;
} GUI_SettingsLCDDimming_t;

/* Op-codes used in dialog windows */
typedef enum {
	SAVE_OP,
	RESTORE_OP,
	SDCARD_NOT_FOUND_OP
} GUI_SettingsOpCodes;

/* ----------------------------------------------------------------------------- */
/* -------------------------- PRIVATE FUNCTIONS HEADERS ------------------------ */
/* ----------------------------------------------------------------------------- */

static void GUI_SettingsServerCallback(WM_MESSAGE *pMsg);
static void GUI_SettingsLCDCallback(WM_MESSAGE *pMsg);
static void GUI_SettingsMiscCallback(WM_MESSAGE *pMsg);

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Server settings dialog */
static const GUI_WIDGET_CREATE_INFO serverDialog[] = {
		{ WINDOW_CreateIndirect, NULL,                    0,                   0,   0,   GUI_SETT_WIDTH,      GUI_SETT_DIALOG_HEIGHT, 0,                               0,  0 }, /* Main window */
		{ TEXT_CreateIndirect,   GUI_SETT_SERVER_IP_TXT,  0,                   10,  14,  GUI_SETT_TEXT_WIDTH, GUI_SETT_TEXT_HEIGHT,   TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0,  0 },
		{ EDIT_CreateIndirect,   NULL,                    GUI_SETT_SERVER_IP,  100, 14,  GUI_SETT_EDIT_WIDTH, GUI_SETT_EDIT_HEIGHT,   GUI_TA_LEFT | GUI_TA_VCENTER,    15, 0 },
		{ TEXT_CreateIndirect,   GUI_SETT_SERVER_SM_TXT,  0,                   10,  58,  GUI_SETT_TEXT_WIDTH, GUI_SETT_TEXT_HEIGHT,   TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0,  0 },
		{ EDIT_CreateIndirect,   NULL,                    GUI_SETT_SERVER_SM,  100, 58,  GUI_SETT_EDIT_WIDTH, GUI_SETT_EDIT_HEIGHT,   GUI_TA_LEFT | GUI_TA_VCENTER,    15, 0 },
		{ TEXT_CreateIndirect,   GUI_SETT_SERVER_GW_TXT,  0,                   10,  102, GUI_SETT_TEXT_WIDTH, GUI_SETT_TEXT_HEIGHT,   TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0,  0 },
		{ EDIT_CreateIndirect,   NULL,                    GUI_SETT_SERVER_GW,  100, 102, GUI_SETT_EDIT_WIDTH, GUI_SETT_EDIT_HEIGHT,   GUI_TA_LEFT | GUI_TA_VCENTER,    15, 0 },
		{ TEXT_CreateIndirect,   GUI_SETT_SERVER_DNS_TXT, 0,                   10,  146, GUI_SETT_TEXT_WIDTH, GUI_SETT_TEXT_HEIGHT,   TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0,  0 },
		{ EDIT_CreateIndirect,   NULL,                    GUI_SETT_SERVER_DNS, 100, 146, GUI_SETT_EDIT_WIDTH, GUI_SETT_EDIT_HEIGHT,   GUI_TA_LEFT | GUI_TA_VCENTER,    10, 0 }
};

/* LCD settings dialog */
static const GUI_WIDGET_CREATE_INFO lcdDialog[] = {
		{ WINDOW_CreateIndirect,   NULL,                        0,                       0,   0,  GUI_SETT_WIDTH,         GUI_SETT_DIALOG_HEIGHT,    0,                               0, 0 }, /* Main window */
		{ TEXT_CreateIndirect,     GUI_SETT_LCD_BRIGHTNESS_TXT, 0,                       10,  14, GUI_SETT_TEXT_WIDTH,    GUI_SETT_TEXT_HEIGHT,      TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0, 0 },
		{ SLIDER_CreateIndirect,   NULL,                        GUI_SETT_LCD_BRIGHTNESS, 100, 14, GUI_SETT_SLIDER_WIDTH,  GUI_SETT_SLIDER_HEIGHT,    SLIDER_CF_HORIZONTAL,            0, 0 },
		{ TEXT_CreateIndirect,     GUI_SETT_LCD_BLANKING_TXT,   0,                       10,  58, GUI_SETT_TEXT_WIDTH,    GUI_SETT_TEXT_HEIGHT,      TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0, 0 },
		{ DROPDOWN_CreateIndirect, NULL,                        GUI_SETT_LCD_BLANKING,   100, 58, GUI_SETT_DROPDOWN_WIDTH, GUI_SETT_DROPDOWN_HEIGHT, DROPDOWN_CF_AUTOSCROLLBAR,       0, 0 },
};

/* Misc settings dialog */
static const GUI_WIDGET_CREATE_INFO miscDialog[] = {
		{ WINDOW_CreateIndirect, NULL,                          0,                     0,   0,  GUI_SETT_WIDTH,      GUI_SETT_DIALOG_HEIGHT, 0,                               0, 0 }, /* Main window */
		{ TEXT_CreateIndirect,   GUI_SETT_MISC_RESTORE_TXT,     0,                     10,  14, GUI_SETT_TEXT_WIDTH, GUI_SETT_TEXT_HEIGHT,   TEXT_CF_RIGHT | TEXT_CF_VCENTER, 0, 0 },
		{ BUTTON_CreateIndirect, GUI_SETT_MISC_RESTORE_BTN_TXT, GUI_SETT_MISC_RESTORE, 100, 14, GUI_SETT_BTN_WIDTH,  GUI_SETT_BTN_HEIGHT,    0,                               0, 0 }
};

/* Settings dialogs */
static GUI_SettingsDialog_t settingsDialogs[] = {
		{ serverDialog, GUI_COUNTOF(serverDialog), GUI_SettingsServerCallback, GUI_SETT_SERVER_TXT },
		{ lcdDialog,    GUI_COUNTOF(lcdDialog),    GUI_SettingsLCDCallback,    GUI_SETT_LCD_TXT },
		{ miscDialog,   GUI_COUNTOF(miscDialog),   GUI_SettingsMiscCallback,   GUI_SETT_MISC_TXT }
};

static GUI_SettingsLCDDimming_t lcdblankingTimes[] = {
		{"Always on", LCD_ALWAYS_ON},
		{"10s", LCD_DIMMING_10S},
		{"30s", LCD_DIMMING_30S},
		{"2min", LCD_DIMMING_2MIN},
		{"5min", LCD_DIMMING_5MIN}
};

static const char *serverEditsTxt[] = {GUI_SETT_SERVER_IP_TXT, GUI_SETT_SERVER_SM_TXT, GUI_SETT_SERVER_GW_TXT, GUI_SETT_SERVER_DNS_TXT};
static char settingsBuffer[16]; /* Buffer for converting ip addresses and DNS name */

static WM_HWIN selfWin; /* Handle of settings window */

static SETTINGS_t *settings; /* Current settings */

static bool restartRequired; /* Flag indicating whether board restart is required */

static GUI_BlockingDialogInfo_t dialogInfo; /* Blocking dialog info */

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Function for easier and faster keyboard dialog opening */
static void GUI_SettingsVKDialog(WM_HWIN srcWin, EDIT_Handle inputHandle, uint8_t maxLen, const char *inputDesc, VALIDATOR_Fn_t validatorFn, uint32_t opCode)
{
	/* Must have lifetime of the program */
	static VK_Params_t params;

	/* Fill dialog info */
	params.copyVal = true;
	params.inputHandle = inputHandle;
	params.maxLen = maxLen;
	params.inputDesc = inputDesc;
	params.validatorFn = validatorFn;
	params.opCode = opCode;

	dialogInfo.srcWin = srcWin;
	dialogInfo.dialog = DIALOG_VK;
	dialogInfo.data = &params;

	GUI_RequestBlockingDialog(&dialogInfo);
}

/* Function for easier and faster confirm dialog opening */
static void GUI_SettingsConfirmDialog(WM_HWIN srcWin, CONFIRM_Type_t type, const char *msg, uint32_t opCode)
{
	/* Must have lifetime of the program */
	static CONFIRM_Params_t params;

	params.type = type;
	params.opCode = opCode;
	params.msg = msg;

	dialogInfo.srcWin = srcWin;
	dialogInfo.dialog = DIALOG_CONFIRM;
	dialogInfo.data = &params;

	GUI_RequestBlockingDialog(&dialogInfo);
}

/* Server settings callback */
static void GUI_SettingsServerCallback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		/* Set current values */
		CONV_IP4CompresssedToStr(settings->httpsrvIp, settingsBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_IP), settingsBuffer);
		CONV_IP4CompresssedToStr(settings->httpsrvSm, settingsBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_SM), settingsBuffer);
		CONV_IP4CompresssedToStr(settings->httpsrvGw, settingsBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_GW), settingsBuffer);
		/* DNS from settings directly */
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_DNS), settings->httpsrvDnsName);
		break;
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
		break;
	case WM_NOTIFY_PARENT:

		widgetId = WM_GetId(pMsg->hWinSrc);

		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED) {
			if (widgetId >= GUI_SETT_SERVER_IP && widgetId < GUI_SETT_SERVER_DNS) {
				/* IP, SM, GW */
				GUI_SettingsVKDialog(pMsg->hWin, WM_GetDialogItem(pMsg->hWin, widgetId), VALIDATOR_IP4_LEN, serverEditsTxt[widgetId - GUI_SETT_SERVER_IP], VALIDATOR_IsIp4, widgetId);
			} else if (widgetId == GUI_SETT_SERVER_DNS) {
				/* DNS name */
				GUI_SettingsVKDialog(pMsg->hWin, WM_GetDialogItem(pMsg->hWin, widgetId), VALIDATOR_DNS_LEN, serverEditsTxt[widgetId - GUI_SETT_SERVER_IP], VALIDATOR_IsDnsName, widgetId);
			}
		}

		break;

	case MSG_VK: {
		const VK_Feedback_t *feedback = (const VK_Feedback_t *)pMsg->Data.p;

		switch (feedback->status) {
		case VK_STORED:
			/* Set restart flag */
			restartRequired = true;

			/* Get raw text */
			EDIT_GetText(WM_GetDialogItem(pMsg->hWin, feedback->opCode), settingsBuffer, 16);

			switch (feedback->opCode) {
			case GUI_SETT_SERVER_IP:
				CONV_IP4StrToCompressed(settingsBuffer, &settings->httpsrvIp);
				break;
			case GUI_SETT_SERVER_SM:
				CONV_IP4StrToCompressed(settingsBuffer, &settings->httpsrvSm);
				break;
			case GUI_SETT_SERVER_GW:
				CONV_IP4StrToCompressed(settingsBuffer, &settings->httpsrvGw);
				break;
			case GUI_SETT_SERVER_DNS:
				strcpy(settings->httpsrvDnsName, settingsBuffer);
				break;
			}

			break;
		case VK_NON_VALID:
			LOGGER_WRITELN(("Virtual keyboard validation error"));
			break;
		case VK_FAILURE:
			GUI_FailedHook();
			break;
		}
		break;
	}

	default:
		WM_DefaultProc(pMsg);
	}
}

/* LCD settings callback */
static void GUI_SettingsLCDCallback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		/* Brightness */
		SLIDER_SetRange(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BRIGHTNESS), LCD_MIN_BRIGHTNESS, LCD_MAX_BRIGHTNESS);
		SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BRIGHTNESS), settings->lcdBrightness);

		/* Dimming dropdown */
		DROPDOWN_SetTextHeight(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), GUI_SETT_DROPDOWN_TEXT_HEIGHT);
		DROPDOWN_SetTextAlign(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), TEXT_CF_VCENTER | TEXT_CF_HCENTER);
		for (uint8_t i = 0; i < GUI_COUNTOF(lcdblankingTimes); i++) {
			DROPDOWN_AddString(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), lcdblankingTimes[i].txt);
			/* Choose correct selection */
			if (settings->lcdDimmingTime == lcdblankingTimes[i].time) {
				DROPDOWN_SetSel(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), i);
			}
		}
		break;
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);

		if (widgetId == GUI_SETT_LCD_BRIGHTNESS && pMsg->Data.v == WM_NOTIFICATION_VALUE_CHANGED) {
			/* Update brightness setting */
			uint8_t sliderVal = SLIDER_GetValue(pMsg->hWinSrc);
			settings->lcdBrightness = sliderVal;
			LCD_SET_BRIGHNTESS(sliderVal);
		} else if (widgetId == GUI_SETT_LCD_BLANKING && pMsg->Data.v == WM_NOTIFICATION_SEL_CHANGED) {
			/* Update dimming time setting */
			settings->lcdDimmingTime = lcdblankingTimes[DROPDOWN_GetSel(pMsg->hWinSrc)].time;
			GUI_UpdateLcdBlankingStatus();
		}

		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

static void GUI_SettingsMiscCallback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;

	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:

		widgetId = WM_GetId(pMsg->hWinSrc);

		if (widgetId == GUI_SETT_SAVE && pMsg->Data.v == WM_NOTIFICATION_CLICKED) {
			GUI_SettingsConfirmDialog(pMsg->hWin, CONFIRM_CONFIRM, GUI_SETT_RESTORE_TXT, RESTORE_OP);
		}

		break;
	case MSG_CONFIRM: {
		const CONFIRM_Feedback_t *feedback = (const CONFIRM_Feedback_t *)pMsg->Data.p;

		if (feedback->opCode == RESTORE_OP) {
			switch (feedback->status) {
			case CONFIRM_YES:
				/* Restore settings */
				if (!SDCARD_IsPresent()) {
					GUI_SettingsConfirmDialog(pMsg->hWin, CONFIRM_ALERT, GUI_SETT_SDCARD_NOT_FOUND_TXT, SDCARD_NOT_FOUND_OP);
				} else {
					SETTINGS_RestoreDefaults();
					SDCARD_ExportSettings();
					BOARD_SystemReset();
				}
				break;
			case CONFIRM_NO:
				LOGGER_WRITELN(("Confirm dialog rejected"));
				break;
			case CONFIRM_FAILURE:
				GUI_FailedHook();
				break;
			}
		}

		break;
	}
	default:
		WM_DefaultProc(pMsg);
	}
}

static bool GUI_SettingsInitWidgets(void)
{
	/* Create widgets */
	MULTIPAGE_Handle multipage = MULTIPAGE_CreateEx(0, 0, GUI_SETT_WIDTH, GUI_SETT_MPAGE_HEIGHT, selfWin, WM_CF_SHOW, 0, GUI_SETT_MPAGE);
	BUTTON_Handle saveBtn = BUTTON_CreateEx(400, GUI_SETT_MPAGE_HEIGHT + 10, GUI_SETT_BTN_WIDTH, GUI_SETT_BTN_HEIGHT, selfWin, WM_CF_SHOW, 0, GUI_SETT_SAVE);

	if (!multipage || !saveBtn) {
		return false;
	}

	/* Buttons */
	BUTTON_SetText(saveBtn, GUI_SETT_SAVE_TXT);

	/* Multipage */
	MULTIPAGE_SetTabHeight(multipage, GUI_SETT_MPAGE_TAB_HEIGHT);

	/* Settings dialogs */
	WM_HWIN dialog;
	for (uint8_t i = 0; i < GUI_COUNTOF(settingsDialogs); i++) {
		dialog = GUI_CreateDialogBox(settingsDialogs[i].info, settingsDialogs[i].widgetsCount, settingsDialogs[i].callback, WM_UNATTACHED, 0, 0);
		if (!dialog) { /* Cannot create dialog */
			return false;
		}
		MULTIPAGE_AddPage(multipage, dialog, settingsDialogs[i].txt);
	}

	return true;
}

/* Window callback */
static void GUI_SettingsCallback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;

	switch (pMsg->MsgId) {
	case WM_CREATE:
		/* Store current settings address */
		settings = SETTINGS_GetInstance();
		/* Store self handle */
		selfWin = pMsg->hWin;
		/* Clear restart flag */
		restartRequired = false;
		/* Create widgets */
		if (!GUI_SettingsInitWidgets()) {
			GUI_FailedHook();
			WM_DeleteWindow(selfWin);
		}
		break;
	case WM_NOTIFY_PARENT:

		widgetId = WM_GetId(pMsg->hWinSrc);

		/* Save button clicked */
		if (widgetId == GUI_SETT_SAVE && pMsg->Data.v == WM_NOTIFICATION_CLICKED) {

			/* Export settings */
			SDCARD_ExportSettings();

			if (restartRequired) {
				GUI_SettingsConfirmDialog(pMsg->hWin, CONFIRM_ALERT, GUI_SETT_RESTART_TXT, SAVE_OP);
			} else {
				WM_DeleteWindow(selfWin);
			}
		}

		break;
	case MSG_CONFIRM: {
		const CONFIRM_Feedback_t *feedback = (const CONFIRM_Feedback_t *)pMsg->Data.p;

		if (feedback->opCode == SAVE_OP) {
			switch (feedback->status) {
			case CONFIRM_YES:
				/* Reboot */
				BOARD_SystemReset();
				break;
			case CONFIRM_NO:
				LOGGER_WRITELN(("Confirm dialog rejected"));
				break;
			case CONFIRM_FAILURE:
				GUI_FailedHook();
				break;
			}
		}

		break;
	}
	case WM_PAINT:
		GUI_SetBkColor(GUI_SETT_BK_COLOR);
		GUI_Clear();
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}


/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void GUI_SettingsOpen(WM_HWIN parent)
{
	WM_HWIN win = WM_CreateWindowAsChild(GUI_SETT_X, GUI_SETT_Y, GUI_SETT_WIDTH, GUI_SETT_HEIGHT, parent, WM_CF_SHOW, GUI_SettingsCallback, 0);

	if (!win) {
		GUI_FailedHook();
	}
}

