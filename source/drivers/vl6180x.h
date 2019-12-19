#ifndef DRIVERS_VL6180X_H_
#define DRIVERS_VL6180X_H_

#include "stdbool.h"
#include "stdint.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* I2C slave address */
#define VL6180X_I2C_ADDR 	0x29
/* Maximum devices number used */
#define VL6180X_NUM_DEV		16

/* Registers used */
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET 		0x0016
#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS  		0x0212
#define VL6180X_RESULT_RANGE_STATUS				0x004D
#define VL6180X_SYSRANGE_START					0x0018
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO	0x004F
#define VL6180X_RESULT_RANGE_VAL				0x0062
#define VL6180X_SYSTEM_INTERRUPT_CLEAR			0x0015

/* Range measurement period */
#define VL6180_X_RANGE_MEAS_PERIOD				0xC8	/* 2000 ms */

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Devices info
 */
typedef struct {
	uint8_t numDevices; //!< The number of devices used
	uint8_t baseAddr;	//!< First device I2C address
} VL6180X_Devices_t;

/*!
 * \brief I2C send function
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param txBuff : Transmit buffer base address
 * \param len : The number of bytes written
 * \return : True if transmission succeed, false otherwise
 */
typedef bool (*VL6180X_SendFn_t)(uint8_t devAddr, uint16_t regAddr, uint8_t *txBuff, uint8_t len);

/*!
 * \brief I2C receive function
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param rxBuff : Receive buffer base address
 * \param len : The number of bytes read
 * \return True if transmission succeed, false otherwise
 */
typedef bool (*VL6180X_ReceiveFn_t)(uint8_t devAddr, uint16_t regAddr, uint8_t *rxBuff, uint8_t len);

/*!
 * \brief Chip enable function. It has to drive CE pins HIGH or LOW depending on device number
 *
 * \param dev : Devices info base address
 * \param devNum : The number of device in the chain
 * \return True if pins logic states was successfully changed, false otherwise
 */
typedef bool (*VL6180X_CEFn_t)(const VL6180X_Devices_t *dev, uint8_t devNum);

/*!
 * \brief Return codes
 */
typedef enum {
	VL6180X_SUCCESS,          		//!< Everything OK
	VL6180X_FN_ERR,           		//!< Send/Receive/CE functions not assigned
	VL6180X_I2C_SENDING_ERR,  		//!< I2C sending error (device not found or NAK typically)
	VL6180X_I2C_RECEIVING_ERR,		//!< I2C receiving error
	VL6180X_DEVICE_NOT_RESPONDING,	//!< Device not responding
	VL6180X_CE_ERROR,				//!< CE pin cannot be updated
	VL6180X_DEV_BUSY				//!< Device busy
} VL6180X_Response_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PUBLIC FUNCTIONS ----------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Init library by assigning needed functions
 *
 * \param sendFn : I2C send function
 * \param receiveFn : I2C receive function
 * \param ceFn : CE function
 */
void VL6180X_Init(VL6180X_SendFn_t sendFn, VL6180X_ReceiveFn_t receiveFn, VL6180X_CEFn_t ceFn);

/*!
 * \brief Generic send function. It can be used but it is better to use dedicated API
 *
 * \param dev : Devices info
 * \param devNum : The number of device in the chain
 * \param regAddr : Register address
 * \param regData : Register data
 * \param len : Data length
 * \return Instance of VL6180X_Response_t
 */
VL6180X_Response_t VL6180X_SendGeneric(const VL6180X_Devices_t *dev, uint8_t devNum, uint16_t regAddr, uint8_t *regData, uint8_t len);

/*!
 * \brief Generic send function. It can be used but it is better to use dedicated API
 *
 * \param dev : Devices info
 * \param devNum : The number of device in the chain
 * \param regAddr : Register address
 * \param otpData : Register response data
 * \param len : Data length
 * \return Instance of VL6180X_Response_t
 */
VL6180X_Response_t VL6180X_ReceiveGeneric(const VL6180X_Devices_t *dev, uint8_t devNum, uint16_t regAddr, uint8_t *otpData, uint8_t len);

/*!
 * \brief Initialize devices
 *
 * \param dev : Devices info
 * \return Instance of VL6180X_Response_t
 */
VL6180X_Response_t VL6180X_DevInit(const VL6180X_Devices_t *dev);

/*!
 * \brief Start range measurements
 *
 * \param dev : Devices info
 * \param devNum : The number of device in the chain
 * \return Instance of VL6180X_Response_t
 */
VL6180X_Response_t VL6180X_StartRangeMesurements(const VL6180X_Devices_t *dev, uint8_t devNum);

/*!
 * \brief Clear interrupt status
 *
 * \param dev : Devices info
 * \param devNum : The number of device in the chain
 * \return Instance of VL6180X_Response_t
 */
VL6180X_Response_t VL6180X_ClearIntStatus(const VL6180X_Devices_t *dev, uint8_t devNum);

/*!
 * \brief Read range measurement
 *
 * \param dev : Devices info
 * \param devNum : The number of device in the chain
 * \param range : Range output buffer
 * \return Instance of VL6180X_Response_t
 */
static inline VL6180X_Response_t VL6180X_ReadRange(const VL6180X_Devices_t *dev, uint8_t devNum, uint8_t *range)
{
	return VL6180X_ReceiveGeneric(dev, devNum, VL6180X_RESULT_RANGE_VAL, range, 1);
}

#endif /* DRIVERS_VL6180X_H_ */
