#include "board_conf.h"
#include "gui_conf.h"
#include "rtos.h"
#include "logger.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Httpsrv */
#include "httpsrv_conf.h"

int main(void)
{
	/* Board initialization */
	BOARD_Init();

    /* Set the backlight PWM. */
    BOARD_InitBacklightPWM();

    /* Start emWin library */
    GUI_Start();

    /* Start Http server */
    HTTPSRV_Init();

    if (!RTOS_TasksCreate() || !RTOS_TimersCreate()) {
    	LOGGER_WRITELN(("FreeRTOS tasks creation failed. The program will not start!"));
    	while (1) {}
    }

    LOGGER_WRITELN((LOGGER_PROJECT_LOGO));

    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never reach this line */
    while (1) {}
}
