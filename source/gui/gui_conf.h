#ifndef GUI_GUI_CONF_H_
#define GUI_GUI_CONF_H_

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Main emWin initialization function
 */
void GUI_Start(void);

/*!
 * \brief Called after emWin cannot create widget
 */
void GUI_FailedHook(void);

#endif /* GUI_GUI_CONF_H_ */
