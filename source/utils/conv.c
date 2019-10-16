#include "conv.h"
#include "stdlib.h"
#include "validator.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

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
