#include "validator.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- API_FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

bool VALIDATOR_IsInt(const char *str)
{
	return slre_match(VALIDATOR_INT_REGEXP, str, strlen(str), NULL, 0, 0) != SLRE_NO_MATCH;
}


bool VALIDATOR_IsDouble(const char *str)
{
	return slre_match(VALIDATOR_DOUBLE_REGEXP, str, strlen(str), NULL, 0, 0) != SLRE_NO_MATCH;
}

bool VALIDATOR_IsBool(const char *str)
{
	return slre_match(VALIDATOR_BOOL_REGEXP, str, strlen(str), NULL, 0, 0) != SLRE_NO_MATCH;
}
