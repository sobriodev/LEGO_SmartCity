#include "lego.h"
#include "board_conf.h"
#include "logger.h"
#include "assert.h"
#include "stdlib.h"
#include "oled.h"

/* emWin */
#include "GUI.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/* MCP23017 chain IDs */
#define LEGO_MCP23017_CHAIN0		0x00
#define LEGO_MCP23017_CHAIN0_DEV	8
#define LEGO_MCP23017_CHAIN1		0x01
#define LEGO_MCP23017_CHAIN1_DEV	3
#define LEGO_MCP23017_CHAIN2		0x02
#define LEGO_MCP23017_CHAIN2_DEV	1
#define LEGO_MCP23017_CH(CH)		(&mcp23017Chains[(CH)])

/* MCP23017 devices - just for convenience macros */
#define LEGO_CH0_DEV0_PA	(&mcp23017Devices[0])
#define LEGO_CH0_DEV0_PB	(&mcp23017Devices[1])
#define LEGO_CH0_DEV1_PA	(&mcp23017Devices[2])
#define LEGO_CH0_DEV1_PB	(&mcp23017Devices[3])
#define LEGO_CH0_DEV2_PA	(&mcp23017Devices[4])
#define LEGO_CH0_DEV2_PB	(&mcp23017Devices[5])
#define LEGO_CH0_DEV3_PA	(&mcp23017Devices[6])
#define LEGO_CH0_DEV3_PB	(&mcp23017Devices[7])
#define LEGO_CH0_DEV4_PA	(&mcp23017Devices[8])
#define LEGO_CH0_DEV4_PB	(&mcp23017Devices[9])
#define LEGO_CH0_DEV5_PA	(&mcp23017Devices[10])
#define LEGO_CH0_DEV5_PB	(&mcp23017Devices[11])
#define LEGO_CH0_DEV6_PA	(&mcp23017Devices[12])
#define LEGO_CH0_DEV6_PB	(&mcp23017Devices[13])
#define LEGO_CH0_DEV7_PA	(&mcp23017Devices[14])
#define LEGO_CH0_DEV7_PB	(&mcp23017Devices[15])

#define LEGO_CH1_DEV0_PA	(&mcp23017Devices[16])
#define LEGO_CH1_DEV0_PB	(&mcp23017Devices[17])
#define LEGO_CH1_DEV1_PA	(&mcp23017Devices[18])
#define LEGO_CH1_DEV1_PB	(&mcp23017Devices[19])
#define LEGO_CH1_DEV2_PA	(&mcp23017Devices[20])
#define LEGO_CH1_DEV2_PB	(&mcp23017Devices[21])

#define LEGO_CH2_DEV0_PA	(&mcp23017Devices[22])
#define LEGO_CH2_DEV0_PB	(&mcp23017Devices[23])

/* Animations */
#define LEGO_PALACE_CINEMA_ANIM_FRAMES	3
#define LEGO_ROLLER_COASTER_ANIM_FRAMES	10

/* Just for convenience */
#define LEGO_AUTO_MODE_INFO()		(&animInfo[0])
#define LEGO_PALACE_CINEMA_INFO()	(&animInfo[1])
#define LEGO_ROLLER_COASTER_INFO()	(&animInfo[2])
#define LEGO_TRAFFIC_LIGHTS_INFO()	(&animInfo[3])
#define LEGO_WIND_TURBINE_INFO()	(&animInfo[4])

/* VL6180X related */
#define VL6180X_DEV0				0x00
#define VL6180X_DEV0_DEVICES		10

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE DATA TYPES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Light search status */
typedef struct {
	uint8_t devUsed;
	uint8_t pinsUsed;
} LEGO_SearchStatus_t;

/* Auto-mode device status */
typedef struct {
	const LEGO_MCP23017Info_t *mcp23017Info;
	uint8_t mask;
	uint8_t portState;
} LEGO_AutoModeEntity_t;

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE VARIABLES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Animation settings */
static LEGO_AnimInfo_t animInfo[] = {
		/* Auto-mode */
		{ NULL, 5000, false},
		/* Palace cinema */
		{ NULL, 150, false },
		/* Roller coaster */
		{ NULL, 150, false },
		/* Traffic lights */
		{ NULL, 10000, false },
		/* Wind turbine */
		{ NULL, 0, false }
};

/* MCP23017 device chains */
static const LEGO_I2CDev_t mcp23017Chains[] = {
		{ TCA9548A_CHANNEL0, { LEGO_MCP23017_CHAIN0, LEGO_MCP23017_CHAIN0_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL1, { LEGO_MCP23017_CHAIN1, LEGO_MCP23017_CHAIN1_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL2, { LEGO_MCP23017_CHAIN2, LEGO_MCP23017_CHAIN2_DEV, MCP23017_BASE_ADDR + 1 } }
};

/* MCP23017 devices */
static const LEGO_MCP23017Info_t mcp23017Devices[] = {
		{ LEGO_MCP23017_CH(0), 0, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 2, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 3, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 4, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(1), 0, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(1), 2, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(1), 2, MCP23017_PORT_B },
		{ LEGO_MCP23017_CH(2), 0, MCP23017_PORT_A },
		{ LEGO_MCP23017_CH(2), 0, MCP23017_PORT_B },
};

/* VL6180X devices */
static const LEGO_VL6180XInfo_t vl6180xDevices = { TCA9548A_CHANNEL2, { 10, VL6180X_I2C_ADDR + 1 } };

/* Smart parking places. Occupied flag is changed during runtime so it cannot be const */
static LEGO_ParkingPlace_t parkingPlaces[] = {
		{ 0, &vl6180xDevices, 0, LEGO_CH2_DEV0_PA, 6 },
		{ 1, &vl6180xDevices, 1, LEGO_CH2_DEV0_PA, 7 },
		{ 2, &vl6180xDevices, 2, LEGO_CH2_DEV0_PA, 3 },
		{ 3, &vl6180xDevices, 3, LEGO_CH2_DEV0_PA, 5 },
		{ 4, &vl6180xDevices, 4, LEGO_CH2_DEV0_PA, 4 },
		{ 5, &vl6180xDevices, 5, LEGO_CH2_DEV0_PB, 4 },
		{ 6, &vl6180xDevices, 6, LEGO_CH2_DEV0_PB, 3 },
		{ 7, &vl6180xDevices, 7, LEGO_CH2_DEV0_PB, 5 },
		{ 8, &vl6180xDevices, 8, LEGO_CH2_DEV0_PB, 7 },
		{ 9, &vl6180xDevices, 9, LEGO_CH2_DEV0_PB, 6 }
};

/* The table containing information about lights */
static const LEGO_Light_t legoLights[] = {
		/* GROUP A - Brick Bank (10251) */
		{ 0,   LEGO_CH0_DEV0_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 1,   LEGO_CH0_DEV0_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Laundry room */
		{ 2,   LEGO_CH0_DEV0_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights */
		{ 3,   LEGO_CH0_DEV0_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_INTERIOR }, LEGO_PERC_SMALL_OBJ }, /* Desk #1 */
		{ 4,   LEGO_CH0_DEV0_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_INTERIOR }, LEGO_PERC_LONG }, /* Chandelier */
		{ 5,   LEGO_CH0_DEV0_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A, LEGO_GROUP_INTERIOR }, LEGO_PERC_SMALL_OBJ }, /* Desk #2 */
		/* Group B1 - Assembly Square #1 (10255) */
		{ 6,   LEGO_CH0_DEV1_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp #1 */
		{ 7,   LEGO_CH0_DEV1_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp #2 */
		{ 8,   LEGO_CH0_DEV1_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_EXTERIOR }, LEGO_PERC_LONG }, /* Fountain */
		{ 9,   LEGO_CH0_DEV1_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Room - 2nd floor */
		{ 10,  LEGO_CH0_DEV1_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Toilet - 2nd floor */
		{ 11,  LEGO_CH0_DEV1_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_LONG }, /* Glass-case #1 */
		{ 12,  LEGO_CH0_DEV1_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light - back */
		{ 13,  LEGO_CH0_DEV1_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_LONG }, /* Glass-case #1  */
		{ 14,  LEGO_CH0_DEV1_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights- front */
		{ 15,  LEGO_CH0_DEV1_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Terrace */
		{ 16,  LEGO_CH0_DEV1_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights - side */
		{ 17,  LEGO_CH0_DEV1_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Bakery */
		{ 18,  LEGO_CH0_DEV1_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_FAST_OBJ }, /* Dentist office - UV light */
		{ 19,  LEGO_CH0_DEV1_PA, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Flower shop */
		{ 20,  LEGO_CH0_DEV1_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Photo studio */
		{ 21,  LEGO_CH0_DEV1_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Dentist office */
		/* Group B2 - Assembly Square #2 (10255) */
		{ 22,  LEGO_CH0_DEV2_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Dance studio */
		{ 23,  LEGO_CH0_DEV2_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Cafe - table */
		{ 24,  LEGO_CH0_DEV2_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights #1 */
		{ 25,  LEGO_CH0_DEV2_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Stairs - ground floor */
		{ 26,  LEGO_CH0_DEV2_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Stairs - 2nd floor */
		{ 27,  LEGO_CH0_DEV2_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Stairs - 1st floor */
		{ 28,  LEGO_CH0_DEV2_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Music store */
		{ 29,  LEGO_CH0_DEV2_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights #2 */
		{ 30,  LEGO_CH0_DEV2_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Cafe */
		/* Group C - Parisian Restaurant (10243) */
		{ 31,  LEGO_CH0_DEV3_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Restaurant - table #1 */
		{ 32,  LEGO_CH0_DEV3_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights */
		{ 33,  LEGO_CH0_DEV3_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Living room */
		{ 34,  LEGO_CH0_DEV3_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Balcony #1 */
		{ 35,  LEGO_CH0_DEV3_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Balcony #2 */
		{ 36,  LEGO_CH0_DEV3_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Painting studio */
		{ 37,  LEGO_CH0_DEV3_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Balcony #3 */
		{ 38,  LEGO_CH0_DEV3_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Balcony #4 */
		{ 39,  LEGO_CH0_DEV3_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Restaurant - table #2 */
		{ 40,  LEGO_CH0_DEV3_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		/* Group D - Palace Cinema (10243) */
		{ 41,  LEGO_CH0_DEV4_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior lights */
		{ 42,  LEGO_CH0_DEV4_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 43,  LEGO_CH0_DEV4_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D, LEGO_GROUP_INTERIOR }, LEGO_PERC_FAST_OBJ }, /* Cinema hall - projector */
		{ 44,  LEGO_CH0_DEV4_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Cinema hall */
		{ 45,  LEGO_CH0_DEV4_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Cinema - ticket office */
		/* Group E - Train Station #1 (60050) */
		{ 46,  LEGO_CH0_DEV6_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E, LEGO_GROUP_INTERIOR }, LEGO_PERC_CAFES }, /* Train station #1 */
		{ 47,  LEGO_CH0_DEV6_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E, LEGO_GROUP_INTERIOR }, LEGO_PERC_CAFES }, /* Train station #2 */
		{ 48,  LEGO_CH0_DEV6_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E, LEGO_GROUP_EXTERIOR }, LEGO_PERC_FAST_OBJ }, /* Cash machine */
		{ 49,  LEGO_CH0_DEV6_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E, LEGO_GROUP_EXTERIOR }, LEGO_PERC_LONG }, /* Train depot */
		/* Group F - Corner Garage (10264) */
		{ 50,  LEGO_CH0_DEV5_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light - front */
		{ 51,  LEGO_CH0_DEV5_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Terrace */
		{ 52,  LEGO_CH0_DEV5_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Animal clinic */
		{ 53,  LEGO_CH0_DEV5_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_EXTERIOR }, LEGO_PERC_LONG }, /* Gas station - pump */
		{ 54,  LEGO_CH0_DEV5_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light - back */
		{ 55,  LEGO_CH0_DEV5_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Car repair shop - exterior lights */
		{ 56,  LEGO_CH0_DEV5_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Kitchen */
		{ 57,  LEGO_CH0_DEV5_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Car repair shop - workshop */
		{ 58,  LEGO_CH0_DEV5_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Car repair shop - office */
		{ 59,  LEGO_CH0_DEV5_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_SMALL_OBJ }, /* Desk */
		{ 60,  LEGO_CH0_DEV5_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 61,  LEGO_CH0_DEV5_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Dining room */
		/* Group G - Pet Shop #1 (10218) */
		{ 62,  LEGO_CH0_DEV6_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Room under construction */
		{ 63,  LEGO_CH0_DEV6_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Living room */
		{ 64,  LEGO_CH0_DEV6_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Attic */
		{ 65,  LEGO_CH0_DEV6_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light - back */
		/* Group H - Pet Shop #2 (10218) */
		{ 66,  LEGO_CH0_DEV6_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Pet shop */
		{ 67,  LEGO_CH0_DEV6_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Attic */
		{ 68,  LEGO_CH0_DEV6_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 69,  LEGO_CH0_DEV6_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Stairs */
		{ 70,  LEGO_CH0_DEV6_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Kitchen */
		/* Group I - Detective's Office (10246) */
		{ 71,  LEGO_CH0_DEV7_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Kitchen */
		{ 72,  LEGO_CH0_DEV7_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #1 */
		{ 73,  LEGO_CH0_DEV7_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Toilet */
		{ 74,  LEGO_CH0_DEV7_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Hairdresser #1 */
		{ 75,  LEGO_CH0_DEV7_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Detective's office */
		{ 76,  LEGO_CH0_DEV7_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Hairdresser #2 */
		{ 77,  LEGO_CH0_DEV7_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #2 */
		{ 78,  LEGO_CH0_DEV7_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #3 */
		{ 79,  LEGO_CH0_DEV7_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_SMALL_OBJ }, /* Detective's office - desk */
		{ 80,  LEGO_CH0_DEV7_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Game room */
		{ 81,  LEGO_CH0_DEV7_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		/* Group J - Downtown dinner (10260) */
		{ 82,  LEGO_CH1_DEV0_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Recording studio #1 */
		{ 83,  LEGO_CH1_DEV0_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 84,  LEGO_CH1_DEV0_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_CAFES }, /* Cafe #2 */
		{ 85,  LEGO_CH1_DEV0_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Gym #1 */
		{ 86,  LEGO_CH1_DEV0_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #1 */
		{ 87,  LEGO_CH1_DEV0_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_CAFES }, /* Cafe #2 */
		{ 88,  LEGO_CH1_DEV0_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Gym #2 */
		{ 89,  LEGO_CH1_DEV0_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #2 */
		{ 90,  LEGO_CH1_DEV0_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Living room */
		{ 91,  LEGO_CH1_DEV0_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J, LEGO_GROUP_INTERIOR }, LEGO_PERC_COMPANIES }, /* Recording studio #2 */
		/* Group K - Park Street Townhouse #1 (31065) */
		{ 92,  LEGO_CH1_DEV1_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Room */
		{ 93,  LEGO_CH1_DEV1_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Terrace #1 */
		{ 94,  LEGO_CH1_DEV1_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Terrace #2 */
		/* Group L - Park Street Townhouse #2 (31065) */
		{ 95,  LEGO_CH1_DEV1_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light */
		{ 96,  LEGO_CH1_DEV1_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L, LEGO_GROUP_STREET }, LEGO_PERC_STREET }, /* Street lamp */
		{ 97,  LEGO_CH1_DEV1_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Room */
		/* Group M - Train Station #2 (7997) */
		{ 98,  LEGO_CH1_DEV1_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Train station #1 */
		{ 99,  LEGO_CH1_DEV1_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Train station - 1st floor */
		{ 100, LEGO_CH1_DEV1_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_EXTERIOR }, LEGO_PERC_LONG }, /* Train station - board */
		{ 101, LEGO_CH1_DEV1_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_INTERIOR }, LEGO_PERC_ROOMS }, /* Train station #2 */
		{ 102, LEGO_CH1_DEV1_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #1 */
		{ 103, LEGO_CH1_DEV1_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M, LEGO_GROUP_EXTERIOR }, LEGO_PERC_EXTERIOR }, /* Exterior light #2 */
		/* Group N - Roller Coaster (10261) */
		{ 104, LEGO_CH1_DEV2_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N, LEGO_GROUP_EXTERIOR }, LEGO_PERC_CAFES }, /* Roller Coaster - gates */
		{ 105, LEGO_CH1_DEV2_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N, LEGO_GROUP_EXTERIOR }, LEGO_PERC_CAFES }, /* Tickets */
		{ 106, LEGO_CH1_DEV2_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N, LEGO_GROUP_EXTERIOR }, LEGO_PERC_CAFES }, /* Ice-cream shop */
		{ 107, LEGO_CH1_DEV2_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N, LEGO_GROUP_EXTERIOR }, LEGO_PERC_CAFES }, /* Candy floss shop */
		{ 108, LEGO_CH1_DEV2_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N, LEGO_GROUP_EXTERIOR }, LEGO_PERC_CAFES }, /* Floodlights */
		/* Animation #1 - Palace Cinema (10243) */
		//{ 109, LEGO_CH0_DEV4_PB, 0, {} }, /* Animation on/off. Not used in current version */
		{ 110, LEGO_CH0_DEV4_PA, 0, { LEGO_GROUP_ANIM_PALACE_CINEMA } }, /* Frame #1 */
		{ 111, LEGO_CH0_DEV4_PA, 1, { LEGO_GROUP_ANIM_PALACE_CINEMA } }, /* Frame #2 */
		{ 112, LEGO_CH0_DEV4_PA, 2, { LEGO_GROUP_ANIM_PALACE_CINEMA } }, /* Frame #3 */
		/* Animation #2 - Roller Coaster (10261)*/
		{ 113, LEGO_CH1_DEV2_PA, 3, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #1 */
		{ 114, LEGO_CH1_DEV2_PA, 4, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #2 */
		{ 115, LEGO_CH1_DEV2_PA, 5, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #3 */
		{ 116, LEGO_CH1_DEV2_PA, 6, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #4 */
		{ 117, LEGO_CH1_DEV2_PA, 7, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #5 */
		{ 118, LEGO_CH1_DEV2_PB, 2, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #6 */
		{ 119, LEGO_CH1_DEV2_PB, 1, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #7 */
		{ 120, LEGO_CH1_DEV2_PB, 0, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #8 */
		{ 121, LEGO_CH1_DEV2_PB, 5, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #9 */
		{ 122, LEGO_CH1_DEV2_PB, 4, { LEGO_GROUP_ANIM_ROLLER_COASTER } }, /* Frame #10 */
		/* Animation #3 - Traffic lights */
		{ 123, LEGO_CH0_DEV4_PA, 3, { LEGO_GROUP_ANIM_TRAFFIC_LIGHTS } }, /* Control #1 */
		{ 124, LEGO_CH0_DEV4_PA, 4, { LEGO_GROUP_ANIM_TRAFFIC_LIGHTS } }, /* Control #1 */
		{ 125, LEGO_CH0_DEV4_PA, 5, { LEGO_GROUP_ANIM_TRAFFIC_LIGHTS } }, /* Control #1 */
		{ 126, LEGO_CH0_DEV4_PA, 6, { LEGO_GROUP_ANIM_TRAFFIC_LIGHTS } }, /* Control #1 */
		{ 127, LEGO_CH0_DEV4_PA, 7, { LEGO_GROUP_ANIM_TRAFFIC_LIGHTS } }, /* Control #1 */
		/* Animation #4 - Wind turbine */
		{ 128, LEGO_CH1_DEV0_PA, 7, { LEGO_GROUP_ANIM_WIND_TURBINE } } /* Control #0 */
};

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE FUNCTIONS --------------------------- */
/* ----------------------------------------------------------------------------- */

/* Get parking place from device info */
static LEGO_ParkingPlace_t *LEGO_VL6180XInfoToParkingPlace(const VL6180X_Devices_t *dev, uint8_t devNum)
{
	LEGO_ParkingPlace_t *place;
	for (uint8_t i = 0; i < GUI_COUNTOF(parkingPlaces); i++) {
		place = &parkingPlaces[i];
		if (&place->vl6180xInfo->devices == dev && place->vl6180xDevNum == devNum) {
			return place;
		}
	}
	return NULL;
}

/* VL6180X chip enable */
static bool LEGO_VL6180XEnablePin(const VL6180X_Devices_t *dev, uint8_t devNum)
{
	LEGO_ParkingPlace_t *place = LEGO_VL6180XInfoToParkingPlace(dev, devNum);

	if (place == NULL) {
		return false;
	}

	/* Drive CE pin high - vl6180x device wake up */
	taskENTER_CRITICAL();
	if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, place->mcp23017CEInfo->i2cDevInfo->channel) != TCA9548A_SUCCESS) {
		taskEXIT_CRITICAL();
		return false;
	}
	if (MCP23017_PinWrite(&place->mcp23017CEInfo->i2cDevInfo->chain, place->mcp23017CEInfo->mcp23017DevNum, place->mcp23017CEInfo->mcp23017Port, place->mcp23017CEPin, MCP23017_PIN_HIGH)) {
		taskEXIT_CRITICAL();
		return false;
	}
	taskEXIT_CRITICAL();

	return true;
}

/* Smart parking startup */
static void LEGO_SmartParkingStartup()
{
	/* Set functions pointers */
	VL6180X_Init(BOARD_I2C_SendMultiByte, BOARD_I2C_ReadMultiByte, LEGO_VL6180XEnablePin);

	/* Clear CE pins - all devices in shutdown mode */
	LEGO_ParkingPlace_t *place;
	for (uint8_t i = 0; i < GUI_COUNTOF(parkingPlaces); i++) {
		place = &parkingPlaces[i];

		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, place->mcp23017CEInfo->i2cDevInfo->channel) != TCA9548A_SUCCESS) {
			LOGGER_WRITELN(("VL6180X initializing error (TCA9548A)"));
			return;
			taskEXIT_CRITICAL();
		}
		if (MCP23017_PinWrite(&place->mcp23017CEInfo->i2cDevInfo->chain, place->mcp23017CEInfo->mcp23017DevNum, place->mcp23017CEInfo->mcp23017Port, place->mcp23017CEPin, MCP23017_PIN_LOW)) {
			LOGGER_WRITELN(("VL6180X initializing error (MCP23017)"));
			taskEXIT_CRITICAL();
			return;
		}
		taskEXIT_CRITICAL();
	}

	if (VL6180X_DevInit(&vl6180xDevices.devices) != VL6180X_SUCCESS) {
		LOGGER_WRITELN(("VL6180X initializing error"));
		return;
	}

	/* Start range measurements in continuous mode */
	for (uint8_t i = 0; i < vl6180xDevices.devices.numDevices; i++) {
		VL6180X_StartRangeMesurements(&vl6180xDevices.devices, i);
	}
}

/* Check if specified light has searched group */
static bool LEGO_HasGroup(const LEGO_Light_t *light, uint32_t groupId)
{
	for (uint8_t i = 0; i < LEGO_LIGHT_MAX_GROUPS; i++) {
		if (light->groupsId[i] == groupId) {
			return true;
		}
	}
	return false;
}

/* Check if specified light has searched id */
static inline bool LEGO_HasId(const LEGO_Light_t *light, uint32_t id)
{
	return (light->lightId == id);
}

/* Compare random value with light percentage */
static inline bool LEGO_CompareOnOffTime(const LEGO_Light_t *light, uint32_t comp)
{
	return (light->autoModePercentage < comp);
}

/* Get light id from device info */
static bool LEGO_DevInfoToId(const LEGO_MCP23017Info_t *dev, uint8_t pin, uint32_t *res)
{
	for (uint8_t i = 0; i < GUI_COUNTOF(legoLights); i++) {
		if (legoLights[i].mcp23017Info == dev && legoLights[i].mcp23017Pin == pin) {
			*res = legoLights[i].lightId;
			return true;
		}
	}
	return false;
}

/* Calculate auto-mode frames */
static void LEGO_AutoModeUpdate(const LEGO_Light_t **autoModeLights, uint8_t autoModeLightsCnt, LEGO_AutoModeEntity_t *devInfo, LEGO_SearchStatus_t *status)
{
	/* Clear status fields */
	status->devUsed = 0;
	status->pinsUsed = 0;

	const LEGO_Light_t *light;
	for (uint8_t i = 0; i < autoModeLightsCnt; i++) {
		light = autoModeLights[i];

		/* Check whether light should be turned on or off */
		uint8_t random = rand() % 100 + 1; /* Range [1, 100] */
		bool onOff = LEGO_CompareOnOffTime(light, random);

		LEGO_AutoModeEntity_t *devRow;
		bool devFound;
		for (uint8_t i = 0; i < status->devUsed; i++) {
			devFound = false;
			devRow = &devInfo[i];
			if (light->mcp23017Info == devRow->mcp23017Info) {
				/* Update entry */
				devFound = true;
				MCP23017_UINT8_BIT_SET(devRow->mask, light->mcp23017Pin);

				/* Update status */
				status->pinsUsed++;

				/* Save valid pin state */
				if (onOff) {
					MCP23017_UINT8_BIT_SET(devRow->portState, light->mcp23017Pin);
				} else {
					MCP23017_UINT8_BIT_CLEAR(devRow->portState, light->mcp23017Pin);
				}

				break;
			}
		}
		if (!devFound) {
			/* Create new entry */
			devInfo[status->devUsed].mcp23017Info = light->mcp23017Info;
			devInfo[status->devUsed].mask = MCP23017_UINT8_BIT(light->mcp23017Pin);

			/* Update status */
			status->devUsed++;
			status->pinsUsed++;

			/* Save valid pin state */
			if (onOff) {
				devInfo[status->devUsed].portState = MCP23017_UINT8_BIT(light->mcp23017Pin);
			} else {
				devInfo[status->devUsed].portState = 0x00;
			}
		}
	}
}

/* Search lights */
static void LEGO_SearchLights(const LEGO_Light_t *lights, uint8_t lightsCnt, LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_SearchRes_t *searchRes, LEGO_SearchStatus_t *status)
{
	/* Clear status fields */
	status->devUsed = 0;
	status->pinsUsed = 0;

	const LEGO_Light_t *light;
	LEGO_SearchRes_t *resRow;

	for (uint8_t i = 0; i < lightsCnt; i++) {
		light = &lights[i];

		/* Search */
		bool result;
		switch (searchPattern) {
		case LEGO_SEARCH_ID:
			result = LEGO_HasId(light, id);
			break;
		case LEGO_SEARCH_GROUP:
			result = LEGO_HasGroup(light, id);
			break;
		}

		if (result) {
			bool devFound;
			for (uint8_t j = 0; j < status->devUsed; j++) {
				resRow = &searchRes[j];
				devFound = false;
				if (resRow->mcp23017Info == light->mcp23017Info) {
					/* Update entry */
					MCP23017_UINT8_BIT_SET(resRow->mask, light->mcp23017Pin);
					devFound = true;

					/* Update status */
					status->pinsUsed++;

					break;
				}
			}
			/* Create new entry */
			if (!devFound) {
				searchRes[status->devUsed].mcp23017Info = light->mcp23017Info;
				searchRes[status->devUsed].mask = MCP23017_UINT8_BIT(light->mcp23017Pin);

				/* Update status */
				status->devUsed++;
				status->pinsUsed++;
			}
		}
 	}
}

/* Draw parking place flag */
static void LEGO_DrawParkingPlaceFlag(uint8_t place)
{
	uint8_t xBase = 20;
	uint8_t yBase = 8;

	uint8_t x = (place > 4) ? xBase + 42 : xBase;
	uint8_t y = yBase + ((place % 5) * 12);

	OLED_Draw_Line(x - 3, y - 3, x + 3, y + 3, parkingPlaces[place].occupied);
	OLED_Draw_Line(x + 3, y - 3, x - 3, y + 3, parkingPlaces[place].occupied);
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- FREERTOS TASKS ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task for Cinema Palace Animation */
static void LEGO_PalaceCinemaTask(void *pvParameters)
{
	uint8_t mask = 0x07;
	uint8_t states[4] = {0b00000000, 0b00000100, 0b00000110, 0b00000111};
	uint8_t i;
	while (1) {
		for (i = 0; i < 4; i++) {
			taskENTER_CRITICAL();
			if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH0_DEV4_PA->i2cDevInfo->channel)) {
				MCP23017_PortWriteMasked(&LEGO_CH0_DEV4_PA->i2cDevInfo->chain, LEGO_CH0_DEV4_PA->mcp23017DevNum, MCP23017_PORT_A, mask, states[i]);
			}
			taskEXIT_CRITICAL();
			vTaskDelay(pdMS_TO_TICKS(LEGO_PALACE_CINEMA_INFO()->delayMs));
		}
	}
}

/* Task for Roller Coaster Animation */
static void LEGO_RollerCoasterTask(void *pvParameters)
{
	uint8_t mask1 = 0b11111000;
	uint8_t mask2 = 0b00110111;
	uint8_t states[2] = {0x00, 0xFF};
	uint8_t i = 0;
	while (1) {
		for (i = 0; i < 2; i++) {
			taskENTER_CRITICAL();
			if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH1_DEV2_PA->i2cDevInfo->channel)) {
				MCP23017_PortWriteMasked(&LEGO_CH1_DEV2_PA->i2cDevInfo->chain, LEGO_CH1_DEV2_PA->mcp23017DevNum, MCP23017_PORT_A, mask1, states[i]);
				MCP23017_PortWriteMasked(&LEGO_CH1_DEV2_PA->i2cDevInfo->chain, LEGO_CH1_DEV2_PA->mcp23017DevNum, MCP23017_PORT_B, mask2, states[i]);
			}
			taskEXIT_CRITICAL();
			vTaskDelay(pdMS_TO_TICKS(LEGO_ROLLER_COASTER_INFO()->delayMs));
		}
	}
}

/* Task for auto mode */
static void LEGO_AutoModeTask(void *pvParameters)
{
	/* Devices buffer */
	LEGO_AutoModeEntity_t devices[GUI_COUNTOF(mcp23017Devices)];

	/* Auto-mode lights buffer */
	const LEGO_Light_t *autoModeLights[GUI_COUNTOF(legoLights)];
	uint8_t autoModeLightsCnt = 0;

	/* Copy auto mode lights pointers into internal buffer. This is done only once during task startup */
	const LEGO_Light_t *light;
	for (uint8_t i = 0; i < GUI_COUNTOF(legoLights); i++) {
		light = &legoLights[i];
		if (light->autoModePercentage) {
			autoModeLights[autoModeLightsCnt] = light;
			autoModeLightsCnt++;
		}
	}

	/* Seed random generator */
	srand(UINT32_MAX);

	/* Task loop */
	while (1) {
		LEGO_SearchStatus_t searchStatus;
		LEGO_AutoModeUpdate(autoModeLights, autoModeLightsCnt, devices, &searchStatus);

		const LEGO_AutoModeEntity_t *dev;
		for (uint8_t i = 0; i < searchStatus.devUsed; i++) {
			dev = &devices[i];

			/* Update MCP23017 port data */
			taskENTER_CRITICAL();
			if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, dev->mcp23017Info->i2cDevInfo->channel)) {
				MCP23017_PortWriteMasked(&dev->mcp23017Info->i2cDevInfo->chain, dev->mcp23017Info->mcp23017DevNum, dev->mcp23017Info->mcp23017Port, dev->mask, dev->portState);
			}
			taskEXIT_CRITICAL();
		}

		vTaskDelay(pdMS_TO_TICKS(LEGO_AUTO_MODE_INFO()->delayMs));
	}
}

/* Task for parking places */
static void LEGO_ParkingTask(void *pvParameters)
{
	/* Buffer for sprintf usage */
	static char oledBuff[10];

	while (1) {
		LEGO_ParkingPlace_t *place;
		for (uint8_t i = 0; i < GUI_COUNTOF(parkingPlaces); i++) {
			place = &parkingPlaces[i];

			taskENTER_CRITICAL();
			if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, place->vl6180xInfo->tca9548aChannel) == TCA9548A_SUCCESS) {
				/* Read range */
				uint8_t range;
				if (VL6180X_ReadRange(&place->vl6180xInfo->devices, place->vl6180xDevNum, &range) == VL6180X_SUCCESS) {
					/* Update status */
					if (range > LEGO_PARKING_TRESHOLD) {
						place->occupied = false;
					} else {
						place->occupied = true;
					}
				}
				/* Clear int status */
				VL6180X_ClearIntStatus(&place->vl6180xInfo->devices, place->vl6180xInfo->tca9548aChannel);
			}

			taskEXIT_CRITICAL();
		}

		/* Update OLED */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_OLED_CHANNEL) == TCA9548A_SUCCESS) {
			uint8_t freePlaces = 0;
			for (uint8_t i = 0; i < GUI_COUNTOF(parkingPlaces); i++) {
				freePlaces += !parkingPlaces[i].occupied;
				LEGO_DrawParkingPlaceFlag(i);
			}
			OLED_Puts(85, 2, " Free");
			OLED_Puts(85, 3, "Places");
			sprintf(oledBuff, "  %02d", freePlaces);
			OLED_Puts(85, 5, oledBuff);
			OLED_Refresh_Gram();
		}
		taskEXIT_CRITICAL();

		vTaskDelay(pdMS_TO_TICKS(LEGO_PARKING_REFRESH_MS));
	}
}

/* Task for traffic lights */
static void LEGO_TrafficLightsTask(void *pvParameters)
{
	uint8_t mask = 0b11111000;
	while (1) {
		/* State 1 - RED */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH0_DEV4_PA->i2cDevInfo->channel)) {
			MCP23017_PortWriteMasked(&LEGO_CH0_DEV4_PA->i2cDevInfo->chain, LEGO_CH0_DEV4_PA->mcp23017DevNum, MCP23017_PORT_A, mask, 0b10110111);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(pdMS_TO_TICKS(LEGO_TRAFFIC_LIGHTS_INFO()->delayMs));

		/* State 2 - RED|YELLOW */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH0_DEV4_PA->i2cDevInfo->channel)) {
			MCP23017_PortWriteMasked(&LEGO_CH0_DEV4_PA->i2cDevInfo->chain, LEGO_CH0_DEV4_PA->mcp23017DevNum, MCP23017_PORT_A, mask, 0b11010111);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(pdMS_TO_TICKS(2000));

		/* State 3 - GREEN */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH0_DEV4_PA->i2cDevInfo->channel)) {
			MCP23017_PortWriteMasked(&LEGO_CH0_DEV4_PA->i2cDevInfo->chain, LEGO_CH0_DEV4_PA->mcp23017DevNum, MCP23017_PORT_A, mask, 0b01101111);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(pdMS_TO_TICKS(LEGO_TRAFFIC_LIGHTS_INFO()->delayMs));

		/* State 4 - YELLOW */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_CH0_DEV4_PA->i2cDevInfo->channel)) {
			MCP23017_PortWriteMasked(&LEGO_CH0_DEV4_PA->i2cDevInfo->chain, LEGO_CH0_DEV4_PA->mcp23017DevNum, MCP23017_PORT_A, mask, 0b01011111);
		}
		taskEXIT_CRITICAL();
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool LEGO_RTOSInit(void)
{
	/* Create animation tasks */
	BaseType_t cinemaTask = xTaskCreate(LEGO_PalaceCinemaTask, LEGO_TASK_PALACE_CINEMA_NAME, LEGO_TASK_PALACE_CINEMA_STACK, NULL, LEGO_TASK_PALACE_CINEMA_PRIO, &LEGO_PALACE_CINEMA_INFO()->taskHandle);
	BaseType_t coasterTask = xTaskCreate(LEGO_RollerCoasterTask, LEGO_TASK_ROLLER_COASTER_NAME, LEGO_TASK_ROLLER_COASTER_STACK, NULL, LEGO_TASK_ROLLER_COASTER_PRIO, &LEGO_ROLLER_COASTER_INFO()->taskHandle);
	BaseType_t autoTask	= xTaskCreate(LEGO_AutoModeTask, LEGO_TASK_AUTO_MODE_NAME, LEGO_TASK_AUTO_MODE_STACK, NULL, LEGO_TASK_AUTO_MODE_PRIO, &LEGO_AUTO_MODE_INFO()->taskHandle);
	BaseType_t trafficTask = xTaskCreate(LEGO_TrafficLightsTask, LEGO_TASK_TRAFFIC_LIGHTS_NAME, LEGO_TASK_TRAFFIC_LIGHTS_STACK, NULL, LEGO_TASK_TRAFFIC_LIGHTS_PRIO, &LEGO_TRAFFIC_LIGHTS_INFO()->taskHandle);

	/* Parking task */
	BaseType_t parkingTask = xTaskCreate(LEGO_ParkingTask, LEGO_TASK_PARKING_NAME, LEGO_TASK_PARKING_STACK, NULL, LEGO_TASK_PARKING_PRIO, NULL);

	if (cinemaTask == pdPASS && coasterTask == pdPASS && autoTask == pdPASS && trafficTask == pdPASS && parkingTask == pdPASS) {
		/* Task suspended on startup */
		vTaskSuspend(LEGO_PALACE_CINEMA_INFO()->taskHandle);
		vTaskSuspend(LEGO_ROLLER_COASTER_INFO()->taskHandle);
		vTaskSuspend(LEGO_AUTO_MODE_INFO()->taskHandle);
		vTaskSuspend(LEGO_TRAFFIC_LIGHTS_INFO()->taskHandle);
		return true;
	}

	return false;
}

bool LEGO_PerformStartup(void)
{
	/* Set TCA9548A and MCP23017 send/receive I2C functions */
	TCA9548A_Init(BOARD_I2C_SendSingleReg, BOARD_I2C_ReadSingleReg);
	MCP23017_Init(BOARD_I2C_SendMultiReg, BOARD_I2C_ReadMultiReg);

	bool response = true;

	/* MCP23017 init */
	for (uint8_t i = 0; i < GUI_COUNTOF(mcp23017Chains); i++) {

		/* Select correct I2C channel */
		TCA9548A_Response_t tcaRes = TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, mcp23017Chains[i].channel);
		if (tcaRes != TCA9548A_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot choose TCA9548A channel (mask #%d). Response code %d", mcp23017Chains[i].channel, tcaRes));
			response = false;
			continue;
		}

		MCP23017_StatusCode_t mcpRes = MCP23017_ChainInit(&mcp23017Chains[i].chain, 0x00);
		if (mcpRes != MCP23017_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot initialize MCP23017 chain #%d. Status code %d", i, mcpRes));
			response = false;
		}

		/* By default all pins are output with logic high (lights off) */
		for (uint8_t j = 0; j < mcp23017Chains[i].chain.numDevices; j++) {
			if (MCP23017_PortSetDirection(&mcp23017Chains[i].chain, j, MCP23017_PORT_A, 0x00) != MCP23017_SUCCESS ||
				MCP23017_PortSetDirection(&mcp23017Chains[i].chain, j, MCP23017_PORT_B, 0x00) != MCP23017_SUCCESS) {
				response = false;
			}

			if (MCP23017_PortWrite(&mcp23017Chains[i].chain, j, MCP23017_PORT_A, 0xFF) != MCP23017_SUCCESS ||
				MCP23017_PortWrite(&mcp23017Chains[i].chain, j, MCP23017_PORT_B, 0xFF) != MCP23017_SUCCESS) {
				response = false;
			}
		}
	}

	/* VL6180x init */
	LEGO_SmartParkingStartup();

	/* OLED init */
	TCA9548A_Response_t tcaRes = TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, LEGO_OLED_CHANNEL);
	if (tcaRes != TCA9548A_SUCCESS) {
		LOGGER_WRITELN(("Cannot initialize OLED (TCA9548A)"));
		return false;
	}
	OLED_Init(I2C1);
	OLED_Draw_Bitmap(parkingBackground);

	/* Turn off cinema and wind turbine states */
	LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_PALACE_CINEMA, LEGO_LIGHT_ON);
	LEGO_AnimControl(LEGO_ANIM_WIND_TURBINE, false);


	return response;
}

uint8_t LEGO_LightsCnt()
{
	return GUI_COUNTOF(legoLights);
}

LEGO_LightOpRes_t LEGO_LightsControl(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightOp_t op)
{
	/* Search lights */
	LEGO_SearchRes_t searchBuff[GUI_COUNTOF(mcp23017Devices)];
	LEGO_SearchStatus_t searchStatus;
	LEGO_SearchLights(legoLights, GUI_COUNTOF(legoLights), searchPattern, id, searchBuff, &searchStatus);

	if (!searchStatus.devUsed) {
		return LEGO_ID_NOT_FOUND;
	}

	const LEGO_SearchRes_t *buffRow;
	const LEGO_MCP23017Info_t *mcp23017Info;
	uint8_t sendVal;

	for (uint8_t i = 0; i < searchStatus.devUsed; i++) {
		buffRow = &searchBuff[i];

		/* Change i2c mux channel */
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannels(TCA9548A_DEFAULT_ADDR, buffRow->mcp23017Info->i2cDevInfo->channel) != TCA9548A_SUCCESS) {
			taskEXIT_CRITICAL();
			return LEGO_I2C_ERR;
		}

		mcp23017Info = buffRow->mcp23017Info;

		switch (op) {
		case LEGO_LIGHT_ON:
			sendVal = 0x00;
			break;
		case LEGO_LIGHT_OFF:
			sendVal = 0xFF;
			break;
		case LEGO_LIGHT_TOGGLE:
			/* Current port state must be read */
			if (MCP23017_PortRead(&mcp23017Info->i2cDevInfo->chain, mcp23017Info->mcp23017DevNum, mcp23017Info->mcp23017Port, &sendVal) != MCP23017_SUCCESS) {
				taskEXIT_CRITICAL();
				return LEGO_I2C_ERR;
			}
			sendVal = ~sendVal;
			break;
		}

		/* Send new port states */
		if (MCP23017_PortWriteMasked(&mcp23017Info->i2cDevInfo->chain, mcp23017Info->mcp23017DevNum, mcp23017Info->mcp23017Port, buffRow->mask, sendVal) != MCP23017_SUCCESS) {
			taskEXIT_CRITICAL();
			return LEGO_I2C_ERR;
		}
		taskEXIT_CRITICAL();
	}

	return LEGO_OP_PERFORMED;
}

LEGO_LightOpRes_t LEGO_GetLightsStatus(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightStatus_t **statusBuff, uint8_t *lightsFound)
{
	/* Should be static -> less runtime cost */
	static LEGO_LightStatus_t response[GUI_COUNTOF(legoLights)];

	/* Search lights */
	LEGO_SearchRes_t searchBuff[GUI_COUNTOF(mcp23017Devices)];
	LEGO_SearchStatus_t searchStatus;
	LEGO_SearchLights(legoLights, GUI_COUNTOF(legoLights), searchPattern, id, searchBuff, &searchStatus);

	if (!searchStatus.devUsed) {
		return LEGO_ID_NOT_FOUND;
	}

	uint8_t found = 0;

	const LEGO_SearchRes_t *searchRow;
	for (uint8_t i = 0; i < searchStatus.devUsed; i++) {
		searchRow = &searchBuff[i];

		/* Read port status */
		uint8_t portStatus;
		taskENTER_CRITICAL();
		if (TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, searchRow->mcp23017Info->i2cDevInfo->channel) != TCA9548A_SUCCESS) {
			taskEXIT_CRITICAL();
			return LEGO_I2C_ERR;
		}
		if (MCP23017_PortRead(&searchRow->mcp23017Info->i2cDevInfo->chain, searchRow->mcp23017Info->mcp23017DevNum, searchRow->mcp23017Info->mcp23017Port, &portStatus) != MCP23017_SUCCESS) {
			taskEXIT_CRITICAL();
			return LEGO_I2C_ERR;
		}
		taskEXIT_CRITICAL();

		/* Iterate over all pins and store results */
		for (uint8_t i = 0; i < MCP23017_PORT_PINS; i++) {
			if (MCP23017_UINT8_IS_BIT_SET(searchRow->mask, i)) {
				uint32_t lightId;
				LEGO_DevInfoToId(searchRow->mcp23017Info, i, &lightId);
				/* Store output info */
				response[found].lightId = lightId;
				response[found].state = !MCP23017_UINT8_IS_BIT_SET(portStatus, i);
				found++;
			}
		}
	}

	*lightsFound = found;
	*statusBuff = response;

	return LEGO_OP_PERFORMED;
}

LEGO_LightOpRes_t LEGO_AnimControl(LEGO_Anim_t animId, bool onOff)
{
	LEGO_AnimInfo_t *info;

	if (onOff) {
		switch (animId) {
		case LEGO_ANIM_AUTO_MODE:
			info = LEGO_AUTO_MODE_INFO();
			break;
		case LEGO_ANIM_PALACE_CINEMA:
			info = LEGO_PALACE_CINEMA_INFO();
			break;
		case LEGO_ANIM_ROLLER_COASTER:
			info = LEGO_ROLLER_COASTER_INFO();
			break;
		case LEGO_ANIM_TRAFFIC_LIGHTS:
			info = LEGO_TRAFFIC_LIGHTS_INFO();
			break;
		case LEGO_ANIM_WIND_TURBINE:
			/* Wind turbine does not have own rtos task! */
			LEGO_WIND_TURBINE_INFO()->onOff = true;
			return LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_WIND_TURBINE, LEGO_LIGHT_OFF);
		default:
			return LEGO_ID_NOT_FOUND;
		}

		/* Resume task and set onOff flag */
		vTaskResume(info->taskHandle);
		info->onOff = true;
		return LEGO_OP_PERFORMED;

	} else {
		LEGO_LightOpRes_t res;

		switch (animId) {
		case LEGO_ANIM_AUTO_MODE:
			/* In auto-mode just suspend the task */
			vTaskSuspend(LEGO_AUTO_MODE_INFO()->taskHandle);
			LEGO_AUTO_MODE_INFO()->onOff = false;
			res = LEGO_OP_PERFORMED;
			break;
		case LEGO_ANIM_PALACE_CINEMA:
			/* Palace cinema - drive pins low */
			vTaskSuspend(LEGO_PALACE_CINEMA_INFO()->taskHandle);
			LEGO_PALACE_CINEMA_INFO()->onOff = false;
			res = LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_PALACE_CINEMA, LEGO_LIGHT_ON);
			break;
		case LEGO_ANIM_ROLLER_COASTER:
			/* Roller Coaster - drive pins high */
			vTaskSuspend(LEGO_ROLLER_COASTER_INFO()->taskHandle);
			LEGO_ROLLER_COASTER_INFO()->onOff = false;
			res = LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_ROLLER_COASTER, LEGO_LIGHT_OFF);
			break;
		case LEGO_ANIM_TRAFFIC_LIGHTS:
			/* Traffic lights - drive pin high */
			vTaskSuspend(LEGO_TRAFFIC_LIGHTS_INFO()->taskHandle);
			LEGO_TRAFFIC_LIGHTS_INFO()->onOff = false;
			res = LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_TRAFFIC_LIGHTS, LEGO_LIGHT_OFF);
			break;
		case LEGO_ANIM_WIND_TURBINE:
			LEGO_WIND_TURBINE_INFO()->onOff = false;
			res = LEGO_LightsControl(LEGO_SEARCH_GROUP, LEGO_GROUP_ANIM_WIND_TURBINE, LEGO_LIGHT_ON);
			break;
		default:
			res = LEGO_ID_NOT_FOUND;
			break;
		}

		return res;
	}
}

LEGO_LightOpRes_t LEGO_SetAnimDelay(LEGO_Anim_t animId, uint32_t delayMs)
{
	uint32_t min;
	uint32_t max;
	LEGO_AnimInfo_t *info;

	switch (animId) {
	case LEGO_ANIM_AUTO_MODE:
		info = LEGO_AUTO_MODE_INFO();
		min = LEGO_AUTO_MODE_DELAY_MIN;
		max = LEGO_AUTO_MODE_DELAY_MAX;
		break;
	case LEGO_ANIM_PALACE_CINEMA:
		info = LEGO_PALACE_CINEMA_INFO();
		min = LEGO_PALACE_CINEMA_DELAY_MIN;
		max = LEGO_PALACE_CINEMA_DELAY_MAX;
		break;
	case LEGO_ANIM_ROLLER_COASTER:
		info = LEGO_ROLLER_COASTER_INFO();
		min = LEGO_ROLLER_COASTER_DELAY_MIN;
		max = LEGO_ROLLER_COASTER_DELAY_MAX;
		break;
	case LEGO_ANIM_TRAFFIC_LIGHTS:
		info = LEGO_TRAFFIC_LIGHTS_INFO();
		min = LEGO_TRAFFIC_LIGHTS_TIME_MIN;
		max = LEGO_TRAFFIC_LIGHTS_TIME_MAX;
		break;
	case LEGO_ANIM_WIND_TURBINE:
		return LEGO_UNSUPPORTED;
	default:
		return LEGO_ID_NOT_FOUND;
	}

	/* Check if delay is inside the valid range */
	if (delayMs < min || delayMs > max) {
		return LEGO_RANGE_ERROR;
	}

	info->delayMs = delayMs;
	return LEGO_OP_PERFORMED;
}

LEGO_LightOpRes_t LEGO_GetAnimInfo(LEGO_Anim_t anim, const LEGO_AnimInfo_t **otp)
{
	switch (anim) {
	case LEGO_ANIM_AUTO_MODE:
		*otp = LEGO_AUTO_MODE_INFO();
		break;
	case LEGO_ANIM_PALACE_CINEMA:
		*otp = LEGO_PALACE_CINEMA_INFO();
		break;
	case LEGO_ANIM_ROLLER_COASTER:
		*otp = LEGO_ROLLER_COASTER_INFO();
		break;
	case LEGO_ANIM_TRAFFIC_LIGHTS:
		*otp = LEGO_TRAFFIC_LIGHTS_INFO();
		break;
	case LEGO_ANIM_WIND_TURBINE:
		*otp = LEGO_WIND_TURBINE_INFO();
		break;
	default:
		return LEGO_ID_NOT_FOUND;
	}

	return LEGO_OP_PERFORMED;
}

void LEGO_GetParkingPlacesStatus(uint8_t **stat, uint8_t *numOfPlaces)
{
	static uint8_t status[GUI_COUNTOF(parkingPlaces)];
	*numOfPlaces = GUI_COUNTOF(parkingPlaces);

	for (uint8_t i = 0; i < *numOfPlaces; i++) {
		status[i] = parkingPlaces[i].occupied;
	}
	*stat = status;
}
