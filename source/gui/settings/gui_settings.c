#include "gui_settings.h"
#include "gui_conf.h"
#include "virtual_keyboard.h"
#include "settings.h"
#include "conv.h"

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
#define GUI_SETT_CLOSE					(GUI_SETT_SAVE + 1)
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

/* Basic widgets */
typedef struct {
	MULTIPAGE_Handle multipage;
	BUTTON_Handle saveBtn;
	BUTTON_Handle closeBtn;
} GUI_SettingsWidgets_t;

/* Dialog entity */
typedef struct {
	const GUI_WIDGET_CREATE_INFO *info;
	uint8_t widgetsCount;
	WM_CALLBACK *callback;
	const char *txt;
} GUI_SettingsDialog_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------- PRIVATE FUNCTIONS HEADERS ------------------------ */
/* ----------------------------------------------------------------------------- */

static void GUI_SettingsServerCallback(WM_MESSAGE *pMsg);
static void GUI_SettingsLCDCallback(WM_MESSAGE *pMsg);

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
		{ miscDialog,   GUI_COUNTOF(miscDialog),   NULL,                       GUI_SETT_MISC_TXT }
};

static const char *serverEditsTxt[] = {GUI_SETT_SERVER_IP_TXT, GUI_SETT_SERVER_SM_TXT, GUI_SETT_SERVER_GW_TXT, GUI_SETT_SERVER_DNS_TXT};
static const char *lcdblankingTimes[] = {"Disabled", "10s", "30s", "2min", "5min"};
static char ipBuffer[16]; /* Buffer for converting ip addresses */

static WM_HWIN selfWin; /* Handle of settings window */
static GUI_SettingsWidgets_t widgets; /* Static widgets */
static SETTINGS_t *settings; /* Current settings */

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Server settings callback */
static void GUI_SettingsServerCallback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;
	static VK_Params_t params;
	static GUI_BlockingDialogInfo_t dialogInfo;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		/* Initialize dialog constants */
		params.copyVal = true;
		dialogInfo.dialog = DIALOG_VK;
		dialogInfo.srcWin = pMsg->hWin;
		dialogInfo.data = &params;
		/* Set current values */
		CONV_IP4CompresssedToStr(settings->httpsrvIp, ipBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_IP), ipBuffer);
		CONV_IP4CompresssedToStr(settings->httpsrvSm, ipBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_SM), ipBuffer);
		CONV_IP4CompresssedToStr(settings->httpsrvGw, ipBuffer);
		EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_SETT_SERVER_GW), ipBuffer);
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
			/* IP, SM, GW */
			if (widgetId >= GUI_SETT_SERVER_IP && widgetId < GUI_SETT_SERVER_DNS) {
				params.validatorFn = VALIDATOR_IsIp4;
				params.maxLen = 15;
				params.inputHandle = WM_GetDialogItem(pMsg->hWin, widgetId);
				params.inputDesc = serverEditsTxt[widgetId - GUI_SETT_SERVER_IP];
				GUI_RequestBlockingDialog(&dialogInfo);
			/* Hostname */
			} else if (widgetId == GUI_SETT_SERVER_DNS) {
				params.validatorFn = NULL;
				params.maxLen = 5;
				params.inputHandle = WM_GetDialogItem(pMsg->hWin, widgetId);
				params.inputDesc = serverEditsTxt[widgetId - GUI_SETT_SERVER_IP];
				GUI_RequestBlockingDialog(&dialogInfo);
			}
		}

		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

/* LCD settings callback */
static void GUI_SettingsLCDCallback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		DROPDOWN_SetTextHeight(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), GUI_SETT_DROPDOWN_TEXT_HEIGHT);
		DROPDOWN_SetTextAlign(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), TEXT_CF_VCENTER | TEXT_CF_HCENTER);
		for (uint8_t i = 0; i < GUI_COUNTOF(lcdblankingTimes); i++) {
			DROPDOWN_AddString(WM_GetDialogItem(pMsg->hWin, GUI_SETT_LCD_BLANKING), lcdblankingTimes[i]);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

static bool GUI_SettingsInitWidgets(void)
{
	/* Create widgets */
	widgets.multipage = MULTIPAGE_CreateEx(0, 0, GUI_SETT_WIDTH, GUI_SETT_MPAGE_HEIGHT, selfWin, WM_CF_SHOW, 0, GUI_SETT_MPAGE);
	widgets.saveBtn = BUTTON_CreateEx(330, GUI_SETT_MPAGE_HEIGHT + 10, GUI_SETT_BTN_WIDTH, GUI_SETT_BTN_HEIGHT, selfWin, WM_CF_SHOW, 0, GUI_SETT_SAVE);
	widgets.closeBtn = BUTTON_CreateEx(400, GUI_SETT_MPAGE_HEIGHT + 10, GUI_SETT_BTN_WIDTH, GUI_SETT_BTN_HEIGHT, selfWin, WM_CF_SHOW, 0, GUI_SETT_CLOSE);

	if (!widgets.multipage || !widgets.saveBtn || !widgets.closeBtn) {
		return false;
	}

	/* Buttons */
	BUTTON_SetText(widgets.saveBtn, GUI_SETT_SAVE_TXT);
	BUTTON_SetText(widgets.closeBtn, GUI_SETT_CLOSE_TXT);

	/* Multipage */
	MULTIPAGE_SetTabHeight(widgets.multipage, GUI_SETT_MPAGE_TAB_HEIGHT);

	/* Settings dialogs */
	WM_HWIN dialog;
	for (uint8_t i = 0; i < GUI_COUNTOF(settingsDialogs); i++) {
		dialog = GUI_CreateDialogBox(settingsDialogs[i].info, settingsDialogs[i].widgetsCount, settingsDialogs[i].callback, WM_UNATTACHED, 0, 0);
		if (!dialog) { /* Cannot create dialog */
			return false;
		}
		MULTIPAGE_AddPage(widgets.multipage, dialog, settingsDialogs[i].txt);
	}

	return true;
}

/* Window callback */
static void GUI_SettingsCallback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId) {
	case WM_CREATE:
		/* Store current settings address */
		settings = SETTINGS_GetInstance();
		/* Store self handle */
		selfWin = pMsg->hWin;
		/* Create widgets */
		if (!GUI_SettingsInitWidgets()) {
			GUI_FailedHook();
			WM_DeleteWindow(selfWin);
		}
		break;
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

