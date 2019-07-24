#include "board_init.h"
#include "gui_conf.h"
#include "rtos.h"

/* Logger */
#include "logger.h"
#include "logger_def.h"

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

    bool tasksCreationSuceed = RTOS_TasksCreate();

    if (!tasksCreationSuceed) {

    	/* Task creation failed. Make optional log message and enter infinite loop */
		#if LOGGING_ENABLED && CORE_LOG
				LOGGER_LogEvent(LOG_ERROR, "Server tasks creation failed. The program will not start!\r\n");
		#endif

    	while (1) {}
    }

	#if LOGGING_ENABLED && CORE_LOG
		LOGGER_LogEvent(LOG_INFO, "Server configuration is done. Starting application\r\n");
	#endif


    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never enter this line */
    while (1) {}
}
