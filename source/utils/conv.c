#include "conv.h"
#include "stdlib.h"
#include "validator.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

bool CONV_StrToIP4(const char *str, uint8_t *addr3, uint8_t *addr2, uint8_t *addr1, uint8_t *addr0)
{
	/* Scanning into uint8_t directly is not supported here */
	uint32_t octets[4];
	if (sscanf(str, "%u.%u.%u.%u", octets + 3, octets + 2, octets + 1, octets) != 4) {
		return false;
	}
	*addr3 = (uint8_t)octets[3];
	*addr2 = (uint8_t)octets[2];
	*addr1 = (uint8_t)octets[1];
	*addr0 = (uint8_t)octets[0];
	return true;
}

bool CONV_IP4StrToCompressed(const char *str, uint32_t *dst)
{
	uint8_t octets[4];
	bool res = CONV_StrToIP4(str, octets + 3, octets + 2, octets + 1, octets);

	if (!res) {
		return false;
	}

	*dst = CONV_IP4_TO_UINT32(octets[3], octets[2], octets[1], octets[0]);
	return true;
}

bool CONV_StrToInt(const char *str, int *dst) {
    if (VALIDATOR_IsInt(str)) {
        *dst = atoi(str);
        return true;
    }
    return false;
}

bool CONV_StrToDouble(const char *str, double *dst) {
    if (VALIDATOR_IsDouble(str)) {
        *dst = atof(str);
        return true;
    }
    return false;
}

bool CONV_StrToBool(const char *str, bool *dst) {
    if (VALIDATOR_IsBool(str)) {
        *dst = !strcmp(str, "true");
        return true;
    }
    return false;
}
