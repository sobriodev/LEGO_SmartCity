#include "gui_conf.h"
#include "gui_desktop.h"
#include "stdint.h"
#include "validator.h"

#include "virtual_keyboard.h"
#include "settings.h"
#include "sdcard_conf.h"
#include "conv.h"
#include "confirm.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

EDIT_Handle e = 0;
BUTTON_Handle b = 0;
static WM_HWIN selfWin;
char buff[15];

uint8_t addr0, addr1, addr2, addr3;
VK_Params_t params;
GUI_BlockingDialogInfo_t infoGUI;
CONFIRM_Params_t p;
GUI_BlockingDialogInfo_t infoConfirm;

void GUI_DesktopCallback(WM_MESSAGE *pMsg)
{
	int widgetId;

	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);
		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_EDIT0) {
			params.copyVal = true;
			params.inputDesc = "Description";
			params.inputHandle = e;
			params.maxLen = 15;
			params.validatorFn = VALIDATOR_IsBool;
			params.srcWin = pMsg->hWin;
			infoGUI.dialog = DIALOG_VK;
			infoGUI.data = &params;
			infoGUI.winSrc = pMsg->hWin;

			GUI_RequestBlockingDialog(&infoGUI);
		}

		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED && widgetId == GUI_ID_BUTTON0) {
			p.msg = "This kind of view requires application restart. Choose correct option and see how it works!";
			p.type = CONFIRM_ALERT;
			infoConfirm.dialog = DIALOG_CONFIRM;
			infoConfirm.winSrc = pMsg->hWin;
			infoConfirm.data = &p;
			GUI_RequestBlockingDialog(&infoConfirm);
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
		break;
	case MSG_VK: /* Virtual keyboard returned */
		switch (pMsg->Data.v) {
		case VK_FAILURE:
			GUI_FailedHook();
			break;
		case VK_NON_VALID:
		case VK_STORED:
			p.msg = "This kind of view requires application restart. Choose correct option and see how it works!";
			p.type = CONFIRM_CONFIRM;
			infoConfirm.dialog = DIALOG_CONFIRM;
			infoConfirm.winSrc = pMsg->hWin;
			infoConfirm.data = &p;
			GUI_RequestBlockingDialog(&infoConfirm);
			break;
		}
		break;
	case MSG_CONFIRM:
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
