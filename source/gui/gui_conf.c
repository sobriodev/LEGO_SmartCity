#include "gui_conf.h"
#include "desktop/gui_desktop.h"

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

	/* Set size for the background window */
	WM_SetSize(WM_HBKWIN, LCD_WIDTH, LCD_HEIGHT);

	/* Set callback for the background window */
	WM_SetCallback(WM_HBKWIN, desktopCallback);

	/* Start GUI */
	WM_Exec();
}
