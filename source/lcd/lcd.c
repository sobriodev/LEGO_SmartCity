#include "lcd.h"
#include "board_conf.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

static LCDState_t LCDState = LCD_ON; /* LCD active on startup */

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

LCDState_t LCD_GetState(void)
{
	return LCDState;
}

void LCD_SetState(LCDState_t state)
{
	LCDState = state;

	uint8_t newPWMPercent;

	/* Check current state and update timer PWM percent */
	switch (state) {
	case LCD_DIMMING:
		newPWMPercent = LCD_DIMMING_BRIGHTNESS;
		break;
	case LCD_OFF:
		newPWMPercent = LCD_MIN_BRIGHTNESS;
		break;
	case LCD_ON:
		newPWMPercent = LCD_MAX_BRIGHTNESS;
		break;
	default:
		break;
	}

	LCD_SET_BRIGHNTESS(newPWMPercent);
}
