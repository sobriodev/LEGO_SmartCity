#include "lego.h"
#include "board_conf.h"
#include "logger.h"
#include "assert.h"

/* emWin */
#include "GUI.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

/* MCP23017 chain IDs */
#define LEGO_MCP23017_CHAIN0		0x00
#define LEGO_MCP23017_CHAIN0_DEV	8
#define LEGO_MCP23017_CHAIN1		0x01
#define LEGO_MCP23017_CHAIN1_DEV	2
#define LEGO_MCP23017_CH(CH)		(&mcp23017Devices[(CH)])

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE VARIABLES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* MCP23017 devices */
static const LEGO_MCP23017Info_t mcp23017Devices[] = {
		{ TCA9548A_CHANNEL0, { LEGO_MCP23017_CHAIN0, LEGO_MCP23017_CHAIN0_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL1, { LEGO_MCP23017_CHAIN1, LEGO_MCP23017_CHAIN1_DEV, MCP23017_BASE_ADDR } }
};

/* The table contains information about static lights only. Animated lights are stored in animation arrays */
static const LEGO_Light_t legoLights[] = {
		/* GROUP A - Brick Bank (10251) */
		{ LEGO_GROUP_A,  0,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_A,  1,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_A,  2,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_A,  3,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_A,  4,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_A,  5,   LEGO_MCP23017_CH(0), 0, MCP23017_PORT_B, 4 },
		/* Group B1 - Assembly Square #1 (10255) */
		{ LEGO_GROUP_B1, 6,   LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_B1, 7,   LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_B1, 8,   LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_B1, 9,   LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 3 },
		{ LEGO_GROUP_B1, 10,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_B1, 11,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_B1, 12,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_B1, 13,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_B, 7 },
		{ LEGO_GROUP_B1, 14,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 2 },
		{ LEGO_GROUP_B1, 15,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 3 },
		{ LEGO_GROUP_B1, 16,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 4 },
		{ LEGO_GROUP_B1, 17,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 5 },
		{ LEGO_GROUP_B1, 18,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 6 },
		{ LEGO_GROUP_B1, 19,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 7 },
		{ LEGO_GROUP_B1, 20,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_B1, 21,  LEGO_MCP23017_CH(0), 1, MCP23017_PORT_A, 1 },
		/* Group B2 - Assembly Square #2 (10255) */
		{ LEGO_GROUP_B2, 22,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_B2, 23,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_B2, 24,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_B2, 25,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_B2, 26,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_B2, 27,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_B2, 28,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_B2, 29,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_B2, 30,  LEGO_MCP23017_CH(0), 2, MCP23017_PORT_A, 2 },
		/* Group C - Parisian Restaurant (10243) */
		{ LEGO_GROUP_C,  31,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_C,  32,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_C,  33,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_C,  34,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_C,  35,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_C,  36,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_C,  37,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_C,  38,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_C,  39,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_A, 3 },
		{ LEGO_GROUP_C,  40,  LEGO_MCP23017_CH(0), 3, MCP23017_PORT_A, 4 },
		/* Group D - Palace Cinema (10243) */
		{ LEGO_GROUP_D,  41,  LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_D,  42,  LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_D,  43,  LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B, 7 },
		{ LEGO_GROUP_D,  44,  LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_D,  45,  LEGO_MCP23017_CH(0), 4, MCP23017_PORT_B, 3 },
		/* Group E - Train Station #1 (60050) */
		{ LEGO_GROUP_E,  46,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 6 },
		{ LEGO_GROUP_E,  47,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 5 },
		{ LEGO_GROUP_E,  48,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 4 },
		{ LEGO_GROUP_E,  49,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 7 },
		/* Group F - Corner Garage (10264) */
		{ LEGO_GROUP_F,  50,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_F,  51,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_F,  52,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_F,  53,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_F,  54,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_F,  55,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_F,  56,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_F,  57,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_F,  58,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 2 },
		{ LEGO_GROUP_F,  59,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 4 },
		{ LEGO_GROUP_F,  60,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 5 },
		{ LEGO_GROUP_F,  61,  LEGO_MCP23017_CH(0), 5, MCP23017_PORT_A, 6 },
		/* Group G - Pet Shop #1 (10218) */
		{ LEGO_GROUP_G,  62,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_G,  63,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 7 },
		{ LEGO_GROUP_G,  64,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_G,  65,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_A, 1 },
		/* Group H - Pet Shop #2 (10218) */
		{ LEGO_GROUP_H,  66,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_H,  67,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_H,  68,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_H,  69,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_H,  70,  LEGO_MCP23017_CH(0), 6, MCP23017_PORT_B, 5 },
		/* Group J - Detective's Office (10246) */
		{ LEGO_GROUP_I,  71,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_I,  72,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_I,  73,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A, 2 },
		{ LEGO_GROUP_I,  74,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_I,  75,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_I,  76,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_I,  77,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A, 4 },
		{ LEGO_GROUP_I,  78,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_A, 5 },
		{ LEGO_GROUP_I,  79,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_I,  80,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_I,  81,  LEGO_MCP23017_CH(0), 7, MCP23017_PORT_B, 6 },
		/* Group J - Downtown dinner (10260) */
		{ LEGO_GROUP_J,  82,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_J,  83,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_J,  84,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 2 },
		{ LEGO_GROUP_J,  85,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_J,  86,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_J,  87,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_J,  88,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_J,  89,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_J,  90,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_A, 3 },
		{ LEGO_GROUP_J,  91,  LEGO_MCP23017_CH(1), 0, MCP23017_PORT_A, 4 },
		/* Group K - Park Street Townhouse #1 (31065) */
		{ LEGO_GROUP_K,  92,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 4 },
		{ LEGO_GROUP_K,  93,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 6 },
		{ LEGO_GROUP_K,  94,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 5 },
		/* Group L - Park Street Townhouse #2 (31065) */
		{ LEGO_GROUP_L,  95,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 2 },
		{ LEGO_GROUP_L,  96,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_L,  97,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_A, 0 },
		/* Group M - Train Station #2 (7997) */
		{ LEGO_GROUP_M,  98,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 0 },
		{ LEGO_GROUP_M,  99,  LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 5 },
		{ LEGO_GROUP_M,  100, LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 4 },
		{ LEGO_GROUP_M,  101, LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 6 },
		{ LEGO_GROUP_M,  102, LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 1 },
		{ LEGO_GROUP_M,  103, LEGO_MCP23017_CH(1), 1, MCP23017_PORT_B, 2 },
		/* Group N - Roller Coaster (10261) */
		{ LEGO_GROUP_N,  104, LEGO_MCP23017_CH(1), 2, MCP23017_PORT_A, 0 },
		{ LEGO_GROUP_N,  105, LEGO_MCP23017_CH(1), 2, MCP23017_PORT_A, 1 },
		{ LEGO_GROUP_N,  106, LEGO_MCP23017_CH(1), 2, MCP23017_PORT_A, 2 },
		{ LEGO_GROUP_N,  107, LEGO_MCP23017_CH(1), 2, MCP23017_PORT_B, 3 },
		{ LEGO_GROUP_N,  108, LEGO_MCP23017_CH(1), 2, MCP23017_PORT_B, 7 },
};

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool LEGO_PerformStartup(void)
{
	/* Set TCA9548A and MCP23017 send/receive I2C functions */
	TCA9548A_Init(BOARD_I2C_SendSingleReg, BOARD_I2C_ReadSingleReg);
	MCP23017_Init(BOARD_I2C_SendMultiReg, BOARD_I2C_ReadMultiReg);

	bool response = true;

	/* MCP23017 init */
	for (uint8_t i = 0; i < GUI_COUNTOF(mcp23017Devices); i++) {

		/* Select correct I2C channel */
		TCA9548A_Response_t tcaRes = TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, mcp23017Devices[i].channel);
		if (tcaRes != TCA9548A_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot choose TCA9548A channel (mask #%d). Response code %d", mcp23017Devices[i].channel, tcaRes));
			response = false;
			continue;
		}

		MCP23017_StatusCode_t mcpRes = MCP23017_ChainInit(&mcp23017Devices[i].chain, 0x00);
		if (mcpRes != MCP23017_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot initialize MCP23017 chain #%d. Status code %d", i, mcpRes));
			response = false;
		}

		/* By default all pins are output with logic high (lights off) */
		for (uint8_t j = 0; j < mcp23017Devices[i].chain.numDevices; j++) {
			if (MCP23017_PortSetDirection(&mcp23017Devices[i].chain, j, MCP23017_PORT_A, 0x00) != MCP23017_SUCCESS ||
				MCP23017_PortSetDirection(&mcp23017Devices[i].chain, j, MCP23017_PORT_B, 0x00) != MCP23017_SUCCESS) {
				response = false;
			}

			if (MCP23017_PortWrite(&mcp23017Devices[i].chain, j, MCP23017_PORT_A, 0xFF) != MCP23017_SUCCESS ||
				MCP23017_PortWrite(&mcp23017Devices[i].chain, j, MCP23017_PORT_B, 0xFF) != MCP23017_SUCCESS) {
				response = false;
			}
		}
	}

	return response;
}

const LEGO_Light_t *LEGO_GetLightById(int16_t id)
{
	/* GUI_COUNTOF defined in emWin library */
	for (uint16_t i = 0; i < GUI_COUNTOF(legoLights); i++) {
		if (legoLights[i].lightId == id) {
			return &legoLights[i];
		}
	}
	return NULL;
}

bool LEGO_LedControl(const LEGO_Light_t *light, LEGO_LightOp_t op)
{
	assert(light != NULL);

	/* Change multiplexer channel */
	TCA9548A_Response_t tcaRes = TCA9548A_SelectChannelsOptimized(TCA9548A_DEFAULT_ADDR, light->mcp23017Info->channel);
	if (tcaRes != TCA9548A_SUCCESS) {
		LOGGER_WRITELN(("Cannot select TCA9548A channel. I2C error. Status code %d", tcaRes));
		return false;
	}

	MCP23017_StatusCode_t mcpRes;

	/* Perform desired operation */
	switch (op) {
	case LEGO_LIGHT_ON:
		mcpRes = MCP23017_PinWrite(&light->mcp23017Info->chain, light->mcp23017DevNum, light->mcp23017Port, light->mcp23017Pin, MCP23017_PIN_LOW);
		break;
	case LEGO_LIGHT_OFF:
		mcpRes = MCP23017_PinWrite(&light->mcp23017Info->chain, light->mcp23017DevNum, light->mcp23017Port, light->mcp23017Pin, MCP23017_PIN_HIGH);
		break;
	case LEGO_LIGHT_TOGGLE:
		mcpRes = MCP23017_PinToggle(&light->mcp23017Info->chain, light->mcp23017DevNum, light->mcp23017Port, light->mcp23017Pin);
		break;
	}

	if (mcpRes != MCP23017_SUCCESS) {
		LOGGER_WRITELN(("MCP23017 pin writing error. Status code %d", mcpRes));
		return false;
	}

	return true;
}
