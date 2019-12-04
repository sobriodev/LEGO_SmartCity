#include "tca9548a.h"
#include "assert.h"
#include "stdlib.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

#define TCA9548A_I2C_HOOK()				{if(i2cSendFn==NULL||i2cReceiveFn==NULL){return TCA9548A_FN_ERR;}}
#define TCA9548A_I2C_SENDING_RES(RES)	{if(!(RES)){return TCA9548A_I2C_SENDING_ERR;}return TCA9548A_SUCCESS;}
#define TCA9548A_I2C_RECEIVING_RES(RES)	{if(!(RES)){return TCA9548A_I2C_RECEIVING_ERR;}return TCA9548A_SUCCESS;}

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Active channels */
static uint8_t activeChannels;

/* Send and receive functions pointers */
static TCA9548A_SendFn_t i2cSendFn;
static TCA9548A_ReceiveFn_t i2cReceiveFn;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void TCA9548A_Init(TCA9548A_SendFn_t sendFn, TCA9548A_ReceiveFn_t receiveFn)
{
	assert(sendFn);
	assert(receiveFn);

	i2cSendFn = sendFn;
	i2cReceiveFn = receiveFn;

	/* By default no channel is used */
	activeChannels = TCA9548A_CHANNEL_NONE;
}

TCA9548A_Response_t TCA9548A_SelectChannels(uint8_t devAddr, uint8_t channels)
{
	TCA9548A_I2C_HOOK();

	bool res = i2cSendFn(devAddr, channels);
	TCA9548A_I2C_SENDING_RES(res);
}

TCA9548A_Response_t TCA9548A_SelectChannelsOptimized(uint8_t devAddr, uint8_t channels)
{
	/* If active channels are the same as new ones do nothing */
	if (channels == activeChannels) {
		return TCA9548A_SUCCESS;
	}

	TCA9548A_Response_t res = TCA9548A_SelectChannels(devAddr, channels);

	if (res == TCA9548A_SUCCESS) {
		activeChannels = channels;
	}
	return res;
}

TCA9548A_Response_t TCA9548A_ReadChannelsStates(uint8_t devAddr, uint8_t *dst)
{
	TCA9548A_I2C_HOOK();
	bool res = i2cReceiveFn(devAddr, dst);
	TCA9548A_I2C_RECEIVING_RES(res);
}