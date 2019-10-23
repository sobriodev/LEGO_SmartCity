#ifndef GUI_SETTINGS_GUI_SETTINGS_H_
#define GUI_SETTINGS_GUI_SETTINGS_H_

#include "stdbool.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Text constants (buttons) */
#define GUI_SETT_SAVE_TXT				"Save"
#define GUI_SETT_CLOSE_TXT				"Cancel"

/* Text constants (tabs) */
#define GUI_SETT_SERVER_TXT				"Http server"
#define GUI_SETT_LCD_TXT				"Display"
#define GUI_SETT_MISC_TXT				"Miscellaneous"

/* Text constants (http server tab) */
#define GUI_SETT_SERVER_IP_TXT			"IP address"
#define GUI_SETT_SERVER_SM_TXT			"Subnet mask"
#define GUI_SETT_SERVER_GW_TXT			"Default gateway"
#define GUI_SETT_SERVER_DNS_TXT			"mDNS hostname"

/* Text constants (display tab) */
#define GUI_SETT_LCD_BRIGHTNESS_TXT		"Brightness"
#define GUI_SETT_LCD_BLANKING_TXT		"Blanking time"

/* Text constants (misc tab) */
#define GUI_SETT_MISC_RESTORE_TXT		"Default settings"
#define GUI_SETT_MISC_RESTORE_BTN_TXT	"Restore"

/* Colors and fonts */
#define GUI_SETT_BK_COLOR			GUI_LIGHTGRAY

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void GUI_SettingsOpen(WM_HWIN parent);

#endif /* GUI_SETTINGS_GUI_SETTINGS_H_ */
