/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define IOCON_PIO_DIGITAL_EN 0x0100u    /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC0 0x00u           /*!<@brief Selects pin function 0 */
#define IOCON_PIO_FUNC1 0x01u           /*!<@brief Selects pin function 1 */
#define IOCON_PIO_FUNC2 0x02u   		/*!@brief Selects pin function 2 */
#define IOCON_PIO_FUNC6 0x06u           /*!<@brief Selects pin function 6 */
#define IOCON_PIO_FUNC7 0x07u           /*!<@brief Selects pin function 7 */
#define IOCON_PIO_I2CDRIVE_HIGH 0x0400u /*!<@brief High drive: 20 mA */
#define IOCON_PIO_I2CFILTER_EN 0x00u    /*!<@brief I2C 50 ns glitch filter enabled */
#define IOCON_PIO_I2CSLEW_I2C 0x00u     /*!<@brief I2C mode */
#define IOCON_PIO_INPFILT_OFF 0x0200u   /*!<@brief Input filter disabled */
#define IOCON_PIO_INV_DI 0x00u          /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u      /*!<@brief No addition pin function */
#define IOCON_PIO_MODE_PULLUP 0x20u     /*!<@brief Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u    /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_FAST 0x0400u     /*!<@brief Fast mode, slew rate control is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u   /*!<@brief Standard mode, output slew rate control is enabled */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port 2 */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port 2 */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port 2 */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port 2 */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port 2 */
#define PIN8_IDX                         8u   /*!< Pin number for pin 8 in a port 2 */
#define PIN9_IDX                         9u   /*!< Pin number for pin 9 in a port 2 */
#define PIN10_IDX                       10u   /*!< Pin number for pin 10 in a port 2 */
#define PIN15_IDX                       15u   /*!< Pin number for pin 15 in a port 3 */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port 0 */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port 0 */
#define PORT0_IDX                        0u   /*!< Port index */
#define PORT2_IDX                        2u   /*!< Port index */
#define PORT3_IDX                        3u   /*!< Port index */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

#define PIO013_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO013_FUNC_ALT1 0x01u        /*!<@brief Selects pin function.: Alternative connection 1. */
#define PIO014_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO014_FUNC_ALT1 0x01u        /*!<@brief Selects pin function.: Alternative connection 1. */
#define PIO331_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO331_FUNC_ALT2 0x02u        /*!<@brief Selects pin function.: Alternative connection 2. */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUserPins(void); /* Function assigned for the Cortex-M4F */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
