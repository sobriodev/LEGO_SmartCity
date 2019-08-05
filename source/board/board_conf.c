#include "board_conf.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_sctimer.h"

/* Touch panel */
#include "fsl_i2c.h"
#include "fsl_ft5406.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "GUIDRV_Lin.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

static uint32_t backlightPWMEvent;

/* Touch driver handle. */
extern ft5406_handle_t touch_handle;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void BOARD_InitBacklightPWM(void)
{
    sctimer_config_t config;
    sctimer_pwm_signal_param_t pwmParam;

    CLOCK_AttachClk(kMAIN_CLK_to_SCT_CLK);
    CLOCK_SetClkDiv(kCLOCK_DivSctClk, 2, true);
    SCTIMER_GetDefaultConfig(&config);
    SCTIMER_Init(SCT0, &config);

    pwmParam.output = kSCTIMER_Out_5;
    pwmParam.level = kSCTIMER_HighTrue;
    pwmParam.dutyCyclePercent = LCD_ON_BRIGHTNESS; /* Default max brightness */

    SCTIMER_SetupPwm(SCT0, &pwmParam, kSCTIMER_CenterAlignedPwm, 1000U, CLOCK_GetFreq(kCLOCK_Sct), &backlightPWMEvent);
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_L);
}


void BOARD_Init(void)
{
	/* Debug console clock */
	CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

	/* Route main clock to LCD. */
	CLOCK_AttachClk(kMAIN_CLK_to_LCD_CLK);

	/* Attach 12 MHz clock to FLEXCOMM2 (I2C master for touch controller) */
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

	CLOCK_EnableClock(kCLOCK_Gpio2);
	CLOCK_SetClkDiv(kCLOCK_DivLcdClk, 1, true);

	BOARD_InitBootPins();
	BOARD_BootClockPLL220M();
	BOARD_InitDebugConsole();
	BOARD_InitSDRAM();
}

void BOARD_SetBacklightPercent(uint8_t percent)
{
	/* Must be from 1 (LCD off) to 99 (LCD max brightness) */
	assert(percent > 0 && percent < 100);
	SCTIMER_UpdatePwmDutycycle(SCT0, 5, percent, backlightPWMEvent);
}

void BOARD_TouchEvent(TouchInfo_t *touchInfo)
{
    touch_event_t touchEvent;
    int touchX;
    int touchY;
    GUI_PID_STATE pidState;
    int newEvent;

    touchInfo->touchEvent = 0;
    touchInfo->eventLCDState = LCD_GetState();

    if (kStatus_Success != FT5406_GetSingleTouch(&touch_handle, &touchEvent, &touchX, &touchY, &newEvent)) {
    	touchInfo->touchPanelStatus = TOUCH_PANEL_STATUS_ERROR;
        touchInfo->touchEvent = 0;
    }
    else if (touchEvent != kTouch_Reserved) {
        pidState.x = touchY;
        pidState.y = touchX;
        pidState.Pressed = ((touchEvent == kTouch_Down) || (touchEvent == kTouch_Contact));
        pidState.Layer = 0;

        /* Reset X and Y coordinates when touch screen was released */
        if (!pidState.Pressed) {
        	pidState.x = -1;
        	pidState.y = -1;
        }

        /* New touch event is available */
    	touchInfo->touchPanelStatus = TOUCH_PANEL_STATUS_OK;
        touchInfo->touchEvent = newEvent;

        /* Store pid state only if LCD is active */
        if (touchInfo->eventLCDState == LCD_ON) {
        	GUI_TOUCH_StoreStateEx(&pidState);
        }
    }
}
