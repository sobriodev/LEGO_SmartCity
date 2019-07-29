#ifndef LCD_H_
#define LCD_H_

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* LCD brightness */
#define LCD_MAX_BRIGHTNESS			99
#define LCD_MIN_BRIGHTNESS			1
#define LCD_ON_BRIGHTNESS			LCD_MAX_BRIGHTNESS
#define LCD_DIMMING_BRIGHTNESS  	30
#define LCD_OFF_BRIGHTNESS			LCD_MIN_BRIGHTNESS

/* LCD timing */
#define LCD_FROM_ON_TO_DIMMING_MS	5000
#define LCD_FROM_DIMMING_TO_OFF_MS	2000

/* This is only alias for setting backlight PWM duty */
#define LCD_SET_BRIGHNTESS(DUTY)	BOARD_SetBacklightPercent((DUTY))

/* ----------------------------------------------------------------------------- */
/* --------------------------------- DATA TYPES -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief LCD state
 */
typedef enum {
	LCD_ON,     	//!< LCD is on. All GUI operations are enabled
	LCD_DIMMING,	//!< LCD is dimming and will be turned off shortly
	LCD_OFF     	//!< LCD is off and it is waiting for touch event to weak up
} LCDState_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Get current LCD state
 *
 * \return Instance of LCDState_t enum
 */
LCDState_t LCD_GetState(void);

/*!
 * \brief Set new LCD state
 *
 * \param state : LCD state to be set. See LCDState_t enum
 */
void LCD_SetState(LCDState_t state);

#endif /* LCD_H_ */
