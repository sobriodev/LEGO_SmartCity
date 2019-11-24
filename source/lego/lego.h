#ifndef LEGO_LEGO_H_
#define LEGO_LEGO_H_

#include "stdint.h"
#include "stdbool.h"

/* I2C devices */
#include "mcp23017.h"
#include "tca9548a.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Lego LED information
 */
typedef struct {
	uint16_t lightId;				//!< Unique ID
	uint8_t tca9548aCh; 			//!< TCA9548a channel
	uint8_t mcp23017Addr; 			//!< MCP23017 device address
	MCP23017_Port_t mcp23017Port;	//!< MCP23017 port
	uint8_t mcp23017Pin; 			//!< MCP23017 pin
} LEGO_Light_t;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Main LEGO startup code. It initializes all needed I2C devices and set default values
 *
 * \return True if startup code succeed, false otherwise
 */
bool LEGO_PerformStartup(void);

#endif /* LEGO_LEGO_H_ */
