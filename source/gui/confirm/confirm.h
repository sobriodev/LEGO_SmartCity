#ifndef GUI_CONFIRM_CONFIRM_H_
#define GUI_CONFIRM_CONFIRM_H_

/* emWin */
#include "emwin_support.h"
#include "GUI.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Text constants */
#define CONFIRM_OK_TXT	"OK"
#define CONFIRM_YES_TXT	"Yes"
#define CONFIRM_NO_TXT	"No"

/* Colors and fonts */
#define CONFIRM_BK_COLOR 	GUI_GRAY
#define CONFIRM_TXT_COLOR	GUI_WHITE

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Possible dialog types
 */
typedef enum {
	CONFIRM_ALERT, 	//!< Message with close button only. For informing about various events
	CONFIRM_CONFIRM	//!< Confirmation dialog. For getting user feedback
} CONFIRM_Type_t;

/*!
 * \brief Dialog parameters
 */
typedef struct {
	CONFIRM_Type_t type;	//!< See CONFIRM_Type_T for more information
	const char *msg;		//!< Message to be displayed
} CONFIRM_Params_t;

/*!
 * \brief Possible return values
 */
typedef enum {
	CONFIRM_FAILURE = -1,	//!< emWin internal error (typically memory problem)
	CONFIRM_YES,         	//!< Confirmation
	CONFIRM_NO           	//!< Rejection
} CONFIRM_Status_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

CONFIRM_Status_t CONFIRM_Exec(CONFIRM_Params_t *params);

#endif /* GUI_CONFIRM_CONFIRM_H_ */
