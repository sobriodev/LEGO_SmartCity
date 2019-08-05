#include "virtual_keyboard.h"
#include "stdbool.h"
#include "gui_conf.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE DATA TYPES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Virtual keyboard data
 */
typedef struct {
	VKParams_t params;								//!< Input params
	BUTTON_Handle buttons[VK_NUM_BUTTONS];			//!< GUI buttons
	BUTTON_Handle funcButtons[VK_NUM_FUNC_BUTTONS];	//!< GUI func buttons
	EDIT_Handle inputHandle;						//!< Input widget
	TEXT_Handle descHandle;							//!< Description widget
	bool shiftPressed;								//!< Shift state
	bool capsPressed;								//!< Caps-lock state
} VKData_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Buttons properties */
static const VKButton_t vkButtonsProps[VK_NUM_BUTTONS] =
{
		/* Row 0: Digits */
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '1'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '2'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '3'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '4'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '5'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '6'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '7'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '8'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '9'),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '0'),

		/* Row 1: qwertyuiop */
		VK_CREATE_BUTTON(1, 1, '`', 'Q', 'q'),
		VK_CREATE_BUTTON(1, 1, '~', 'W', 'w'),
		VK_CREATE_BUTTON(1, 1, '\\', 'E', 'e'),
		VK_CREATE_BUTTON(1, 1, '|', 'R', 'r'),
		VK_CREATE_BUTTON(1, 1, '<', 'T', 't'),
		VK_CREATE_BUTTON(1, 1, '>', 'Y', 'y'),
		VK_CREATE_BUTTON(1, 1, '{', 'U', 'u'),
		VK_CREATE_BUTTON(1, 1, '}', 'I', 'i'),
		VK_CREATE_BUTTON(1, 1, '[', 'O', 'o'),
		VK_CREATE_BUTTON(1, 1, ']', 'P', 'p'),

		/* Row 2: asdfghjkl */
		VK_CREATE_BUTTON(1, 1, '!', 'A', 'a'),
		VK_CREATE_BUTTON(1, 1, '@', 'S', 's'),
		VK_CREATE_BUTTON(1, 1, '#', 'D', 'd'),
		VK_CREATE_BUTTON(1, 1, '$', 'F', 'f'),
		VK_CREATE_BUTTON(1, 1, '%', 'G', 'g'),
		VK_CREATE_BUTTON(1, 1, '&', 'H', 'h'),
		VK_CREATE_BUTTON(1, 1, '*', 'J', 'j'),
		VK_CREATE_BUTTON(1, 1, '(', 'K', 'k'),
		VK_CREATE_BUTTON(1, 1, ')', 'L', 'l'),

		/* Row 3: zxcvbnm */
		VK_CREATE_BUTTON(1, 1, '-', 'Z', 'z'),
		VK_CREATE_BUTTON(1, 1, '\'', 'X', 'x'),
		VK_CREATE_BUTTON(1, 1, '\"', 'C', 'c'),
		VK_CREATE_BUTTON(1, 1, ':', 'V', 'v'),
		VK_CREATE_BUTTON(1, 1, ';', 'B', 'b'),
		VK_CREATE_BUTTON(1, 1, ',', 'N', 'n'),
		VK_CREATE_BUTTON(1, 1, '?', 'M', 'm'),

		/* Row 4: ,space. */
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, ','),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, ' '),
		VK_CREATE_BUTTON(0, 0, VK_UNUSED, VK_UNUSED, '.'),
};

/* Virtual keyboard data instance (handles and settings) */
static VKData_t vkData;

/* Used for converting char to string */
static char charToStrConvArray[2] = {0};

/* Used for getting text from widgets */
static char inputBuffer[VK_INPUT_MAX_LEN];

/* Skin properties */
static BUTTON_SKINFLEX_PROPS buttonGuiPropsBackup;
static BUTTON_SKINFLEX_PROPS buttonGuiClickedPropsBackup;

/* ID of VK window */
static WM_HWIN selfWin;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Convert character to string */
static const char *VK_CharToStr(uint8_t ch)
{
	charToStrConvArray[0] = ch;
	/* Make sure the null terminator is present */
	charToStrConvArray[1] = '\0';
	return charToStrConvArray;
}

/* Setup skinflex properties */
static void VK_setupGuiSkins(void)
{
	BUTTON_GetSkinFlexProps(&buttonGuiPropsBackup, BUTTON_SKINFLEX_PI_ENABLED);
	BUTTON_GetSkinFlexProps(&buttonGuiClickedPropsBackup, BUTTON_SKINFLEX_PI_ENABLED);

	BUTTON_SKINFLEX_PROPS buttonGuiProps = buttonGuiPropsBackup;
	BUTTON_SKINFLEX_PROPS buttonGuiClickedProps = buttonGuiClickedPropsBackup;

	buttonGuiProps.aColorLower[0] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorLower[1] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorUpper[0] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorUpper[1] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorFrame[0] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorFrame[1] = VK_BUTTON_BK_COLOR;
	buttonGuiProps.aColorFrame[2] = VK_BUTTON_BK_COLOR;

	buttonGuiClickedProps.aColorLower[0] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorLower[1] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorUpper[0] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorUpper[1] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorFrame[0] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorFrame[1] = VK_BUTTON_BK_CLICKED;
	buttonGuiClickedProps.aColorFrame[2] = VK_BUTTON_BK_CLICKED;

	BUTTON_SetSkinFlexProps(&buttonGuiProps, BUTTON_SKINFLEX_PI_ENABLED);
	BUTTON_SetSkinFlexProps(&buttonGuiClickedProps, BUTTON_SKINFLEX_PI_PRESSED);
}

/* Restore old skinflex properties */
static void VK_RestoreGuiSkins(void)
{
	BUTTON_SetSkinFlexProps(&buttonGuiPropsBackup, BUTTON_SKINFLEX_PI_ENABLED);
	BUTTON_SetSkinFlexProps(&buttonGuiClickedPropsBackup, BUTTON_SKINFLEX_PI_PRESSED);
}

/* Create buttons */
static bool VK_CreateButtons(void)
{
	/* Each row has it's own properties. First element of array determines the number of buttons, the second X origin, and the third Y origin */
	const uint8_t rowProps[VK_NUM_ROWS][3] = {
			{VK_BUTTONS_ROW0, VK_BUTTON_ROW0_X, VK_BUTTON_ROW0_Y},
			{VK_BUTTONS_ROW1, VK_BUTTON_ROW1_X, VK_BUTTON_ROW1_Y},
			{VK_BUTTONS_ROW2, VK_BUTTON_ROW2_X, VK_BUTTON_ROW2_Y},
			{VK_BUTTONS_ROW3, VK_BUTTON_ROW3_X, VK_BUTTON_ROW3_Y},
			{VK_BUTTONS_ROW4, VK_BUTTON_ROW4_X, VK_BUTTON_ROW4_Y}
	};

	uint8_t buttonOffset = 0; 	/* Current button array offset */
	uint8_t buttonSize; 		/* Determines button size. e.g spacebar is wider than normal button */
	uint16_t buttonXOffset;		/* Button X origin */

	for (uint8_t row = 0; row < VK_NUM_ROWS; row++) {

		/* Reset button offset */
		buttonXOffset = rowProps[row][1];

		for (uint8_t button = 0; button < rowProps[row][0]; button++, buttonOffset++, buttonXOffset += buttonSize + VK_BUTTON_XSPACING) {

			/* Check whether spacebar is processed */
			buttonSize = (buttonOffset == VK_BUTTON_SPACE_OFFSET) ? VK_BUTTON_BIG_XSIZE : VK_BUTTON_SMALL_XSIZE;

			vkData.buttons[buttonOffset] = BUTTON_CreateEx(buttonXOffset, rowProps[row][2], buttonSize, VK_BUTTON_YSIZE, selfWin, WM_CF_SHOW, 0, VK_BUTTON_ID + buttonOffset);

			if (!vkData.buttons[buttonOffset]) {
				GUI_FailedHook();
				return false;
			}

			/* Set default (basic) char */
			BUTTON_SetText(vkData.buttons[buttonOffset], VK_CharToStr(VK_GET_CHAR_BASIC(vkButtonsProps[buttonOffset])));
			BUTTON_SetTextColor(vkData.buttons[buttonOffset], BUTTON_CI_UNPRESSED, VK_BUTTON_TXT_COLOR);
			BUTTON_SetFont(vkData.buttons[buttonOffset], VK_FONT);
			BUTTON_SetFocusable(vkData.buttons[buttonOffset], 0);
		}
	}

	/* SHIFT button */
	vkData.funcButtons[VK_SHIFT_ARR] = BUTTON_CreateEx(VK_SHIFT_X, VK_BUTTON_ROW3_Y, VK_BUTTON_MEDIUM_XSIZE, VK_BUTTON_YSIZE, selfWin, WM_CF_SHOW, 0, VK_BUTTON_FUNC_ID + VK_SHIFT_ARR);
	/* CAPSLOCK button */
	vkData.funcButtons[VK_CAPS_ARR] = BUTTON_CreateEx(VK_CAPS_X, VK_BUTTON_ROW4_Y, VK_BUTTON_MEDIUM_XSIZE, VK_BUTTON_YSIZE, selfWin, WM_CF_SHOW, 0, VK_BUTTON_FUNC_ID + VK_CAPS_ARR);
	/* BACKSPACE button */
	vkData.funcButtons[VK_BACKSPACE_ARR] = BUTTON_CreateEx(VK_BACKSPACE_X, VK_BUTTON_ROW3_Y, VK_BUTTON_MEDIUM_XSIZE, VK_BUTTON_YSIZE, selfWin, WM_CF_SHOW, 0, VK_BUTTON_FUNC_ID + VK_BACKSPACE_ARR);
	/* ENTER button */
	vkData.funcButtons[VK_ENTER_ARR] = BUTTON_CreateEx(VK_ENTER_X, VK_BUTTON_ROW4_Y, VK_BUTTON_MEDIUM_XSIZE, VK_BUTTON_YSIZE, selfWin, WM_CF_SHOW, 0, VK_BUTTON_FUNC_ID + VK_ENTER_ARR);

	for (int i = 0; i < VK_NUM_FUNC_BUTTONS; i++) {
		if (!vkData.funcButtons[i]) {
			GUI_FailedHook();
			return false;
		}
		BUTTON_SetTextColor(vkData.funcButtons[i], BUTTON_CI_UNPRESSED, VK_BUTTON_TXT_COLOR);
		BUTTON_SetFont(vkData.funcButtons[i], VK_FONT);
		BUTTON_SetFocusable(vkData.funcButtons[i], 0);
	}

	/* Set texts only if buttons were created successfully */
	BUTTON_SetText(vkData.funcButtons[VK_SHIFT_ARR], VK_SHIFT_DI_TXT);
	BUTTON_SetText(vkData.funcButtons[VK_CAPS_ARR], VK_CAPS_DI_TXT);
	BUTTON_SetText(vkData.funcButtons[VK_BACKSPACE_ARR], VK_BACKSPACE_TXT);
	BUTTON_SetText(vkData.funcButtons[VK_ENTER_ARR], VK_ENTER_TXT);

	return true;
}

/* Create input widget and description */
static bool VK_CreateInput(void)
{
	/* Input edit widget */
	vkData.inputHandle = EDIT_CreateEx(VK_INPUT_X, VK_INPUT_Y, VK_INPUT_XSIZE, VK_INPUT_YSIZE, selfWin, WM_CF_SHOW, 0, VK_INPUT_ID, vkData.params.maxLen);

	if (!vkData.inputHandle) {
		GUI_FailedHook();
		return false;
	}

	EDIT_SetBkColor(vkData.inputHandle, EDIT_CI_ENABLED, VK_INPUT_BK_COLOR);
	EDIT_SetBkColor(vkData.inputHandle, EDIT_CI_CURSOR, VK_INPUT_CURSOR_COLOR);
	EDIT_SetFont(vkData.inputHandle, VK_FONT);
	WM_SetFocus(vkData.inputHandle); /* Edit widget should has focus by default */

	/* Store current input value */
	if (vkData.params.copyVal) {
		EDIT_GetText(vkData.params.inputHandle, inputBuffer, VK_INPUT_MAX_LEN);
		EDIT_SetText(vkData.inputHandle, inputBuffer);
	}

	/* Input description */
	vkData.descHandle = TEXT_CreateEx(VK_DESC_X, VK_DESC_Y, VK_DESC_XSIZE, VK_DESC_YSIZE, selfWin, WM_CF_SHOW, TEXT_CF_VCENTER | TEXT_CF_HCENTER, VK_DESC_ID, vkData.params.inputDesc);

	if (!vkData.descHandle) {
		GUI_FailedHook();
		return false;
	}

	TEXT_SetFont(vkData.descHandle, VK_FONT);

	return true;
}

/* Change buttons texts after shift was pressed */
static void VK_ButtonsAfterShift(void)
{
	for (uint8_t button = 0; button < VK_NUM_BUTTONS; button++) {
		if (VK_SHIFT_SUPP(vkButtonsProps[button])) {
			BUTTON_SetText(vkData.buttons[button], VK_CharToStr(VK_GET_CHAR_AFTER_SHIFT(vkButtonsProps[button])));
		}
	}
}

/* Change buttons texts after caps was pressed */
static void VK_ButtonsAfterCaps(void)
{
	for (uint8_t button = 0; button < VK_NUM_BUTTONS; button++) {
		if (VK_CAPS_SUPP(vkButtonsProps[button])) {
			BUTTON_SetText(vkData.buttons[button], VK_CharToStr(VK_GET_CHAR_AFTER_CAPS(vkButtonsProps[button])));
		}
	}
}

/* Set default buttons texts */
static void VK_ButtonsReset(void)
{
	for (uint8_t button = 0; button < VK_NUM_BUTTONS; button++) {
		BUTTON_SetText(vkData.buttons[button], VK_CharToStr(VK_GET_CHAR_BASIC(vkButtonsProps[button])));
	}
}

/* Routine called after functional button was pressed */
static void VK_FuncButtonClicked(uint8_t funcButton)
{
	switch (funcButton) {

	case VK_SHIFT_ARR:

		vkData.shiftPressed = !vkData.shiftPressed;

		if (vkData.shiftPressed) {
			VK_ButtonsAfterShift();
			BUTTON_SetText(vkData.funcButtons[VK_SHIFT_ARR], VK_SHIFT_EN_TXT);
		} else {
			if (vkData.capsPressed) {
				VK_ButtonsAfterCaps(); /* Shift is not pressed and caps is pressed */
			} else {
				VK_ButtonsReset(); /* Shift is not pressed and caps is not pressed */
			}

			BUTTON_SetText(vkData.funcButtons[VK_SHIFT_ARR], VK_SHIFT_DI_TXT);
		}

		break;

	case VK_CAPS_ARR:

		vkData.capsPressed = !vkData.capsPressed;

		if (vkData.capsPressed) {
			if (!vkData.shiftPressed) {
				VK_ButtonsAfterCaps(); /* Shift is not pressed so caps is active */
			}
			BUTTON_SetText(vkData.funcButtons[VK_CAPS_ARR], VK_CAPS_EN_TXT);
		} else {
			if (!vkData.shiftPressed) { /* Shift is not pressed so reset buttons texts */
				VK_ButtonsReset();
			}
			BUTTON_SetText(vkData.funcButtons[VK_CAPS_ARR], VK_CAPS_DI_TXT);
		}
		break;

	case VK_BACKSPACE_ARR:
		EDIT_AddKey(vkData.inputHandle, GUI_KEY_BACKSPACE);
		break;

	case VK_ENTER_ARR:

		EDIT_GetText(vkData.inputHandle, inputBuffer, VK_INPUT_MAX_LEN);

		/* Validation function was passed */
		if (vkData.params.validatorFn != NULL && vkData.params.validatorFn(inputBuffer)) {
			EDIT_SetText(vkData.params.inputHandle, inputBuffer);
		}

		WM_DeleteWindow(selfWin);
		break;

	default:
		break;

	}
}

/* Virtual keyboard callback */
static void VK_Callback(WM_MESSAGE *pMsg)
{
	int16_t notifyWidgetId;

	switch (pMsg->MsgId) {

	case WM_CREATE:
		selfWin = pMsg->hWin; /* Store self handle */

		/* Reset shift and caps states */
		vkData.shiftPressed = false;
		vkData.capsPressed = false;

		/* Setup GUI skins and create all buttons */
		VK_setupGuiSkins();
		VK_CreateButtons();
		break;

	case WM_USER_DATA:
		WM_GetUserData(selfWin, &vkData.params, sizeof(VKParams_t));
		VK_CreateInput(); /* Create input and description after user data was passed */

	case WM_PAINT:
		GUI_SetBkColor(VK_BK_COLOR);
		GUI_Clear();
		break;

	case WM_DELETE:
		VK_RestoreGuiSkins(); /* Before deleting keyboard window restore all changed GUI skin properties */

	case WM_NOTIFY_PARENT:
		notifyWidgetId = WM_GetId(pMsg->hWinSrc);

		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED) {
			if (notifyWidgetId >= VK_BUTTON_ID && notifyWidgetId < VK_BUTTON_FUNC_ID) {

				/* Normal button was clicked. Add new character */
				BUTTON_GetText(vkData.buttons[notifyWidgetId - VK_BUTTON_ID], charToStrConvArray, 2);
				EDIT_AddKey(vkData.inputHandle, charToStrConvArray[0]);

			} else if (notifyWidgetId >= VK_BUTTON_FUNC_ID && notifyWidgetId < VK_BUTTON_FUNC_ID + VK_NUM_FUNC_BUTTONS) {
				/* Functional button was clicked. Call specified routine */
				VK_FuncButtonClicked(notifyWidgetId - VK_BUTTON_FUNC_ID);
			}

		}

		break;

	default:
		WM_DefaultProc(pMsg);
	}
}


/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

bool VK_GetInput(const VKParams_t *params)
{
	WM_HWIN win = WM_CreateWindow(VK_X, VK_Y, VK_XSIZE, VK_YSIZE, WM_CF_SHOW, VK_Callback, sizeof(VKParams_t));

	if (!win) {
		GUI_FailedHook();
		return false;
	}

	WM_SetUserData(win, params, sizeof(VKParams_t)); /* Pass input data as user data */
	return true;
}

