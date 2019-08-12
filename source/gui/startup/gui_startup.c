#include "gui_startup.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

static char *stepText = GUI_STARTUP_DEFAULT_TXT;

/* Step semaphore. Used to prevent executing code before GUI refreshing */
bool stepSem = true;

void GUI_StartupCallback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(GUI_STARTUP_BK_COLOR);
		GUI_Clear();
		GUI_DispStringHCenterAt(stepText, GUI_STARTUP_WIDTH / 2, GUI_STARTUP_HEIGHT / 2);
		stepSem = true; /* Clear semaphore occupation */
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

void GUI_StartupChangeStep(const char *stepName)
{
	stepText = (char *)stepName;
	stepSem = false; /* Block semaphore */
	WM_InvalidateWindow(WM_HBKWIN);

	while (!stepSem) {
		WM_Exec(); /* Wait for GUI refreshing */
	}
}
