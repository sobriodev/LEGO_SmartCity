#include "gui_startup.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Text to be displayed */
static char *stepText = GUI_STARTUP_DEFAULT_TXT;

/* Window id */
static WM_HWIN selfWin;

/* Step semaphore. Used to prevent executing code before GUI refreshing */
static bool stepSem = true;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void GUI_StartupCallback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId) {
	case WM_SET_CALLBACK:
		selfWin = pMsg->hWin;
		break;
	case WM_PAINT:
		GUI_SetBkColor(GUI_STARTUP_BK_COLOR);
		GUI_Clear();
		GUI_DispStringHCenterAt(stepText, GUI_STARTUP_WIDTH / 2, GUI_STARTUP_HEIGHT / 2);
		stepSem = true; /* Take semaphore */
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void GUI_StartupChangeStep(const char *stepName)
{
	stepText = (char *)stepName;
	stepSem = false; /* Block semaphore */
	WM_InvalidateWindow(selfWin);

	while (!stepSem) {
		WM_Exec(); /* Wait for GUI refreshing */
	}
}