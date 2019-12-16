#ifndef HTTPSRV_API_H_
#define HTTPSRV_API_H_

/* Httpsrv */
#include "httpsrv.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------------ MACROS --------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Light control api names */
#define API_LIGHT_CONTROL_NAME		"lego-light"
#define API_ALL_CONTROL_NAME		"lego-all"

/* Animation api names */
#define API_ANIM_CONTROL			"lego-anim"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

int32_t API_LightControl(HTTPSRV_CGI_REQ_STRUCT *request);

#endif /* HTTPSRV_API_H_ */
