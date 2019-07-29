#include "logger_def.h"
#include "logger.h"
#include "stdarg.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE_VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Buffer for storing processed messages */
static char sbuffer[LOGGER_SBUFF_LEN];

/* Logger sources enable/disable entries. The order of elements is determined by LoggerSource_t enum */
static bool loggerSources[MAX_LOGGER_SOURCES] =
{
		CRITICAL_LOG_ENABLED,
		QS_LOG_ENABLED
};

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

const char *LOGGER_MakeLogMsg(const char *template, ...)
{
	/* Handle optional arguments using variadic arguments rules */
    va_list args;
    va_start(args, template);
    int ret = vsnprintf(sbuffer, LOGGER_SBUFF_LEN, template, args);
    va_end(args);

    /* If ret contains negative number an error occurred and NULL should be returned */
    if (ret < 0)
    	return NULL;

    return sbuffer;
}

void LOGGER_LogEvent(LoggerSource_t source, const char *logType, const char *msg, bool newLine)
{
	#if LOGGING_ENABLED
		/* Print only if desired type is enabled */
		if (loggerSources[source]) {
			PRINTF(LOG_TEMPLATE, logType, msg);
			if (newLine) {
				PRINTF(LOGGER_NEWLINE);
			}
		}
	#endif
}
