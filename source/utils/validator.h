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
#define VALIDATOR_BOOL_REGEXP		"^(true|false)$"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- DATA TYPES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \typedef Validation function
 *
 * \param str : Input string base address
 * \return True if validation passed, false otherwise
 */
typedef bool (*VALIDATORFn_t)(const char *str);

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- API_FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Check whether value is an integer
 *
 * @param str : Input string
 * @return True if value is an int, false otherwise
 */
bool VALIDATOR_IsInt(const char *str);

/*!
 * \brief Check whether value is a double
 *
 * @param str : Input string
 * @return True if value is a double, false otherwise
 */
bool VALIDATOR_IsDouble(const char *str);

/*!
 * \brief Check whether value is bool (true/false)
 *
 * @param str : Input string
 * @return True if value is boolean, false otherwise
 */
bool VALIDATOR_IsBool(const char *str);

#endif /* UTILS_VALIDATOR_H_ */
