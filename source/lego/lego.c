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
#define LEGO_MCP23017_CHAINS		2
#define LEGO_MCP23017_CHAIN0		0x00
#define LEGO_MCP23017_CHAIN0_DEV	8
#define LEGO_MCP23017_CHAIN1		0x01
#define LEGO_MCP23017_CHAIN1_DEV	2

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE VARIABLES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* MCP23017 devices */
static const LEGO_MCP23017Info_t mcp23017Devices[LEGO_MCP23017_CHAINS] = {
		{ TCA9548A_CHANNEL0, { LEGO_MCP23017_CHAIN0, LEGO_MCP23017_CHAIN0_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL1, { LEGO_MCP23017_CHAIN1, LEGO_MCP23017_CHAIN1_DEV, MCP23017_BASE_ADDR } }
};

/* LEGO lights */
static const LEGO_Light_t legoLights[] = {
		{ 0, &mcp23017Devices[LEGO_MCP23017_CHAIN0], 0, MCP23017_PORT_A, 0 },
		{ 1, &mcp23017Devices[LEGO_MCP23017_CHAIN0], 0, MCP23017_PORT_A, 2 },
		{ 2, &mcp23017Devices[LEGO_MCP23017_CHAIN0], 0, MCP23017_PORT_B, 2 },
		{ 3, &mcp23017Devices[LEGO_MCP23017_CHAIN1], 1, MCP23017_PORT_B, 6 },
		{ 4, &mcp23017Devices[LEGO_MCP23017_CHAIN1], 1, MCP23017_PORT_B, 7 }
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
	for (uint8_t i = 0; i < LEGO_MCP23017_CHAINS; i++) {

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
