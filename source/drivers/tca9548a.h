#ifndef DRIVERS_TCA9548A_H_
#define DRIVERS_TCA9548A_H_

#include "stdint.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Bit operations */
#define TCA9548A_UINT8_BIT(BIT)				((uint8_t)(1 << (BIT)))

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA_TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Send function header
 *
 * \param devAddr : Device address
 * \param sendData : Register data
 * \return True if transmission succeed, false otherwise
 */
typedef bool (*TCA9548A_SendFn_t)(uint8_t devAddr, uint8_t sendData);

/*!
 * \brief Receive function header
 *
 * \param devAddr : Device address
 * \param readBuff : Buffer where register data will be stored
 * \return True if transmission succeed, false otherwise
 */
typedef bool (*TCA9548A_ReceiveFn_t)(uint8_t devAddr, uint8_t *readBuff);

/*!
 * \brief Return codes when using API functions
 */
typedef enum {
	TCA9548A_FN_ERR,			//!< Send/Receive function not assigned
	TCA9548A_SUCCESS,			//!< Data successfully written(read) to the control register
	TCA9548A_I2C_SENDING_ERR, 	//!< I2C sending error (typically device not found or NAK was returned)
	TCA9548A_I2C_RECEIVING_ERR	//!< I2C receiving error
} TCA9548A_Response_t;

/*!
 * \brief Control register channel bits
 */
typedef enum {
	TCA9548A_CHANNEL0 = TCA9548A_UINT8_BIT(0x00),	//!< Channel 0
	TCA9548A_CHANNEL1 = TCA9548A_UINT8_BIT(0x01),	//!< Channel 1
	TCA9548A_CHANNEL2 = TCA9548A_UINT8_BIT(0x02),	//!< Channel 2
	TCA9548A_CHANNEL3 = TCA9548A_UINT8_BIT(0x03),	//!< Channel 3
	TCA9548A_CHANNEL4 = TCA9548A_UINT8_BIT(0x04),	//!< Channel 4
	TCA9548A_CHANNEL5 = TCA9548A_UINT8_BIT(0x05),	//!< Channel 5
	TCA9548A_CHANNEL6 = TCA9548A_UINT8_BIT(0x06),	//!< Channel 6
	TCA9548A_CHANNEL7 = TCA9548A_UINT8_BIT(0x07),	//!< Channel 7
} TCA9548A_Channel_t;

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PUBLIC FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Init TCA9548A I2C functions
 *
 * \param sendFn : Send function base address
 * \param receiveFn : Receive function base address
 */
void TCA9548A_Init(TCA9548A_SendFn_t sendFn, TCA9548A_ReceiveFn_t receiveFn);

/*!
 * \brief Select active channels
 *
 * \param devAddr : I2C device address
 * \param channels : Bit 0 reflects the state of channel 0, ..., bit 7 reflects the state of channel 7. Bit set = channel selected, bit cleared = channel deselected
 * \return Instance of TCA9548A_Response_t
 */
TCA9548A_Response_t TCA9548A_SelectChannels(uint8_t devAddr, uint8_t channels);

/*!
 * \brief Read channels states
 *
 * \param devAddr : I2C device address
 * \param dst : Channels output buffer
 * \return Instance of TCA9548A_Response_t
 */
TCA9548A_Response_t TCA9548A_ReadChannelsStates(uint8_t devAddr, uint8_t *dst);

#endif /* DRIVERS_TCA9548A_H_ */
