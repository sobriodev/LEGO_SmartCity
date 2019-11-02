#ifndef HTTPSRV_QUERY_STRING_H_
#define HTTPSRV_QUERY_STRING_H_

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Remove */
#include "stdbool.h"
#include "stdint.h"

#define QS_DELIM				'&'
#define QS_PARAM_DELIM			'='
/* For convenience */
#define QS_PARAM_IS_INT(P) 		((P)->paramType == QS_PARAM_INT)
#define QS_PARAM_IS_DOUBLE(P)	((P)->paramType == QS_PARAM_DOUBLE)
#define QS_PARAM_IS_BOOL(P) 	((P)->paramType == QS_PARAM_BOOL)
#define QS_PARAM_EXISTS(P)		((P) != NULL)

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Supported formats
 */
typedef enum {
	QS_PARAM_INT,    //!< Integer value
	QS_PARAM_DOUBLE, //!< Double value
	QS_PARAM_BOOL,   //!< Boolean value (true/false)
	QS_PARAM_STRING  //!< Raw string
} QS_ParamType_t;

/*!
 * \brief Param value storage union
 */
typedef union {
	int valueInt;			//!< Integer value
	double valueDouble;		//!< Double value
	bool valueBool;			//!< Boolean value
	const char *valueStr;	//!< String value
} QS_ParamValue_t;

/*!
 * \brief QS Param structure
 */
typedef struct {
	const char *name; 			//!< Param name
	const char *valueRaw; 		//!< Raw value (string)
    QS_ParamType_t paramType;	//!< Param type. See QS_ParamType_t for more information
    QS_ParamValue_t value;		//!< Converted param value
} QS_Param_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Parse query string
 *
 * \param qs : Input buffer
 * \param paramsTbl : Params table base address
 * \param maxParams : Maximum number of params stored
 * \return The number of params stored
 * \note The input buffer will be modified
 */
uint8_t QS_Parse(char *qs, QS_Param_t *paramsTbl, uint8_t maxParams);

/*!
 * \brief Get param
 *
 * \param name : Param name
 * \param paramsTbl : Params table base address
 * \param paramsCount : Params table length
 * \return Searched param base address or NULL if param not found
 */
const QS_Param_t *QS_GetParam(const char *name, QS_Param_t *paramsTbl, uint8_t paramsCount);

#endif /* HTTPSRV_QUERY_STRING_H_ */
