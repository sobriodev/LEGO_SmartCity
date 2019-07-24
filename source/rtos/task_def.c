#include "task_def.h"
#include "stdbool.h"
#include "board.h"
#include "fsl_debug_console.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void vTaskGUI(void *pvParameters)
{
	/* Variable indicating touch event presence */
	bool touchEvent;

    while (1) {
    	touchEvent = BOARD_Touch_Poll();

    	if (touchEvent) {
    		/* Touch event occurred */
    		PRINTF("New touch occurred\r\n");
    	}

    	/* emWin execute */
        GUI_MULTIBUF_Begin();
        WM_Exec();
        GUI_MULTIBUF_End();
    }
}

void vTaskBacklight(void *pvParameters)
{
	while (1) {}
}
