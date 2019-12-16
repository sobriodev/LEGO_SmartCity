#include "board_conf.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_sctimer.h"
#include "fsl_iocon.h"
#include "fsl_spifi.h"

#include "gui_conf.h"
#include "gui_startup.h"
#include "gui_desktop.h"

#include "logger.h"

/* Touch panel */
#include "fsl_i2c.h"
#include "fsl_ft5406.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "GUIDRV_Lin.h"

/* Httpsrv */
#include "httpsrv_conf.h"

/* SD card */
#include "sdcard_conf.h"
#include "fsl_sdmmc_host.h"

/* Lego */
#include "lego.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE MACROS ------------------------------- */
/* ----------------------------------------------------------------------------- */

#define BOARD_I2C_MASTER_CLOCK_FREQUENCY 12000000

/* Board QSPI Flash */
#define BOARD_FLASH_SPIFI SPIFI0
#define PAGE_SIZE (256)
#define SECTOR_SIZE (4096)
#define EXAMPLE_SPI_BAUDRATE (96000000)
#define FLASH_W25Q
#define COMMAND_NUM (6)
#define READ (0)
#define PROGRAM_PAGE (1)
#define GET_STATUS (2)
#define ERASE_SECTOR (3)
#define WRITE_ENABLE (4)
#define WRITE_REGISTER (5)


/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

static uint32_t backlightPWMEvent;

/* Touch driver handle. */
extern ft5406_handle_t touch_handle;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Initialize the QSPI Flash memory. */
void BOARD_InitQSPI(void)
{
	spifi_config_t config = {0};
	uint32_t sourceClockFreq;
	uint8_t val = 0;

#if defined FLASH_W25Q
	spifi_command_t command[COMMAND_NUM] = {
			{PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
			{PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x32},
			{1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
			{1, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x31}};
#define QUAD_MODE_VAL 0x02
#elif defined FLASH_MX25R
	spifi_command_t command[COMMAND_NUM] = {
			{PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
			{PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x38},
			{1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
			{1, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x01}};
#define QUAD_MODE_VAL 0x40
#else /* Use MT25Q */
	spifi_command_t command[COMMAND_NUM] = {
			{PAGE_SIZE, false, kSPIFI_DataInput, 1, kSPIFI_CommandDataQuad, kSPIFI_CommandOpcodeAddrThreeBytes, 0x6B},
			{PAGE_SIZE, false, kSPIFI_DataOutput, 0, kSPIFI_CommandOpcodeSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x38},
			{1, false, kSPIFI_DataInput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x05},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeAddrThreeBytes, 0x20},
			{0, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x06},
			{1, false, kSPIFI_DataOutput, 0, kSPIFI_CommandAllSerial, kSPIFI_CommandOpcodeOnly, 0x61}};
#endif

	BOARD_BootClockFROHF96M(); /* Boot up FROHF96M for SPIFI to use*/
	CLOCK_AttachClk(kFRO_HF_to_SPIFI_CLK);
	sourceClockFreq = CLOCK_GetFroHfFreq();
	/* Set the clock divider */
	CLOCK_SetClkDiv(kCLOCK_DivSpifiClk, sourceClockFreq / EXAMPLE_SPI_BAUDRATE, false);
	/* Initialize SPIFI */
	SPIFI_GetDefaultConfig(&config);
	SPIFI_Init(BOARD_FLASH_SPIFI, &config);
#if defined QUAD_MODE_VAL
	/* Enable Quad mode */
	SPIFI_SetCommand(BOARD_FLASH_SPIFI, &command[WRITE_ENABLE]);
	SPIFI_SetCommand(BOARD_FLASH_SPIFI, &command[WRITE_REGISTER]);
	SPIFI_WriteDataByte(BOARD_FLASH_SPIFI, QUAD_MODE_VAL);
	/* Check WIP bit */
	val = 0;
	do{
		SPIFI_SetCommand(BOARD_FLASH_SPIFI, &command[GET_STATUS]);
		while ((BOARD_FLASH_SPIFI->STAT & SPIFI_STAT_INTRQ_MASK) == 0U);
		val = SPIFI_ReadDataByte(BOARD_FLASH_SPIFI);
	} while (val & 0x1);
#endif
	/* Setup memory command */
	SPIFI_SetMemoryCommand(BOARD_FLASH_SPIFI, &command[READ]);
}

/* This function is used to cover the IP bug which the DATA4-7 pin should be configured */
static void BOARD_InitSdifUnusedDataPin(void)
{
	IOCON_PinMuxSet(IOCON, 4, 29,
			(IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[4] */
	IOCON_PinMuxSet(IOCON, 4, 30,
			(IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[5] */
	IOCON_PinMuxSet(IOCON, 4, 31,
			(IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[6] */
	IOCON_PinMuxSet(IOCON, 5, 0,
			(IOCON_FUNC2 | IOCON_PIO_SLEW_MASK | IOCON_DIGITAL_EN | IOCON_MODE_PULLUP)); /* sd data[7] */
}

/* Init FLEXCOMM1 as I2C master */
static void BOARD_InitI2C(void)
{
    /* Attach 12 MHz clock to FLEXCOMM1 (I2C master) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1);
    RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn);
    BOARD_I2C_Init(I2C1, BOARD_I2C_MASTER_CLOCK_FREQUENCY);
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- FREERTOS TASKS ----------------------------- */
/* ----------------------------------------------------------------------------- */

static void BOARD_StartupTask(void *pvParameters)
{
	/* SD card must be initialized first. Other routines use settings struct */
	SDCARD_RTOSInit();

	LOGGER_WRITELN(("Initializing SD card"));
	GUI_StartupChangeStep("Initializing SD card");
    if (SDCARD_Init()) {
    	SDCARD_ImportSettings();
    }

    /* Start Http server (it internally creates RTOS tasks) */
    LOGGER_WRITELN(("Initializing HTTP server"));
    GUI_StartupChangeStep("Initializing HTTP server. Plug in Ethernet cable");
    HTTPSRV_Init();

    /* Initialize I2C devices */
    LOGGER_WRITELN(("Performing LEGO startup. Initializing I2C devices"));
    GUI_StartupChangeStep("Performing LEGO startup. Initializing I2C devices");
    if (!LEGO_PerformStartup()) {
    	LOGGER_WRITELN(("LEGO startup failure"));
    }

    LOGGER_WRITELN(("Initializing LEGO RTOS"));
    GUI_StartupChangeStep("Initializing LEGO RTOS");
    if (!LEGO_RTOSInit()) {
    	LOGGER_WRITELN(("LEGO RTOS init failure. Check heap and stack usage"));
    }

    /* Open desktop window */
    GUI_DesktopCreate();

    /* Start GUI and touch tasks */
    if (!GUI_RTOSInit()) {
    	LOGGER_WRITELN(("GUI/Touch RTOS error. Check heap and stack usage"));
    }

    LOGGER_WRITELN((LOGGER_PROJECT_LOGO));

    vTaskDelete(NULL); /* Deleting startup task allows lower priority tasks (GUI, server, ...) to work */
}

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
    pwmParam.dutyCyclePercent = LCD_MAX_BRIGHTNESS; /* Default max brightness */

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
	BOARD_InitQSPI();

	/* Attach main clock to SDIF */
	CLOCK_AttachClk(BOARD_SDIF_CLK_ATTACH);
	/* This function is used to cover the IP bug which the DATA4-7 pin should be configured, otherwise the SDIF will not work */
	BOARD_InitSdifUnusedDataPin();
	/* Need call this function to clear the halt bit in clock divider register */
	CLOCK_SetClkDiv(kCLOCK_DivSdioClk, (uint32_t)(SystemCoreClock / FSL_FEATURE_SDIF_MAX_SOURCE_CLOCK + 1U), true);
	/* Set SD host interrupt priority */
	NVIC_SetPriority(SD_HOST_IRQ, 5U);

	/* Init I2C */
	BOARD_InitI2C();
}

void BOARD_SetBacklightPercent(uint8_t percent)
{
	/* Must be from 1 (LCD off) to 99 (LCD max brightness) */
	assert(percent > 0 && percent < 100);
	SCTIMER_UpdatePwmDutycycle(SCT0, 5, percent, backlightPWMEvent);
}

void BOARD_TouchEvent(BOARD_TouchInfo_t *touchInfo)
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

bool BOARD_RTOSInit(void)
{
	return (xTaskCreate(BOARD_StartupTask, BOARD_TASK_STARTUP_NAME, BOARD_TASK_STARTUP_STACK, NULL, BOARD_TASK_STARTUP_PRIO, NULL) != pdFAIL);
}

void BOARD_SystemReset(void)
{
	NVIC_SystemReset();
}

bool BOARD_I2C_SendSingleReg(uint8_t devAddr, uint8_t txBuff)
{
	return (BOARD_I2C_Send(I2C1, devAddr, 0, 0, &txBuff, 1) == kStatus_Success);
}

bool BOARD_I2C_ReadSingleReg(uint8_t devAddr, uint8_t *rxBuff)
{
	return (BOARD_I2C_Receive(I2C1, devAddr, 0, 0, rxBuff, 1) == kStatus_Success);
}

bool BOARD_I2C_SendMultiReg(uint8_t devAddr, uint8_t regAddr, uint8_t txBuff)
{
	return (BOARD_I2C_Send(I2C1, devAddr, regAddr, 1, &txBuff, 1) == kStatus_Success);
}

bool BOARD_I2C_ReadMultiReg(uint8_t devAddr, uint8_t regAddr, uint8_t *rxBuff)
{
	return (BOARD_I2C_Receive(I2C1, devAddr, regAddr, 1, rxBuff, 1) == kStatus_Success);
}

bool BOARD_I2C_SendMultiByte(uint8_t devAddr, uint16_t regAddr, uint8_t *txBuff, uint8_t buffLen)
{
	return (BOARD_I2C_Send(I2C1, devAddr, regAddr, 2, txBuff, buffLen) == kStatus_Success);
}

bool BOARD_I2C_ReadMultiByte(uint8_t devAddr, uint16_t regAddr, uint8_t *rxBuff, uint8_t bytesRead)
{
	return (BOARD_I2C_Receive(I2C1, devAddr, regAddr, 2, rxBuff, bytesRead) == kStatus_Success);
}
