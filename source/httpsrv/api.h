#ifndef HTTPSRV_API_H_
#define HTTPSRV_API_H_

/* Httpsrv */
#include "httpsrv.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------------ MACROS --------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Light control api names */
#define API_LIGHT_CONTROL_NAME		"lego-light"

/* Animation api names */
#define API_ANIM_STATUS_NAME		"lego-anim-status"
#define API_ANIM_DELAY_NAME			"lego_anim_delay"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

int32_t API_LightControl(HTTPSRV_CGI_REQ_STRUCT *request);

int32_t API_AnimStatus(HTTPSRV_CGI_REQ_STRUCT *request);

int32_t API_AnimDelay(HTTPSRV_CGI_REQ_STRUCT *request);

#endif /* HTTPSRV_API_H_ */
