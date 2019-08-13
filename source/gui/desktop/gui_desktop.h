#ifndef GUI_DESKTOP_GUI_DESKTOP_H_
#define GUI_DESKTOP_GUI_DESKTOP_H_

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

#define GUI_DESKTOP_X		0
#define GUI_DESKTOP_Y		0
#define GUI_DESKTOP_WIDTH	LCD_WIDTH
#define GUI_DESKTOP_HEIGHT	LCD_HEIGHT

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Desktop view callback
 *
 * \param pMsg :emWin message base address
 */
void GUI_DesktopCallback(WM_MESSAGE *pMsg);

/*!
 * \brief Create desktop window
 */
void GUI_DesktopCreate(void);

#endif /* GUI_DESKTOP_GUI_DESKTOP_H_ */
