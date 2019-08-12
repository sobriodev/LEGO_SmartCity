#ifndef GUI_STARTUP_GUI_STARTUP_H_
#define GUI_STARTUP_GUI_STARTUP_H_

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

#define GUI_STARTUP_WIDTH			LCD_WIDTH
#define GUI_STARTUP_HEIGHT			LCD_HEIGHT
#define GUI_STARTUP_BK_COLOR		GUI_LIGHTBLUE
#define GUI_STARTUP_COLOR			GUI_WHITE

#define GUI_STARTUP_DEFAULT_TXT		"Starting application";

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * todo docs
 * @param pMsg
 */
void GUI_StartupCallback(WM_MESSAGE *pMsg);

/*!
 * todo docs
 * @param stepName
 */
void GUI_StartupChangeStep(const char *stepName);

#endif /* GUI_STARTUP_GUI_STARTUP_H_ */
