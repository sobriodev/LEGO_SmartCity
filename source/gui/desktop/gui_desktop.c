#include "gui_conf.h"
#include "gui_desktop.h"
#include "stdint.h"
#include "validator.h"

#include "virtual_keyboard.h"
#include "settings.h"
#include "sdcard_conf.h"
#include "conv.h"
#include "confirm.h"
#include "gui_settings.h"
#include "background.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

static BUTTON_Handle settingsButton;
static WM_HWIN selfWin;

void GUI_DesktopCallback(WM_MESSAGE *pMsg)
{
	int widgetId;
	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);
		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_BUTTON0) {
			GUI_SettingsOpen(pMsg->hWin);
		}
		break;
	case WM_SET_CALLBACK:
		selfWin = pMsg->hWin;
		settingsButton = BUTTON_CreateEx(400, 50, 60, 60, selfWin, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
		BUTTON_SetText(settingsButton, "Settings");
		break;
	case WM_PAINT:
		//GUI_SetBkColor(GUI_LIGHTBLUE);
		GUI_Clear();
		GUI_DrawBitmap(&bmBackground, 0, 0);
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void GUI_DesktopCreate(void)
{
	/* Just take control of background window */
	WM_SetCallback(WM_HBKWIN, GUI_DesktopCallback);
}
