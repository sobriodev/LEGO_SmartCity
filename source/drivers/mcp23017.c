#include "mcp23017.h"
#include "assert.h"
#include "stdlib.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

#define MCP23017_I2C_HOOK(FN)		{if(i2cSendFn==NULL||i2cReceiveFn==NULL){return MCP23017_MISSING_FN;}}
#define MCP23017_SENDING_RES(RES)	{if(!(RES)){return MCP23017_SENDING_ERR;}return MCP23017_SUCCESS;}
#define MCP23017_RECEIVING_RES(RES)	{if(!(RES)){return MCP23017_RECEIVING_ERR;}return MCP23017_SUCCESS;}

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Send and receive functions pointers */
static MCP23017_SendFn_t i2cSendFn;
static MCP23017_ReceiveFn_t i2cReceiveFn;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

MCP23017_StatusCode_t MCP23017_SendGeneric(const MCP23017_Chain_t *chain, uint8_t devNum, uint8_t regAddr, uint8_t regData)
{
	/* Check preconditions */
	assert(chain);
	assert(devNum < MCP23017_MAX_DEVICES);
	assert(chain->chainBaseAddr >= MCP23017_BASE_ADDR && chain->chainBaseAddr < MCP23017_BASE_ADDR + MCP23017_MAX_DEVICES);
	MCP23017_I2C_HOOK();

	/* Send the data and return I2C status */
	bool res = i2cSendFn(chain->chainBaseAddr + devNum, regAddr, regData);
	MCP23017_SENDING_RES(res);
}

MCP23017_StatusCode_t MCP23017_ReceiveGeneric(const MCP23017_Chain_t *chain, uint8_t devNum, uint8_t regAddr, uint8_t *dst)
{
	/* Check preconditions */
	assert(chain);
	assert(devNum < MCP23017_MAX_DEVICES);
	assert(chain->chainBaseAddr >= MCP23017_BASE_ADDR && chain->chainBaseAddr < MCP23017_BASE_ADDR + MCP23017_MAX_DEVICES);
	assert(dst);
	MCP23017_I2C_HOOK();

	/* Send the data and return I2C status */
	bool res = i2cReceiveFn(chain->chainBaseAddr + devNum, regAddr, dst);
	MCP23017_RECEIVING_RES(res);
}

void MCP23017_Init(MCP23017_SendFn_t sendFn, MCP23017_ReceiveFn_t receiveFn)
{
	assert(sendFn);
	assert(receiveFn);

	i2cSendFn = sendFn;
	i2cReceiveFn = receiveFn;
}

MCP23017_StatusCode_t MCP23017_ChainInit(const MCP23017_Chain_t *chain, MCP23017_IOCON_t ioconf)
{
	/* Check preconditions */
	assert(chain->numDevices > 0 && chain->numDevices <= MCP23017_MAX_DEVICES);

#if defined(MCP23017_REG_BANK1) && MCP23017_REG_BANK1
	/* Make sure the BANK bit is set when using BANK1 registers map */
	MCP23017_UINT8_BIT_SET(ioconf, MCP23017_IOCON_BANK_POS);
#endif

	/* Send init frames */
	uint8_t ioconReg = 0x0A; /* When initializing IOCON address is always 0x0A no matter which BANK map is used */
	for (uint8_t i = 0; i < chain->numDevices; i++) {
		if (MCP23017_SendGeneric(chain, i, ioconReg, ioconf) != MCP23017_SUCCESS) {
			return MCP23017_SENDING_ERR;
		}
	}
	return MCP23017_SUCCESS;
}

MCP23017_StatusCode_t MCP23017_PinWrite(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin, bool value)
{
	/* Check preconditions */
	assert(pin < MCP23017_PORT_PINS);

	/* Get port status */
	uint8_t portStatus;
	MCP23017_StatusCode_t res = MCP23017_PortRead(chain, devNum, port, &portStatus);

	if (res != MCP23017_SUCCESS) {
		return MCP23017_RECEIVING_ERR;
	}

	/* Update and store port data */
	if (value) {
		MCP23017_UINT8_BIT_SET(portStatus, pin);
	} else {
		MCP23017_UINT8_BIT_CLEAR(portStatus, pin);
	}

	return MCP23017_SendGeneric(chain, devNum, port, portStatus);
}

MCP23017_StatusCode_t MCP23017_PinRead(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin, bool *dst)
{
	/* Check preconditions */
	assert(pin < MCP23017_PORT_PINS);

	/* Get port status */
	uint8_t portStatus;
	MCP23017_StatusCode_t res = MCP23017_ReceiveGeneric(chain, devNum, port, &portStatus);

	if (res != MCP23017_SUCCESS) {
		return MCP23017_RECEIVING_ERR;
	}

	/* Get desired bit */
	*dst = (bool)MCP23017_UINT8_GET_BIT(portStatus, pin);
	return MCP23017_SUCCESS;
}

MCP23017_StatusCode_t MCP23017_PinToggle(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin)
{
	/* Check preconditions */
	assert(pin < MCP23017_PORT_PINS);

	/* Get port status */
	uint8_t portStatus;
	MCP23017_StatusCode_t res = MCP23017_ReceiveGeneric(chain, devNum, port, &portStatus);

	if (res != MCP23017_SUCCESS) {
		return MCP23017_RECEIVING_ERR;
	}

	/* Toggle bit */
	MCP23017_UINT8_TOGGLE_BIT(portStatus, pin);
	return MCP23017_SendGeneric(chain, devNum, port, portStatus);
}

MCP23017_StatusCode_t MCP23017_PortClearInt(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, bool intCap, uint8_t *dst)
{
	if (intCap) {
		return MCP23017_ReceiveGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_INTCAPA, MCP23017_INTCAPB), dst);
	}
	return MCP23017_ReceiveGeneric(chain, devNum, port, dst);
}
