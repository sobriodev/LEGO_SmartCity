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

/* Maximum number of groups the light belongs to */
#define LEGO_LIGHT_MAX_GROUPS	3

/* Animations FreeRTOS */
#define LEGO_TASK_PALACE_CINEMA_NAME		"TASK_CINEMA"
#define LEGO_TASK_PALACE_CINEMA_STACK		0x200
#define LEGO_TASK_PALACE_CINEMA_PRIO		(tskIDLE_PRIORITY + 2)
#define LEGO_TASK_ROLLER_COASTER_NAME		"TASK_COASTER"
#define LEGO_TASK_ROLLER_COASTER_STACK		0x200
#define LEGO_TASK_ROLLER_COASTER_PRIO		(tskIDLE_PRIORITY + 2)

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Structure which joins TCA9548A channel with MCP23017 devices
 */
typedef struct {
	TCA9548A_Channel_t channel; //!< TCA9548A channel
	MCP23017_Chain_t chain;		//!< MCP23017 chain info
} LEGO_I2CDev_t;

/*!
 * \brief Structure which uniquely identifies device on the I2C bus
 * \note According to the MCP23017 manual port A and B are identified as separate instances
 */
typedef struct {
	const LEGO_I2CDev_t *i2cDevInfo;			//!< See LEGO_I2CDev_t for more information
	uint8_t mcp23017DevNum;						//!< MCP23017 device offset (0-7)
	MCP23017_Port_t mcp23017Port;       		//!< MCP23017 port
} LEGO_MCP23017Info_t;

/*!
 * \brief Groups enum for convenience
 */
typedef enum {
	LEGO_GROUP_BROADCAST = 1,	//!< Broadcast group
	/* Building groups */
	LEGO_GROUP_A , 				//!< Brick Bank (10251)
	LEGO_GROUP_B1,				//!< Assembly Square #1 (10255)
	LEGO_GROUP_B2,				//!< Assembly Square #2 (10255)
	LEGO_GROUP_C, 				//!< Parisian Restaurant (10243)
	LEGO_GROUP_D, 				//!< Palace Cinema (10243)
	LEGO_GROUP_E, 				//!< Train Station #1 (60050)
	LEGO_GROUP_F, 				//!< Corner Garage (10264)
	LEGO_GROUP_G, 				//!< Pet Shop #1 (10218)
	LEGO_GROUP_H, 				//!< Pet Shop #2 (10218)
	LEGO_GROUP_I, 				//!< Detective's Office (10246)
	LEGO_GROUP_J, 				//!< Downtown dinner (10260)
	LEGO_GROUP_K, 				//!< Park Street Townhouse #1 (31065)
	LEGO_GROUP_L, 				//!< Park Street Townhouse #2 (31065)
	LEGO_GROUP_M, 				//!< Train Station #2 (7997)
	LEGO_GROUP_N,  				//!< Roller Coaster (10261)
	/* Animations */
	LEGO_GROUP_ANIM0,			//!< Animation 0
	LEGO_GROUP_ANIM1			//!< Animation 1
} LEGO_LightGroup_t;

/*!
 * \brief LEGO Light information
 */
typedef struct {
	uint32_t lightId;							//!< Unique ID
	const LEGO_MCP23017Info_t *mcp23017Info;	//!< See LEGO_MCP23017Info_t
	uint8_t mcp23017Pin;						//!< MCP23017 pin
	uint32_t groupsId[LEGO_LIGHT_MAX_GROUPS];	//!< Specific groups the light belongs to
} LEGO_Light_t;

/*!
 * \brief Possible light operations
 */
typedef enum {
	LEGO_LIGHT_ON,   	//!< Turn on the light
	LEGO_LIGHT_OFF,  	//!< Turn off the light
	LEGO_LIGHT_TOGGLE	//!< Toggle the light state
} LEGO_LightOp_t;

/*!
 * \brief Buffer used in search
 */
typedef struct {
	const LEGO_MCP23017Info_t *mcp23017Info;	//!< LEGO_MCP23017Info_t
	uint8_t mask;								//!< Bit 0 contains information about pin 0, ..., bit 7 contains information about pin 7. Bit set = pin has specified id, bit cleared = pin has not specified id
	uint8_t userField;							//!< Bit 0 contains information about pin state 0, ..., bit 7 contains information about pin state 7. This field is for user only
} LEGO_SearchRes_t;

/*!
 * \brief Search patterns
 */
typedef enum {
	LEGO_SEARCH_ID,  	//!< Search by id
	LEGO_SEARCH_GROUP	//!< Search by group
} LEGO_SearchPattern_t;

/*!
 * \brief Possible return values when using API functions
 */
typedef enum {
	LEGO_ID_NOT_FOUND,	//!< Id not found
	LEGO_I2C_ERR,		//!< I2C error
	LEGO_OP_PERFORMED  	//!< Success
} LEGO_LightOpRes_t;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief RTOS Init (e.g. task for animations)
 *
 * \return True if RTOS initialized successfully, false otherwise
 */
bool LEGO_RTOSInit(void);

/*!
 * \brief Main LEGO startup code. It initializes all needed I2C devices and set default values
 *
 * \return True if startup code succeed, false otherwise
 */
bool LEGO_PerformStartup(void);

/*!
 * \brief Search all lights with specified search pattern
 *
 * \param : Search pattern. See LEGO_SearchPattern_t
 * \param id : Searched id
 * \param lightBuff : See LEGO_SearchRes_t for more information. The buffer must be as big as needed, otherwise the memory will be overridden
 * \return The number of i2c devices whose pins have specified id
 */
uint8_t LEGO_SearchLights(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_SearchRes_t *searchRes);

LEGO_LightOpRes_t LEGO_LightsControl(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightOp_t op);

#endif /* LEGO_LEGO_H_ */
