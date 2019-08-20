#include "board_conf.h"
#include "gui_conf.h"
#include "gui_desktop.h"
#include "logger.h"
#include "gui_startup.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Httpsrv */
#include "httpsrv_conf.h"

/* SD card */
#include "sdcard_conf.h"

int main(void)
{
	/* Board initialization */
	BOARD_Init();
    BOARD_InitBacklightPWM();

    /* Start emWin library */
    LOGGER_WRITELN(("Configuring emWin library"));
    GUI_Start();

    GUI_StartupChangeStep("Initializing HTTP server. Plug in Ethernet cable");
    LOGGER_WRITELN(("Initializing HTTP server. Plug in Ethernet cable"));

    /* Start Http server (it internally creates RTOS tasks) */
    HTTPSRV_Init();

    GUI_StartupChangeStep("Creating RTOS tasks");
    LOGGER_WRITELN(("Creating RTOS tasks"));

    GUI_RTOSInit();

    GUI_StartupChangeStep("Configuring SD controller. Insert card");
    LOGGER_WRITELN(("Configuring SD controller. Insert card"));

    SDCARD_RTOSInit();

    LOGGER_WRITELN(("Launching application..."));
    LOGGER_WRITELN((LOGGER_PROJECT_LOGO));

    /* Open default window */
    GUI_DesktopCreate();

    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never reach this line */
    while (1) {}
}
