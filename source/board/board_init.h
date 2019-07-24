#ifndef BOARD_INIT_H_
#define BOARD_INIT_H_

#include "stdint.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

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
 * \brief 			Set LCD brightness
 * \param percent : PWM duty percent (range 1-99)
 */
void BOARD_SetBacklightPercent(uint8_t percent);

#endif /* BOARD_INIT_H_ */
