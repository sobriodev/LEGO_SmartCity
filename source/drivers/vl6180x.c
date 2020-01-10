#include "vl6180x.h"
#include "assert.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

#define VL6180X_EXPECT_SUCCESS_B(VAL, RES)	{if(!(VAL)) {return (RES);}}
#define VL6180X_SEND_HOOK_B(VAL)			VL6180X_EXPECT_SUCCESS_B((VAL), VL6180X_I2C_SENDING_ERR);
#define VL6180X_RECEIVE_HOOK_B(VAL)			VL6180X_EXPECT_SUCCESS_B((VAL), VL6180X_I2C_RECEIVING_ERR);

#define VL6180X_EXPECT_SUCCESS(VAL, RES)	{if((VAL)!=VL6180X_SUCCESS) {return (RES);}}
#define VL6180X_SEND_HOOK(VAL)				VL6180X_EXPECT_SUCCESS((VAL), VL6180X_I2C_SENDING_ERR);
#define VL6180X_RECEIVE_HOOK(VAL)			VL6180X_EXPECT_SUCCESS((VAL), VL6180X_I2C_RECEIVING_ERR);

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

static VL6180X_SendFn_t i2cSendFn;
static VL6180X_ReceiveFn_t i2cReceiveFn;
static VL6180X_CEFn_t chipEnableFn;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Helper for writing single byte */
static inline VL6180X_Response_t VL6180X_WriteByte(uint8_t devAddr, uint16_t regAddr, uint8_t data)
{
	return i2cSendFn(devAddr, regAddr, &data, 1);
}

static VL6180X_Response_t VL6180X_WriteRecommendedSettings(void)
{
	/* Mandatory - private registers */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0207, 0x01));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0208, 0x01));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0096, 0x00));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0097, 0xFD));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00E3, 0x00));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00E4, 0x04));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00E5, 0x02));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00E6, 0x01));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00E7, 0x03));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00F5, 0x02));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00D9, 0x05));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00DB, 0xCE));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00DC, 0x03));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00DD, 0xF8));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x009F, 0x00));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00A3, 0x3C));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00B7, 0x00));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00BB, 0x3C));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00B2, 0x09));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00CA, 0x09));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0198, 0x01));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x01B0, 0x17));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x01AD, 0x00));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x00FF, 0x05));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0100, 0x05));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0199, 0x05));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x01A6, 0x1B));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x01AC, 0x3E));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x01A7, 0x1F));
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0030, 0x00));

	/* Recommended */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0011, 0x10)); /* GPIO1 as active-low interrupt pin */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x010A, 0x30)); /* Set the averaging sample period(compromise between lower noise and increased execution time) */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x003F, 0x46)); /* Sets the light and dark gain (uppernibble). Dark gain should not be changed */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0031, 0xFF)); /* Sets the # of range measurements after which auto calibration of system is performed */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0040, 0x63)); /* Set ALS integration time to 100ms */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x002E, 0x01)); /* Perform a single temperature calibration of the ranging sensor */

	/* Optional */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x001B, VL6180_X_RANGE_MEAS_PERIOD)); /* Set default ranging inter-measurement period to 100ms */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x003E, 0x31)); /* Set default ALS inter-measurement period to 500ms */
	VL6180X_SEND_HOOK_B(VL6180X_WriteByte(VL6180X_I2C_ADDR, 0x0014, 0x24)); /* Configures interrupt on New Sample Ready threshold event */

	return VL6180X_SUCCESS;
}

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PUBLIC FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

void VL6180X_Init(VL6180X_SendFn_t sendFn, VL6180X_ReceiveFn_t receiveFn, VL6180X_CEFn_t ceFn)
{
	assert(sendFn);
	assert(receiveFn);
	assert(ceFn);

	i2cSendFn = sendFn;
	i2cReceiveFn = receiveFn;
	chipEnableFn = ceFn;
}

VL6180X_Response_t VL6180X_SendGeneric(const VL6180X_Devices_t *dev, uint8_t devNum, uint16_t regAddr, uint8_t *regData, uint8_t len)
{
	assert(dev);
	assert(dev->numDevices > 0 && dev->numDevices < VL6180X_NUM_DEV);
	assert(devNum < VL6180X_NUM_DEV);

	bool res = i2cSendFn(dev->baseAddr + devNum, regAddr, regData, len);
	if (!res) {
		return VL6180X_I2C_SENDING_ERR;
	}
	return VL6180X_SUCCESS;
}

VL6180X_Response_t VL6180X_ReceiveGeneric(const VL6180X_Devices_t *dev, uint8_t devNum, uint16_t regAddr, uint8_t *regData, uint8_t len)
{
	assert(dev);
	assert(dev->numDevices > 0 && dev->numDevices < VL6180X_NUM_DEV);
	assert(devNum < VL6180X_NUM_DEV);

	bool res = i2cReceiveFn(dev->baseAddr + devNum, regAddr, regData, len);
	if (!res) {
		return VL6180X_I2C_RECEIVING_ERR;
	}
	return VL6180X_SUCCESS;
}

VL6180X_Response_t VL6180X_DevInit(const VL6180X_Devices_t *dev)
{
	assert(dev->numDevices > 0 && dev->numDevices < VL6180X_NUM_DEV);

	/* At this point all devices have the same I2C address */
	for (uint8_t i = 0; i < dev->numDevices; i++) {
		/* Choose correct device, check response and initialize sensor settings */
		if (!chipEnableFn(dev, i)) {
			return VL6180X_CE_ERROR;
		}

		/* Wait some time according to the VL6180X user manual */
		for (uint32_t i = 0; i < 10000; i++) {
			__asm("NOP");
		}

		/* Check if reset flag is set */
		uint8_t freshOutReset;
		VL6180X_RECEIVE_HOOK_B(i2cReceiveFn(VL6180X_I2C_ADDR, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, &freshOutReset, 1));

		if (freshOutReset != 0x01) {
			/* Device not responded */
			return VL6180X_DEVICE_NOT_RESPONDING;
		}

		/* Write recommended settings */
		VL6180X_WriteRecommendedSettings();
		/* Change I2C address */
		uint8_t i2cAddr = dev->baseAddr + i;
		VL6180X_SEND_HOOK_B(i2cSendFn(VL6180X_I2C_ADDR, VL6180X_I2C_SLAVE_DEVICE_ADDRESS, &i2cAddr, 1));
		/* Clear reset bit using new i2c address */
		uint8_t regData = 0x00;
		VL6180X_SEND_HOOK(VL6180X_SendGeneric(dev, i, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, &regData, 1));
	}

	return VL6180X_SUCCESS;
}

VL6180X_Response_t VL6180X_StartRangeMesurements(const VL6180X_Devices_t *dev, uint8_t devNum)
{
	uint8_t status;
	VL6180X_ReceiveGeneric(dev, devNum, VL6180X_RESULT_RANGE_STATUS, &status, 1);

	if (!(status & 0x01)) {
		return VL6180X_DEV_BUSY;
	}

	uint8_t send = 0x03;
	return VL6180X_SendGeneric(dev, devNum, VL6180X_SYSRANGE_START, &send, 1);
}

VL6180X_Response_t VL6180X_ClearIntStatus(const VL6180X_Devices_t *dev, uint8_t devNum)
{
	uint8_t send = 0x07;
	return VL6180X_SendGeneric(dev, devNum, VL6180X_SYSTEM_INTERRUPT_CLEAR, &send, 1);
}
