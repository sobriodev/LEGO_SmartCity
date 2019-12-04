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
 * \brief Possible light operations
 */
typedef enum {
	LEGO_LIGHT_ON,   	//!< Turn on the light
	LEGO_LIGHT_OFF,  	//!< Turn off the light
	LEGO_LIGHT_TOGGLE	//!< Toggle the light state
} LEGO_LightOp_t;

/*!
 * \brief Structure which joins TCA9548A channel with MCP23017 devices
 */
typedef struct {
	TCA9548A_Channel_t channel; //!< TCA9548A channel
	MCP23017_Chain_t chain;		//!< MCP23017 chain info
} LEGO_MCP23017Info_t;

/*!
 * \brief LEGO LED information
 */
typedef struct {
	uint16_t lightId;							//!< Unique ID
	const LEGO_MCP23017Info_t *mcp23017Info;	//!< See LEGO_MCP23017Info_t
	uint8_t mcp23017DevNum; 					//!< MCP23017 device offset (0-7)
	MCP23017_Port_t mcp23017Port;				//!< MCP23017 port
	uint8_t mcp23017Pin; 						//!< MCP23017 pin
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

/*!
 * \brief Find light instance using unique Id
 *
 * \param id : Light Id
 * \return Searched light structure base address or NULL if nothing was found
 */
const LEGO_Light_t *LEGO_GetLightById(int16_t id);

/*!
 * \brief Light control function
 *
 * \param light : Light instance
 * \param op : Desired operation. See LEGO_LightOp_t for more information
 * \return True if operation succeed, false otherwise
 */
bool LEGO_LedControl(const LEGO_Light_t *light, LEGO_LightOp_t op);

#endif /* LEGO_LEGO_H_ */