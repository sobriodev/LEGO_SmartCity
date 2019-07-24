#include "gui_desktop.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void desktopCallback(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(GUI_LIGHTBLUE);
		GUI_Clear();
		/* Text display */
		GUI_DispStringHCenterAt(DESKTOP_STRING, LCD_WIDTH / 2, LCD_HEIGHT / 2);
	default:
		WM_DefaultProc(pMsg);
	}
}
