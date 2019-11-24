#include "lego.h"
#include "board_conf.h"
#include "logger.h"

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
/* ---------------------------- PRIVATE DATA TYPES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Structure which joins TCA9548A channel with MCP23017 devices */
typedef struct {
	TCA9548A_Channel_t channel;
	MCP23017_Chain_t chain;
} LEGO_MCP23017Info_t;

/* ----------------------------------------------------------------------------- */
/* ---------------------------- PRIVATE VARIABLES ------------------------------ */
/* ----------------------------------------------------------------------------- */

static const LEGO_MCP23017Info_t mcp23017Devices[LEGO_MCP23017_CHAINS] = {
		{ TCA9548A_CHANNEL0, { LEGO_MCP23017_CHAIN0, LEGO_MCP23017_CHAIN0_DEV, MCP23017_BASE_ADDR } },
		{ TCA9548A_CHANNEL1, { LEGO_MCP23017_CHAIN1, LEGO_MCP23017_CHAIN1_DEV, MCP23017_BASE_ADDR } }
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
	/* MCP23017 Init */
	for (uint8_t i = 0; i < LEGO_MCP23017_CHAINS; i++) {
		/* Select correct I2C channel */
		TCA9548A_Response_t tcaRes = TCA9548A_SelectChannels(TCA9548A_DEFAULT_ADDR, mcp23017Devices[i].channel);

		if (tcaRes != TCA9548A_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot choose TCA9548A channel #%d. Response code %d", mcp23017Devices[i].channel - 1, tcaRes));
			response = false;
			continue;
		}

		MCP23017_StatusCode_t mcpRes = MCP23017_ChainInit(&mcp23017Devices[i].chain, 0x00);

		if (mcpRes != MCP23017_SUCCESS) {
			LOGGER_WRITELN(("LEGO startup problem. Cannot initialize MCP23017 chain #%d. Status code %d", i, mcpRes));
			response = false;
		}

	}

	return response;
}
