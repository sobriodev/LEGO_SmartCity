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

/* Maximum logger sources */
#define MAX_LOGGER_SOURCES	50

#define LOG_INFO            "INFO"		/* Just for information */
#define LOG_WARNING         "WARNING" 	/* Something is not in the correct state or has not got correct value */
#define LOG_ERROR           "ERROR"		/* Typically something crashed the application */
#define LOG_TEMPLATE		"[%s] %s"

#define LOGGER_NEWLINE		"\r\n"

/* Aliases for convenience */
#define LOGGER_WRITE(SOURCE, LOG_TYPE, MSG)		LOGGER_LogEvent((SOURCE), (LOG_TYPE), (MSG), false)
#define LOGGER_WRITELN(SOURCE, LOG_TYPE, MSG)	LOGGER_LogEvent((SOURCE), (LOG_TYPE), (MSG), true)

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Available logger sources
 *
 * \note When adding new logger source, update this enum and also add enable/disable entry in source files array
 */
typedef enum {
	CRITICAL_LOG = 0,	//!< Critical system messages
	QS_LOG           	//!< Query string messages
} LoggerSource_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Make log message using template message and optional template arguments
 *
 * \param template : Base address of template message
 * \return Base address of message or NULL if an error occurred
 */
const char *LOGGER_MakeLogMsg(const char *template, ...);

/*!
 * \brief Log event
 *
 * \param source : Log source. See LoggerSource_t for more information
 * \param logType : Base address of log type
 * \param msg : Base address of log message
 * \param newLine :  True if new line should be automatically added after log message, false otherwise
 */
void LOGGER_LogEvent(LoggerSource_t source, const char *logType, const char *msg, bool newLine);

#endif /* LOGGER_H_ */
