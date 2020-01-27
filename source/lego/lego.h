#ifndef LEGO_LEGO_H_
#define LEGO_LEGO_H_

#include "stdint.h"
#include "stdbool.h"

/* I2C devices */
#include "mcp23017.h"
#include "tca9548a.h"
#include "vl6180x.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Maximum number of groups the light belongs to */
#define LEGO_LIGHT_MAX_GROUPS	3

/* Animations FreeRTOS */
#define LEGO_TASK_PALACE_CINEMA_NAME		"TASK_CINEMA"
#define LEGO_TASK_PALACE_CINEMA_STACK		0x100
#define LEGO_TASK_PALACE_CINEMA_PRIO		(tskIDLE_PRIORITY + 3)
#define LEGO_TASK_ROLLER_COASTER_NAME		"TASK_COASTER"
#define LEGO_TASK_ROLLER_COASTER_STACK		0x100
#define LEGO_TASK_ROLLER_COASTER_PRIO		(tskIDLE_PRIORITY + 3)
#define LEGO_TASK_AUTO_MODE_NAME			"TASK_LEGO_AUTO"
#define LEGO_TASK_AUTO_MODE_STACK			0x150
#define LEGO_TASK_AUTO_MODE_PRIO			(tskIDLE_PRIORITY + 2)

#define LEGO_TASK_TRAFFIC_LIGHTS_NAME		"TASK-TRAFFIC"
#define LEGO_TASK_TRAFFIC_LIGHTS_STACK		0x100
#define LEGO_TASK_TRAFFIC_LIGHTS_PRIO		(tskIDLE_PRIORITY + 3)

/* Parking FreeRTOS */
#define LEGO_TASK_PARKING_NAME				"TASK-LEGO-PARKING"
#define LEGO_TASK_PARKING_STACK				0x150
#define LEGO_TASK_PARKING_PRIO				(tskIDLE_PRIORITY + 2)

/* Animation delays valid ranges in ms */
#define LEGO_AUTO_MODE_DELAY_MIN			2000
#define LEGO_AUTO_MODE_DELAY_MAX			60000
#define LEGO_PALACE_CINEMA_DELAY_MIN		150
#define LEGO_PALACE_CINEMA_DELAY_MAX		5000
#define LEGO_ROLLER_COASTER_DELAY_MIN		150
#define LEGO_ROLLER_COASTER_DELAY_MAX		5000
#define LEGO_TRAFFIC_LIGHTS_TIME_MIN		5000
#define LEGO_TRAFFIC_LIGHTS_TIME_MAX		20000

/* Parking related */
#define LEGO_PARKING_TRESHOLD				40 /* 4cm */
#define LEGO_PARKING_REFRESH_MS				2000

/* TCA9548A channel */
#define LEGO_OLED_CHANNEL					TCA9548A_CHANNEL2

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
 * \brief Animations used
 */
typedef enum {
	LEGO_ANIM_AUTO_MODE,  		//!< Auto mode
	LEGO_ANIM_PALACE_CINEMA, 	//!< Palace cinema
	LEGO_ANIM_ROLLER_COASTER, 	//!< Roller Coaster
	LEGO_ANIM_TRAFFIC_LIGHTS	//!< Traffic lights
} LEGO_Anim_t;

/*!
 * \brief Groups enum for convenience
 */
typedef enum {
	LEGO_GROUP_BROADCAST = 1,		//!< Broadcast group
	/* Building groups */
	LEGO_GROUP_A , 					//!< Brick Bank (10251)
	LEGO_GROUP_B1,					//!< Assembly Square #1 (10255)
	LEGO_GROUP_B2,					//!< Assembly Square #2 (10255)
	LEGO_GROUP_C, 					//!< Parisian Restaurant (10243)
	LEGO_GROUP_D, 					//!< Palace Cinema (10243)
	LEGO_GROUP_E, 					//!< Train Station #1 (60050)
	LEGO_GROUP_F, 					//!< Corner Garage (10264)
	LEGO_GROUP_G, 					//!< Pet Shop #1 (10218)
	LEGO_GROUP_H, 					//!< Pet Shop #2 (10218)
	LEGO_GROUP_I, 					//!< Detective's Office (10246)
	LEGO_GROUP_J, 					//!< Downtown dinner (10260)
	LEGO_GROUP_K, 					//!< Park Street Townhouse #1 (31065)
	LEGO_GROUP_L, 					//!< Park Street Townhouse #2 (31065)
	LEGO_GROUP_M, 					//!< Train Station #2 (7997)
	LEGO_GROUP_N,  					//!< Roller Coaster (10261)
	/* Other groups and presets */
	LEGO_GROUP_STREET,				//!< Street lights
	LEGO_GROUP_INTERIOR,			//!< Interior lights
	LEGO_GROUP_EXTERIOR,			//!< Exterior lights
	/* Animations */
	LEGO_GROUP_ANIM_PALACE_CINEMA,	//!< Palace cinema animations
	LEGO_GROUP_ANIM_ROLLER_COASTER, //!< Roller Coaster animations
	LEGO_GROUP_ANIM_TRAFFIC_LIGHTS  //!< Traffic lights
} LEGO_LightGroup_t;

/*!
 * \brief Lightning time percentage in auto-mode
 */
typedef enum {
	LEGO_PERC_DISABLE = 0,		//!< Disabled in auto-mode
	LEGO_PERC_STREET = 80,   	//!< Street objects
	LEGO_PERC_EXTERIOR = 70, 	//!< Exterior lights
	LEGO_PERC_ROOMS = 50,    	//!< Rooms
	LEGO_PERC_CAFES = 70,    	//!< Cafes/restaurants
	LEGO_PERC_COMPANIES = 60,	//!< Companies/services
	LEGO_PERC_SMALL_OBJ = 70,	//!< Small objects
	LEGO_PERC_FAST_OBJ = 5,  	//!< Fast blinking
	LEGO_PERC_LONG = 75      	//!< Long lightning time
} LEGO_AutoModePercentage_t;

/*!
 * \brief LEGO Light information
 */
typedef struct {
	uint32_t lightId;							//!< Unique ID
	const LEGO_MCP23017Info_t *mcp23017Info;	//!< See LEGO_MCP23017Info_t
	uint8_t mcp23017Pin;						//!< MCP23017 pin
	uint32_t groupsId[LEGO_LIGHT_MAX_GROUPS];	//!< Specific groups the light belongs to
	uint8_t autoModePercentage;					//!< If 0 the light does not act in automatic mode. Otherwise greater value = the light will be turning on more often
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
} LEGO_SearchRes_t;

/*!
 * \brief Search patterns
 */
typedef enum {
	LEGO_SEARCH_ID,  		//!< Search by id
	LEGO_SEARCH_GROUP,		//!< Search by group
} LEGO_SearchPattern_t;

/*!
 * \brief Possible return values when using API functions
 */
typedef enum {
	LEGO_ID_NOT_FOUND,	//!< Id not found
	LEGO_I2C_ERR,		//!< I2C error
	LEGO_OP_PERFORMED,  //!< Success
	LEGO_RANGE_ERROR	//!< Value outside the valid range
} LEGO_LightOpRes_t;

/*!
 * \brief Structure for reading pin state
 */
typedef struct {
	uint32_t lightId; //!< Unique id
	bool state;		  //!< Pin state (1 = light turned on, 0 = light turned off)
} LEGO_LightStatus_t;

/*!
 * \brief Animation info structure
 */
typedef struct {
	TaskHandle_t taskHandle; //!< Task handle
	uint32_t delayMs;		 //!< Delay in milliseconds
	bool onOff;				 //!< True if animation enabled, false if disabled
} LEGO_AnimInfo_t;

/*!
 * \brief VL6180X info
 */
typedef struct {
	TCA9548A_Channel_t tca9548aChannel; //!< TCA9548A channel
	VL6180X_Devices_t devices;			//!< VL6180X devices
} LEGO_VL6180XInfo_t;

/*!
 * \brief Parking place structure
 */
typedef struct {
	uint8_t placeId;							//!< Unique ID
	const LEGO_VL6180XInfo_t *vl6180xInfo;		//!< VL6810X info
	uint8_t vl6180xDevNum;						//!< VL6180X device number
	const LEGO_MCP23017Info_t *mcp23017CEInfo;	//!< MCP23017 info */
	uint8_t mcp23017CEPin;						//!< MCP23017 chip-enable pin */
	bool occupied;								//!< True if place is occupied, false otherwise
} LEGO_ParkingPlace_t;

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
 * \brief Get total number of lights used
 *
 * \return The number of lights
 */
uint8_t LEGO_LightsCnt();

/*!
 * \brief Lights control function
 *
 * \param searchPattern : See LEGO_SearchPattern_t
 * \param id : Searched light/group id
 * \param op : See LEGO_LightOp_t
 * \return Instance of LEGO_LightOpRes_t
 */
LEGO_LightOpRes_t LEGO_LightsControl(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightOp_t op);

/*!
 * \brief Get lights status
 *
 * \param searchPattern : See LEGO_SearchPattern_t
 * \param id : Searched light/group id
 * \param statusBuff : The output buffer address will be written here
 * \param lightsFound : Output buffer for storing the number of lights found
 * \return Instance of LEGO_LightOpRes_t
 */
LEGO_LightOpRes_t LEGO_GetLightsStatus(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightStatus_t **statusBuff, uint8_t *lightsFound);

/*!
 * \brief Enable/disable specified animation
 *
 * \param animId : See LEGO_Anim_t
 * \param onOff : True for enabling, false for disabling
 * \return Instance of LEGO_LightOpRes_t
 */
LEGO_LightOpRes_t LEGO_AnimControl(LEGO_Anim_t animId, bool onOff);

/*!
 * \brief Set specified animation delay
 *
 * \param animId :  See LEGO_Anim_t
 * \param delayMs : Delay in milliseconds
 * \return Instance of LEGO_LightOpRes_t
 */
LEGO_LightOpRes_t LEGO_SetAnimDelay(LEGO_Anim_t animId, uint32_t delayMs);

/*!
 * \brief Get info about specific animation
 *
 * \param anim: See LEGO_Anim_t
 * \param otp : Output buffer
 * \return Instance of LEGO_LightOpRes_t
 */
LEGO_LightOpRes_t LEGO_GetAnimInfo(LEGO_Anim_t anim, const LEGO_AnimInfo_t **otp);

/*!
 * \brief Get parking places status
 *
 * \param stat : Output buffer
 * \param numOfPlaces : The number of parking places found
 */
void LEGO_GetParkingPlacesStatus(uint8_t **stat, uint8_t *numOfPlaces);

#endif /* LEGO_LEGO_H_ */
