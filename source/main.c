#include "board_conf.h"
#include "logger.h"
#include "gui_conf.h"

/* FreeRTOS */
#include "FreeRTOS.h"

int main(void)
{
	/* Board initialization */
	BOARD_Init();
    BOARD_InitBacklightPWM();

    /* Create all needed tasks. Http server task is created internally inside lwIP files */
    if (!BOARD_RTOSInit()) {
    	LOGGER_WRITELN(("RTOS startup error. Check heap and stack usage"));
    }

    /* emWin config */
    GUI_Start();

    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never reach this line */
    while (1) {}
}
