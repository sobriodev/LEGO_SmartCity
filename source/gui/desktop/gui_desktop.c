#include "gui_conf.h"
#include "gui_desktop.h"
#include "stdint.h"
#include "validator.h"

#include "virtual_keyboard.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

EDIT_Handle e = 0;
WM_HWIN selfWin;

void GUI_DesktopCallback(WM_MESSAGE *pMsg)
{
	int widgetId;

	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);
		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_EDIT0) {
			VKParams_t params = {e, "Enter true/false value", 15, true , VALIDATOR_IsBool};
			VK_GetInput(&params);
		}
		break;
	case WM_CREATE:
		selfWin = pMsg->hWin;
		e = EDIT_CreateEx(10, 90, 150, 40, selfWin, WM_CF_SHOW, 0, GUI_ID_EDIT0, 20);
		EDIT_SetText(e, "Edit me");

		break;
	case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTBLUE);
		GUI_Clear();
	default:
		WM_DefaultProc(pMsg);
	}
}

void GUI_DesktopCreate(void)
{
	if (!WM_CreateWindow(GUI_DESKTOP_X, GUI_DESKTOP_Y, GUI_DESKTOP_WIDTH, GUI_DESKTOP_HEIGHT, WM_CF_SHOW, GUI_DesktopCallback, 0)) {
		GUI_FailedHook();
	}
}
