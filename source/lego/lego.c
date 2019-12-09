#include "lego.h"
#include "board_conf.h"
#include "logger.h"
#include "assert.h"

/* emWin */
#include "GUI.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/* MCP23017 chain IDs */
#define LEGO_MCP23017_CHAIN0		0x00
#define LEGO_MCP23017_CHAIN0_DEV	8
#define LEGO_MCP23017_CHAIN1		0x01
#define LEGO_MCP23017_CHAIN1_DEV	3
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

/* Animations */
#define LEGO_PALACE_CINEMA_ANIM_FRAMES	3
#define LEGO_ROLLER_COASTER_ANIM_FRAMES	10

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE VARIABLES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Animation tasks handles */
static TaskHandle_t cinemaPalaceTask;
static TaskHandle_t rollerCoasterTask;

/* MCP23017 device chains */
static const LEGO_I2CDev_t mcp23017Chains[] = {
		{ TCA9548A_CHANNEL0, { LEGO_MCP23017_CHAIN0, LEGO_MCP23017_CHAIN0_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL1, { LEGO_MCP23017_CHAIN1, LEGO_MCP23017_CHAIN1_DEV, MCP23017_BASE_ADDR } }
};

/* MCP23017 devices. Exclude masks are calculated during startup so it cannot be const */
static LEGO_MCP23017Info_t mcp23017Devices[] = {
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
		{ LEGO_MCP23017_CH(1), 2, MCP23017_PORT_B }
};

/* The table containing information about lights */
static const LEGO_Light_t legoLights[] = {
		/* GROUP A - Brick Bank (10251) */
		{ 0,   LEGO_CH0_DEV0_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Street lamp */
		{ 1,   LEGO_CH0_DEV0_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Laundry room */
		{ 2,   LEGO_CH0_DEV0_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Exterior lights */
		{ 3,   LEGO_CH0_DEV0_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Desk #1 */
		{ 4,   LEGO_CH0_DEV0_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Chandelier */
		{ 5,   LEGO_CH0_DEV0_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_A } }, /* Desk #2 */
		/* Group B1 - Assembly Square #1 (10255) */
		{ 6,   LEGO_CH0_DEV1_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Street lamp #1 */
		{ 7,   LEGO_CH0_DEV1_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Street lamp #2 */
		{ 8,   LEGO_CH0_DEV1_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Fountain */
		{ 9,   LEGO_CH0_DEV1_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Room - 2nd floor */
		{ 10,  LEGO_CH0_DEV1_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Toilet - 2nd floor */
		{ 11,  LEGO_CH0_DEV1_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Glass-case #1 */
		{ 12,  LEGO_CH0_DEV1_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Exterior light - back */
		{ 13,  LEGO_CH0_DEV1_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Glass-case #1  */
		{ 14,  LEGO_CH0_DEV1_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Exterior lights- front */
		{ 15,  LEGO_CH0_DEV1_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Terrace */
		{ 16,  LEGO_CH0_DEV1_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Exterior lights - side */
		{ 17,  LEGO_CH0_DEV1_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Bakery */
		{ 18,  LEGO_CH0_DEV1_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Dentist office - UV light */
		{ 19,  LEGO_CH0_DEV1_PA, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Flower shop */
		{ 20,  LEGO_CH0_DEV1_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Photo studio */
		{ 21,  LEGO_CH0_DEV1_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B1 } }, /* Dentist office */
		/* Group B2 - Assembly Square #2 (10255) */
		{ 22,  LEGO_CH0_DEV2_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Dance studio */
		{ 23,  LEGO_CH0_DEV2_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Cafe - table */
		{ 24,  LEGO_CH0_DEV2_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Exterior lights #1 */
		{ 25,  LEGO_CH0_DEV2_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Stairs - ground floor */
		{ 26,  LEGO_CH0_DEV2_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Stairs - 2nd floor */
		{ 27,  LEGO_CH0_DEV2_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Stairs - 1st floor */
		{ 28,  LEGO_CH0_DEV2_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Music store */
		{ 29,  LEGO_CH0_DEV2_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Exterior lights #2 */
		{ 30,  LEGO_CH0_DEV2_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_B2 } }, /* Cafe */
		/* Group C - Parisian Restaurant (10243) */
		{ 31,  LEGO_CH0_DEV3_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Restaurant - table #1 */
		{ 32,  LEGO_CH0_DEV3_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Exterior lights */
		{ 33,  LEGO_CH0_DEV3_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Living room */
		{ 34,  LEGO_CH0_DEV3_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Balcony #1 */
		{ 35,  LEGO_CH0_DEV3_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Balcony #2 */
		{ 36,  LEGO_CH0_DEV3_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Painting studio */
		{ 37,  LEGO_CH0_DEV3_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Balcony #3 */
		{ 38,  LEGO_CH0_DEV3_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Balcony #4 */
		{ 39,  LEGO_CH0_DEV3_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Restaurant - table #2 */
		{ 40,  LEGO_CH0_DEV3_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_C } }, /* Street lamp */
		/* Group D - Palace Cinema (10243) */
		{ 41,  LEGO_CH0_DEV4_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D } }, /* Exterior lights */
		{ 42,  LEGO_CH0_DEV4_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D } }, /* Street lamp */
		{ 43,  LEGO_CH0_DEV4_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D } }, /* Cinema hall - projector */
		{ 44,  LEGO_CH0_DEV4_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D } }, /* Cinema hall */
		{ 45,  LEGO_CH0_DEV4_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_D } }, /* Cinema - ticket office */
		/* Group E - Train Station #1 (60050) */
		{ 46,  LEGO_CH0_DEV6_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E } }, /* Train station #1 */
		{ 47,  LEGO_CH0_DEV6_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E } }, /* Train station #2 */
		{ 48,  LEGO_CH0_DEV6_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E } }, /* Cash machine */
		{ 49,  LEGO_CH0_DEV6_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_E } }, /* Train depot */
		/* Group F - Corner Garage (10264) */
		{ 50,  LEGO_CH0_DEV5_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Exterior light - front */
		{ 51,  LEGO_CH0_DEV5_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Terrace */
		{ 52,  LEGO_CH0_DEV5_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Animal clinic */
		{ 53,  LEGO_CH0_DEV5_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Gas station - pump */
		{ 54,  LEGO_CH0_DEV5_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Exterior light - back */
		{ 55,  LEGO_CH0_DEV5_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Car repair shop - exterior lights */
		{ 56,  LEGO_CH0_DEV5_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Kitchen */
		{ 57,  LEGO_CH0_DEV5_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Car repair shop - workshop */
		{ 58,  LEGO_CH0_DEV5_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Car repair shop - office */
		{ 59,  LEGO_CH0_DEV5_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Desk */
		{ 60,  LEGO_CH0_DEV5_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Street lamp */
		{ 61,  LEGO_CH0_DEV5_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_F } }, /* Dining room */
		/* Group G - Pet Shop #1 (10218) */
		{ 62,  LEGO_CH0_DEV6_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G } }, /* Room under construction */
		{ 63,  LEGO_CH0_DEV6_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G } }, /* Living room */
		{ 64,  LEGO_CH0_DEV6_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G } }, /* Attic */
		{ 65,  LEGO_CH0_DEV6_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_G } }, /* Exterior light - back */
		/* Group H - Pet Shop #2 (10218) */
		{ 66,  LEGO_CH0_DEV6_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H } }, /* Pet shop */
		{ 67,  LEGO_CH0_DEV6_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H } }, /* Attic */
		{ 68,  LEGO_CH0_DEV6_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H } }, /* Street lamp */
		{ 69,  LEGO_CH0_DEV6_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H } }, /* Stairs */
		{ 70,  LEGO_CH0_DEV6_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_H } }, /* Kitchen */
		/* Group I - Detective's Office (10246) */
		{ 71,  LEGO_CH0_DEV7_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Kitchen */
		{ 72,  LEGO_CH0_DEV7_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Exterior light #1 */
		{ 73,  LEGO_CH0_DEV7_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Toilet */
		{ 74,  LEGO_CH0_DEV7_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Hairdresser #1 */
		{ 75,  LEGO_CH0_DEV7_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Detective's office */
		{ 76,  LEGO_CH0_DEV7_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Hairdresser #2 */
		{ 77,  LEGO_CH0_DEV7_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Exterior light #2 */
		{ 78,  LEGO_CH0_DEV7_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Exterior light #3 */
		{ 79,  LEGO_CH0_DEV7_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Detective's office - desk */
		{ 80,  LEGO_CH0_DEV7_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Game room */
		{ 81,  LEGO_CH0_DEV7_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_I } }, /* Street lamp */
		/* Group J - Downtown dinner (10260) */
		{ 82,  LEGO_CH1_DEV0_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Recording studio #1 */
		{ 83,  LEGO_CH1_DEV0_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Street lamp */
		{ 84,  LEGO_CH1_DEV0_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Cafe #2 */
		{ 85,  LEGO_CH1_DEV0_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Gym #1 */
		{ 86,  LEGO_CH1_DEV0_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Exterior light #1 */
		{ 87,  LEGO_CH1_DEV0_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Cafe #2 */
		{ 88,  LEGO_CH1_DEV0_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Gym #2 */
		{ 89,  LEGO_CH1_DEV0_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Exterior light #2 */
		{ 90,  LEGO_CH1_DEV0_PA, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Living room */
		{ 91,  LEGO_CH1_DEV0_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_J } }, /* Recording studio #2 */
		/* Group K - Park Street Townhouse #1 (31065) */
		{ 92,  LEGO_CH1_DEV1_PA, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K } }, /* Room */
		{ 93,  LEGO_CH1_DEV1_PA, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K } }, /* Terrace #1 */
		{ 94,  LEGO_CH1_DEV1_PA, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_K } }, /* Terrace #2 */
		/* Group L - Park Street Townhouse #2 (31065) */
		{ 95,  LEGO_CH1_DEV1_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L } }, /* Exterior light */
		{ 96,  LEGO_CH1_DEV1_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L } }, /* Street lamp */
		{ 97,  LEGO_CH1_DEV1_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_L } }, /* Room */
		/* Group M - Train Station #2 (7997) */
		{ 98,  LEGO_CH1_DEV1_PB, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Train station #1 */
		{ 99,  LEGO_CH1_DEV1_PB, 5, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Train station - 1st floor */
		{ 100, LEGO_CH1_DEV1_PB, 4, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Train station - board */
		{ 101, LEGO_CH1_DEV1_PB, 6, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Train station #2 */
		{ 102, LEGO_CH1_DEV1_PB, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Exterior light #1 */
		{ 103, LEGO_CH1_DEV1_PB, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_M } }, /* Exterior light #2 */
		/* Group N - Roller Coaster (10261) */
		{ 104, LEGO_CH1_DEV2_PA, 0, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N } }, /* Roller Coaster - gates */
		{ 105, LEGO_CH1_DEV2_PA, 1, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N } }, /* Tickets */
		{ 106, LEGO_CH1_DEV2_PA, 2, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N } }, /* Ice-cream shop */
		{ 107, LEGO_CH1_DEV2_PB, 3, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N } }, /* Candy floss shop */
		{ 108, LEGO_CH1_DEV2_PB, 7, { LEGO_GROUP_BROADCAST, LEGO_GROUP_N } }, /* Floodlights */
		/* Animation #1 - Palace Cinema (10243) */
		{ 109, LEGO_CH0_DEV4_PB, 0, {} }, /* Animation on/off */
		{ 110, LEGO_CH0_DEV4_PA, 0, { LEGO_GROUP_ANIM0 } }, /* Frame #1 */
		{ 111, LEGO_CH0_DEV4_PA, 1, { LEGO_GROUP_ANIM0 } }, /* Frame #2 */
		{ 112, LEGO_CH0_DEV4_PA, 2, { LEGO_GROUP_ANIM0 } }, /* Frame #3 */
		/* Animation #2 - Palace Cinema (10243) */
		{ 113, LEGO_CH1_DEV2_PA, 3, { LEGO_GROUP_ANIM1 } }, /* Frame #1 */
		{ 114, LEGO_CH1_DEV2_PA, 4, { LEGO_GROUP_ANIM1 } }, /* Frame #2 */
		{ 115, LEGO_CH1_DEV2_PA, 5, { LEGO_GROUP_ANIM1 } }, /* Frame #3 */
		{ 116, LEGO_CH1_DEV2_PA, 6, { LEGO_GROUP_ANIM1 } }, /* Frame #4 */
		{ 117, LEGO_CH1_DEV2_PB, 7, { LEGO_GROUP_ANIM1 } }, /* Frame #5 */
		{ 118, LEGO_CH1_DEV2_PB, 2, { LEGO_GROUP_ANIM1 } }, /* Frame #6 */
		{ 119, LEGO_CH1_DEV2_PB, 1, { LEGO_GROUP_ANIM1 } }, /* Frame #7 */
		{ 120, LEGO_CH1_DEV2_PB, 0, { LEGO_GROUP_ANIM1 } }, /* Frame #8 */
		{ 121, LEGO_CH1_DEV2_PB, 5, { LEGO_GROUP_ANIM1 } }, /* Frame #9 */
		{ 122, LEGO_CH1_DEV2_PB, 4, { LEGO_GROUP_ANIM1 } }, /* Frame #10 */
};

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE FUNCTIONS --------------------------- */
/* ----------------------------------------------------------------------------- */

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
	return light->lightId == id;
}

static inline bool LEGO_CompareOnOffTime(const LEGO_Light_t *light, uint32_t time)
{
	return true;
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- FREERTOS TASKS ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Task for Cinema Palace Animation */
static void LEGO_PalaceCinemaTask(void *pvParameters)
{
	vTaskSuspend(NULL);
}

/* Task for Roller Coaster Animation */
static void LEGO_RollerCoasterTask(void *pvParameters)
{
	vTaskSuspend(NULL);
}

static void LEGO_AutoModeTask(void)
{
	/* Copy auto mode enabled pins to the buffer */

}

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool LEGO_RTOSInit(void)
{
	BaseType_t cinemaTask = xTaskCreate(LEGO_PalaceCinemaTask, LEGO_TASK_PALACE_CINEMA_NAME, LEGO_TASK_PALACE_CINEMA_STACK, NULL, LEGO_TASK_PALACE_CINEMA_PRIO, &cinemaPalaceTask);
	BaseType_t coasterTask = xTaskCreate(LEGO_RollerCoasterTask, LEGO_TASK_ROLLER_COASTER_NAME, LEGO_TASK_ROLLER_COASTER_STACK, NULL, LEGO_TASK_ROLLER_COASTER_PRIO, &rollerCoasterTask);

	return (cinemaTask == pdPASS && coasterTask == pdPASS);
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

	return response;
}

uint8_t LEGO_SearchLights(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_SearchRes_t *searchRes)
{
	uint8_t devUsed = 0;
	const LEGO_Light_t *light;
	LEGO_SearchRes_t *resRow;

	for (uint8_t i = 0; i < GUI_COUNTOF(legoLights); i++) {
		light = &legoLights[i];

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
			for (uint8_t j = 0; j < devUsed; j++) {
				resRow = &searchRes[j];
				devFound = false;
				if (resRow->mcp23017Info == light->mcp23017Info) {
					/* Update entry */
					MCP23017_UINT8_BIT_SET(resRow->mask, light->mcp23017Pin);
					devFound = true;
					break;
				}
			}
			/* Create new entry */
			if (!devFound) {
				searchRes[devUsed].mcp23017Info = light->mcp23017Info;
				searchRes[devUsed].mask = MCP23017_UINT8_BIT(light->mcp23017Pin);
				devUsed++;
			}
		}
 	}

	return devUsed;
}

LEGO_LightOpRes_t LEGO_LightsControl(LEGO_SearchPattern_t searchPattern, uint32_t id, LEGO_LightOp_t op)
{
	/* Search lights */
	LEGO_SearchRes_t searchBuff[GUI_COUNTOF(mcp23017Devices)];
	uint8_t devFound = LEGO_SearchLights(searchPattern, id, searchBuff);

	if (!devFound) {
		return LEGO_ID_NOT_FOUND;
	}

	const LEGO_SearchRes_t *buffRow;
	const LEGO_MCP23017Info_t *mcp23017Info;
	uint8_t sendVal;

	for (uint8_t i = 0; i < devFound; i++) {
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
