#include "gui_desktop.h"
#include "stdint.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

uint8_t i = 64;

void desktopCallback(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		i++;
		WM_InvalidateWindow(WM_HBKWIN);
		break;
	case WM_SET_CALLBACK:
	{
		BUTTON_Handle b = BUTTON_CreateEx(10, 10, 150, 50, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
		BUTTON_SetText(b, "Increment counter");
	}
		break;
	case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTBLUE);
		GUI_Clear();
		/* Text display */
		GUI_DispCharAt(i, 200, 200);
	default:
		WM_DefaultProc(pMsg);
	}
}
