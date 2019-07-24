#ifndef LOGGER_H_
#define LOGGER_H_

#include "logger_def.h"
#include "stdio.h"
#include "fsl_debug_console.h"


/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Maximum log length */
#define LOGGER_SBUFF_LEN    150

#define LOG_INFO            "INFO"		/* Just for information */
#define LOG_WARNING         "WARNING" 	/* Something is not in the correct state or has not got correct value */
#define LOG_ERROR           "ERROR"		/* Typically something crashed the application */
#define LOG_TEMPLATE		"[%s] %s"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Make log message using template message and optional template arguments
 *
 * \param template : Base address of template message
 * \return Base address of ready message or NULL if error occurred
 */
const char *LOGGER_MakeLogMsg(const char *template, ...);

/*!
 * \brief Log system event
 *
 * \param logType : Base address of string containing error type
 * \param msg : Message to be displayed
 */
static inline void LOGGER_LogEvent(const char *logType, const char *msg)
{
	PRINTF(LOG_TEMPLATE, logType, msg);
}

#endif /* LOGGER_H_ */
