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
#define API_ANIM_DELAY_NAME			"lego-anim-delay"

/* Parking api names */
#define API_PARKING_STATUS_NAME		"lego-parking-status"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Light control API function
 * \param request : Http request
 * \return The number of bytes written back
 */
int32_t API_LightControl(HTTPSRV_CGI_REQ_STRUCT *request);

/*!
 * \brief Animation status control API function
 * \param request : Http request
 * \return The number of bytes written back
 */
int32_t API_AnimStatus(HTTPSRV_CGI_REQ_STRUCT *request);

/*!
 * \brief Animation delay control API function
 * \param request : Http request
 * \return The number of bytes written back
 */
int32_t API_AnimDelay(HTTPSRV_CGI_REQ_STRUCT *request);

/*!
 * \brief Parking control API function
 * \param request : Http request
 * \return The number of bytes written back
 */
int32_t API_ParkingStatus(HTTPSRV_CGI_REQ_STRUCT *request);

#endif /* HTTPSRV_API_H_ */
