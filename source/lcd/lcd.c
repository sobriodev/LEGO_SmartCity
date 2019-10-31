#include "lcd.h"
#include "board_conf.h"
#include "settings.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

static LCD_State_t lcdState = LCD_ON; /* LCD active on startup */

/* ----------------------------------------------------------------------------- */
/* ------------------------------- API FUNCTIONS ------------------------------- */
/* ----------------------------------------------------------------------------- */

LCD_State_t LCD_GetState(void)
{
	return lcdState;
}

void LCD_SetState(LCD_State_t state)
{
	lcdState = state;

	uint8_t newPWMPercent;

	/* Check current state and update timer PWM percent */
	switch (state) {
	case LCD_DIMMING:
		newPWMPercent = LCD_DIMMING_BRIGHTNESS;
		break;
	case LCD_OFF:
		newPWMPercent = LCD_OFF_BRIGHTNESS;
		break;
	case LCD_ON:
		newPWMPercent = SETTINGS_GetInstance()->lcdBrightness;
		break;
	default:
		break;
	}

	LCD_SET_BRIGHNTESS(newPWMPercent);
}
