#include <board_conf.h>
#include "gui_conf.h"
#include "rtos.h"

/* Logger */
#include "logger.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

int main(void)
{
	/* Board initialization */
	BOARD_Init();

    /* Set the backlight PWM. */
    BOARD_InitBacklightPWM();

    /* Start emWin library */
    GUI_Start();

    if (!RTOS_TasksCreate() || !RTOS_TimersCreate()) {
    	LOGGER_WRITELN(CRITICAL_LOG, LOG_ERROR, "FreeRTOS tasks creation failed. The program will not start!");
    	while (1) {}
    }

    LOGGER_WRITELN(CRITICAL_LOG, LOG_INFO, "Program initialization succeed. Starting...");

    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never enter this line */
    while (1) {}
}
