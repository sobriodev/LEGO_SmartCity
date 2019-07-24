#ifndef UTILS_LOGGER_DEF_H_
#define UTILS_LOGGER_DEF_H_

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* If 1 logging is enabled and log event are generated depending on the macros below. 0 disables all logging features */
#define LOGGING_ENABLED		1

/* Log core messages (FreeRTOS tasks errors, etc.) */
#define CORE_LOG			1

/* Query string logging (input arguments, parsing errors) */
#define QUERY_STRING_LOG	1


#endif /* UTILS_LOGGER_DEF_H_ */
