#include "query_string.h"
#include "string.h"
#include "stdbool.h"
#include "validator.h"
#include "conv.h"
#include "logger.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Cast param */
static void QS_CastParam(const char *param, QS_Param_t *dst)
{
    if (CONV_StrToInt(param, &dst->value.valueInt)) {
        dst->paramType = QS_PARAM_INT;
        return;
    } else if (CONV_StrToDouble(param, &dst->value.valueDouble)) {
        dst->paramType = QS_PARAM_DOUBLE;
        return;
    } else if (CONV_StrToBool(param, &dst->value.valueBool)) {
        dst->paramType = QS_PARAM_BOOL;
        return;
    }

    /* Conversion failed. Just save it as string */
    dst->paramType = QS_PARAM_STRING;
    dst->value.valueStr = param;
}

/* Parse query string param */
static bool QS_ParseParam(char *param, QS_Param_t *paramDst)
{
	char *delimPos = strchr(param, QS_PARAM_DELIM);
	if (delimPos != NULL) {
		*delimPos = '\0';

		if (strlen(param) > 0 && strlen(delimPos + 1) > 0) {
			/* Param contains delimiter and name/value length is correct so it can be saved */
			paramDst->name = param;
			paramDst->valueRaw = delimPos + 1;
			QS_CastParam(paramDst->valueRaw, paramDst);
			return true;
		}
	}
	return false;
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Parse query string */
uint8_t QS_Parse(char *qs, QS_Param_t *paramsTbl, uint8_t maxParams)
{
	uint8_t paramOffset = 0;
	char *delimPos = NULL;
	char *startAddr = qs;

	delimPos = strchr(qs, QS_DELIM);
	while (delimPos != NULL && paramOffset < maxParams) {
		/* Change delimiter char to the NULL char so we can navigate easily */
		*delimPos = '\0';

		if (QS_ParseParam(startAddr, paramsTbl + paramOffset)) {
			paramOffset++;
		} else {
            LOGGER_WRITELN(("Invalid query param (aka \"%s\")", startAddr));
		}

		/* Search next param after the delimiter */
		startAddr = delimPos + 1;
		delimPos = strchr(startAddr, QS_DELIM);
	}

	/* Handle trailing characters */
	if (paramOffset < maxParams && *startAddr != '\0') {
        if (QS_ParseParam(startAddr, paramsTbl + paramOffset)) {
            paramOffset++;
        } else {
            LOGGER_WRITELN(("Invalid query param (aka \"%s\")", startAddr));
        }
    }

	return paramOffset;
}

/* Get query param. The function returns the first occurrence in params table */
const QS_Param_t *QS_GetParam(const char *name, QS_Param_t *paramsTbl, uint8_t paramsCount) {

	for (uint8_t i = 0; i < paramsCount; i++) {
		if (!strcmp(paramsTbl[i].name, name)) {
			return &paramsTbl[i];
		}
	}

	return NULL;
}
