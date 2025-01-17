#ifndef UTILS_LOGGER_DEF_H_
#define UTILS_LOGGER_DEF_H_

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* If 1 logging is enabled and log event are generated depending on the macros below. 0 disables all logging features */
#define LOGGING_ENABLED				1

/* Log core messages (FreeRTOS tasks errors, etc.) */
#define CRITICAL_LOG_ENABLED		1

/* Query string logging (input arguments, parsing errors) */
#define QS_LOG_ENABLED	1

#endif /* UTILS_LOGGER_DEF_H_ */
