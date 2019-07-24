#include <lcd.h>

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

static LCDState_t LCDState;

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
}
