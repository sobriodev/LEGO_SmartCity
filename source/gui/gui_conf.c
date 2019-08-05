#include "gui_conf.h"
#include "desktop/gui_desktop.h"
#include "logger.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void GUI_Start(void)
{
	/* emWin start */
	GUI_Init();

	/* Enable auto-buffering */
	WM_MULTIBUF_Enable(1);

	/* Set size for the background window */
	WM_SetSize(WM_HBKWIN, LCD_WIDTH, LCD_HEIGHT);

	/* Set callback for the background window */
	WM_SetCallback(WM_HBKWIN, desktopCallback);

	/* All buttons should react on level */
	BUTTON_SetReactOnLevel();

	/* Start GUI */
	WM_Exec();
}

void GUI_FailedHook(void)
{
	LOGGER_WRITELN(CRITICAL_LOG, LOG_ERROR, "emWin widget creation error. Check stack and heap usage");
}
