#include "validator.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- API_FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

bool VALIDATOR_IsInt(const char *str)
{
	return slre_match(VALIDATOR_INT_REGEXP, str, strlen(str), NULL, 0, 0) > 0;
}

bool VALIDATOR_IsDouble(const char *str)
{
	return slre_match(VALIDATOR_DOUBLE_REGEXP, str, strlen(str), NULL, 0, 0) > 0;
}

bool VALIDATOR_IsBool(const char *str)
{
	return slre_match(VALIDATOR_BOOL_REGEXP, str, strlen(str), NULL, 0, 0) > 0;
}

bool VALIDATOR_IsIp4(const char *str)
{
	struct slre_cap octets[4];
	char buff[4];
	if (slre_match(VALIDATOR_IP4_REGEXP, str, strlen(str), octets, 4, 0) <= 0) {
		return false;
	}

	for (uint8_t i = 0; i < 4; i++) {
		snprintf(buff, 4, "%.*s", octets[i].len, octets[i].ptr);
		if (!VALIDATOR_IsInt(buff) || atoi(buff) > 255) {
			return false;
		}
	}

	return true;
}
