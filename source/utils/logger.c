#include "logger_def.h"
#include "logger.h"
#include "stdarg.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE_VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

static char sbuffer[LOGGER_SBUFF_LEN] = {0};

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

    /* If ret contains negative number an error occured and NULL should be returned */
    if (ret < 0)
    	return NULL;

    return sbuffer;
}
