#ifndef UTILS_VALIDATOR_H_
#define UTILS_VALIDATOR_H_

#include "stdbool.h"
#include "string.h"
#include "slre.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Regular expressions */
#define VALIDATOR_INT_REGEXP		"^-?(0|[1-9][0-9]*)$"
#define VALIDATOR_DOUBLE_REGEXP		"^-?(0|[1-9][0-9]*)\\.[0-9]+$"
#define VALIDATOR_BOOL_REGEXP		"^true|false$"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- DATA TYPES ------------------------------ */
/* ----------------------------------------------------------------------------- */

typedef bool (*VALIDATORFn_t)(const char *value);

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- API_FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

bool VALIDATOR_IsInt(const char *str);
bool VALIDATOR_IsDouble(const char *str);
bool VALIDATOR_IsBool(const char *str);

#endif /* UTILS_VALIDATOR_H_ */
