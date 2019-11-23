#ifndef BOARD_INIT_H_
#define BOARD_INIT_H_

#include "stdint.h"
#include "lcd.h"
#include "stdbool.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

#define BOARD_TASK_STARTUP_NAME		"TASK-STARTUP"
#define BOARD_TASK_STARTUP_STACK	0x2710
#define BOARD_TASK_STARTUP_PRIO		(configMAX_PRIORITIES - 2)

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Possible return values when getting single touch from touch panel
 */
typedef enum {
	TOUCH_PANEL_STATUS_ERROR = -1,	//!< An error occurred and touch event cannot be processed
	TOUCH_PANEL_STATUS_OK         	//!< A touch event was successfully stored
} BOARD_TouchPanelStatus_t;

/*!
 * \brief Structure containing response from touch panel controller
 */
typedef struct {
	BOARD_TouchPanelStatus_t touchPanelStatus;	//!< See BOARD_TouchPanelStatus_t for more information
	uint32_t touchEvent;						//!< If 1 new touch event was generated, 0 otherwise
	LCD_State_t eventLCDState;					//!< LCD state when touch event was generated. See LCD_State_t for more information
} BOARD_TouchInfo_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Configure LCD backlight PWM
 */
void BOARD_InitBacklightPWM(void);

/*!
 * \brief Initialize all needed clocks and peripherals during startup
 */
void BOARD_Init(void);

/*!
 * \brief Set LCD brightness
 *
 * \param percent : PWM duty percent (range 1-99)
 */
void BOARD_SetBacklightPercent(uint8_t percent);

/*!
 * \brief Poll the touch controller for new events
 *
 * \param touchInfo : Base address of BOARD_TouchInfo_t struct
 */
void BOARD_TouchEvent(BOARD_TouchInfo_t *touchInfo);

/*!
 * \brief The function creates startup task which create and configure all RTOS routines
 *
 * \return True if RTOS initialized successfully, false otherwise
 */
bool BOARD_RTOSInit(void);

/*!
 * \brief Reset board
 */
void BOARD_SystemReset(void);

/*!
 * \brief Function for sending I2C frames when slave device has only one byte-register
 *
 * \param devAddr : Device address
 * \param txBuff : Transmit byte
 * \return True if slave device acknowledged the data, false otherwise
 */
bool BOARD_I2C_SendSingleReg(uint8_t devAddr, uint8_t txBuff);

/*!
 * \brief Function for receiving I2C frames when slave device has only one byte-register
 *
 * \param devAddr : Device address
 * \param rxBuff : Receive byte base address
 * \return True if slave data was successfully read, false otherwise
 */
bool BOARD_I2C_ReadSingleReg(uint8_t devAddr, uint8_t *rxBuff);

/*!
 * \brief Function for sending I2C frames when slave device has multiple byte-registers
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param txBuff : Transmit byte
 * \return True if slave device acknowledged the data, false otherwise
 */
bool BOARD_I2C_SendMultiReg(uint8_t devAddr, uint8_t regAddr, uint8_t txBuff);

/*!
 * \brief Function for receiving I2C frames when slave device has multiple byte-registers
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param RxBuff : Receive byte base address
 * \return True if slave data was successfully read, false otherwise
 */
bool BOARD_I2C_ReadMultiReg(uint8_t devAddr, uint8_t regAddr, uint8_t *rxBuff);

/*!
 * \brief Function for sending I2C frames when slave device has multiple multibyte-registers. It supports two-byte device address
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param txBuff : Transmit buffer base address
 * \param buffLen : Transmit buffer length
 * \return True if slave device acknowledged the data, false otherwise
 */
bool BOARD_I2C_SendMultiByte(uint8_t devAddr, uint16_t regAddr, uint8_t *txBuff, uint8_t buffLen);

/*!
 * \brief Function for receiving I2C frames when slave device has multiple multibyte-registers. It supports two-byte device address
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param rxBuff : Receive buffer base address
 * \param bytesRead : Number of bytes read from slave device
 * \return True if slave data was successfully read, false otherwise
 */
bool BOARD_I2C_ReadMultByte(uint8_t devAddr, uint16_t regAddr, uint8_t *rxBuff, uint8_t bytesRead);

#endif /* BOARD_INIT_H_ */
