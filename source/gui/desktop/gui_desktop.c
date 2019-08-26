#include "gui_conf.h"
#include "gui_desktop.h"
#include "stdint.h"
#include "validator.h"

#include "virtual_keyboard.h"
#include "settings.h"
#include "sdcard_conf.h"
#include "misc.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

EDIT_Handle e = 0;
BUTTON_Handle b = 0;
static WM_HWIN selfWin;

char buff[15];

uint8_t addr0, addr1, addr2, addr3;

void GUI_DesktopCallback(WM_MESSAGE *pMsg)
{
	int widgetId;

	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);
		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_EDIT0) {
			VKParams_t params = {e, "Enter true/false value", 15, true , VALIDATOR_IsIp4};
			if (VK_GetInput(&params)) {
				EDIT_GetText(e, buff, 15);
				stringToIPv4(buff, &addr0, &addr1, &addr2, &addr3);
				PRINTF("%d %d %d %d", addr0, addr1, addr2, addr3);
			}
		}

		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_BUTTON0) {
		}

		break;
	case WM_SET_CALLBACK:
		selfWin = pMsg->hWin;
		e = EDIT_CreateEx(10, 90, 150, 40, selfWin, WM_CF_SHOW, 0, GUI_ID_EDIT0, 20);
		b = BUTTON_CreateEx(200, 50, 60, 60, selfWin, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
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
	/* Just take control of background window */
	WM_SetCallback(WM_HBKWIN, GUI_DesktopCallback);
}
