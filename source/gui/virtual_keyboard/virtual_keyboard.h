#ifndef GUI_VIRTUAL_KEYBOARD_VIRTUAL_KEYBOARD_H_
#define GUI_VIRTUAL_KEYBOARD_VIRTUAL_KEYBOARD_H_

#include "stdint.h"
#include "validator.h"

/* emWin */
#include "emwin_support.h"
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "TEXT.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- MACROS (CORE) ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Getting and storing values */
#define VK_BIT(POS)						(1 << (POS))
#define VK_FLAG_SIZE					1
#define VK_CHAR_SIZE					8

/* Values positions */
#define VK_BASIC_POS					0
#define VK_AFTER_CAPS_POS				8
#define VK_AFTER_SHIFT_POS				16
#define VK_CAPS_SUPP_POS				24
#define VK_SHIFT_SUPP_POS				25

/* Masks */
#define VK_BASIC_MASK					0xFF
#define VK_AFTER_CAPS_MASK				0xFF00
#define VK_AFTER_SHIFT_MASK				0xFF0000
#define VK_CAPS_SUPP_MASK				VK_BIT(VK_CAPS_SUPP_POS)
#define VK_SHIFT_SUPP_MASK				VK_BIT(VK_SHIFT_SUPP_POS)

/* Value getters and issers */
#define VK_GET_CHAR_BASIC(BUTTON)		(((BUTTON) & VK_BASIC_MASK) >> VK_BASIC_POS)
#define VK_GET_CHAR_AFTER_CAPS(BUTTON)	(((BUTTON) & VK_AFTER_CAPS_MASK) >> VK_AFTER_CAPS_POS)
#define VK_GET_CHAR_AFTER_SHIFT(BUTTON)	(((BUTTON) & VK_AFTER_SHIFT_MASK) >> VK_AFTER_SHIFT_POS)

/* Check whether button supports CAPS or SHIFT operations */
#define VK_CAPS_SUPP(BUTTON)			(((BUTTON) & VK_CAPS_SUPP_MASK) >> VK_CAPS_SUPP_POS)
#define VK_SHIFT_SUPP(BUTTON)			(((BUTTON) & VK_SHIFT_SUPP_MASK) >> VK_SHIFT_SUPP_POS)

/* If button does not support CAPS or SHIFT operations pass this value in SHIFT/CAPS place */
#define VK_UNUSED						0

/* Create button macro for convenience */
#define VK_CREATE_BUTTON(SH_SP, CP_SP, AF_SH, AF_CP, BASIC) \
	((((BASIC) & 0xFF) << VK_BASIC_POS) | (((AF_CP) & 0xFF) << VK_AFTER_CAPS_POS) | (((AF_SH) & 0xFF) << VK_AFTER_SHIFT_POS) | (((CP_SP) & 0x1) << VK_CAPS_SUPP_POS)  | (((SH_SP) & 0x1) << VK_SHIFT_SUPP_POS))


/* ----------------------------------------------------------------------------- */
/* --------------------------------- MACROS (GUI) ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Window sizing */
#define VK_XSIZE					LCD_WIDTH
#define VK_YSIZE					LCD_HEIGHT
#define VK_X						0
#define VK_Y						0

#define VK_NUM_BUTTONS				39
#define VK_BUTTON_SPACE_OFFSET		(VK_NUM_BUTTONS - 2) /* Spacebar array offset (used for drawing operations) */
#define VK_NUM_FUNC_BUTTONS			4

/* GUI IDs */
#define VK_BUTTON_ID				GUI_ID_USER /* Button start ID */
#define VK_BUTTON_FUNC_ID			(VK_BUTTON_ID + VK_NUM_BUTTONS) /* Functional button start ID */

/* Buttons row count */
#define VK_NUM_ROWS					5
#define VK_BUTTONS_ROW0				10
#define VK_BUTTONS_ROW1				10
#define VK_BUTTONS_ROW2				9
#define VK_BUTTONS_ROW3				7
#define VK_BUTTONS_ROW4				3

/* Button sizing and spacing */
#define VK_BUTTON_SMALL_XSIZE		40
#define VK_BUTTON_MEDIUM_XSIZE		50
#define VK_BUTTON_BIG_XSIZE			124
#define VK_BUTTON_YSIZE				30	/* Height is always the same */
#define VK_BUTTON_XSPACING			2
#define VK_BUTTON_YSPACING			VK_BUTTON_XSPACING

/* Keyboard rows X and Y origins */
#define VK_BUTTON_ROW0_X			31
#define VK_BUTTON_ROW1_X			VK_BUTTON_ROW0_X
#define VK_BUTTON_ROW2_X			52
#define VK_BUTTON_ROW3_X			94
#define VK_BUTTON_ROW4_X			136
#define VK_BUTTON_ROW0_Y			110
#define VK_BUTTON_ROW1_Y			(VK_BUTTON_ROW0_Y + VK_BUTTON_YSIZE + VK_BUTTON_YSPACING)
#define VK_BUTTON_ROW2_Y			(VK_BUTTON_ROW1_Y + VK_BUTTON_YSIZE + VK_BUTTON_YSPACING)
#define VK_BUTTON_ROW3_Y			(VK_BUTTON_ROW2_Y + VK_BUTTON_YSIZE + VK_BUTTON_YSPACING)
#define VK_BUTTON_ROW4_Y			(VK_BUTTON_ROW3_Y + VK_BUTTON_YSIZE + VK_BUTTON_YSPACING)

/* Functional buttons have absolute positions */
#define VK_SHIFT_X					42
#define VK_CAPS_X					84
#define VK_BACKSPACE_X				388
#define VK_ENTER_X					346

/* Functional buttons names */
#define VK_SHIFT_DI_TXT				"!@#"
#define VK_SHIFT_EN_TXT				"abc"
#define VK_CAPS_DI_TXT				"AB"
#define VK_CAPS_EN_TXT				"ab"
#define VK_BACKSPACE_TXT			"BACK"
#define VK_ENTER_TXT				"ENTER"

/* Functional buttons array offsets */
#define VK_SHIFT_ARR				0
#define VK_CAPS_ARR					(VK_SHIFT_ARR + 1)
#define VK_BACKSPACE_ARR			(VK_SHIFT_ARR + 2)
#define VK_ENTER_ARR				(VK_SHIFT_ARR + 3)

/* Input related */
#define VK_INPUT_X					20
#define VK_INPUT_Y					65
#define VK_INPUT_XSIZE				440
#define VK_INPUT_YSIZE				35
#define VK_INPUT_ID					GUI_ID_EDIT0
#define VK_INPUT_MAX_LEN			255

/* Description related */
#define VK_DESC_X					VK_X
#define VK_DESC_Y					10
#define VK_DESC_XSIZE				VK_XSIZE
#define VK_DESC_YSIZE				45
#define VK_DESC_ID					GUI_ID_TEXT0

/* Colors and fonts */
#define VK_FONT						&GUI_Font8x15B_1
#define VK_BUTTON_BK_COLOR			0x0038342C
#define VK_BUTTON_BK_CLICKED		0x00665E4F
#define VK_BUTTON_TXT_COLOR			0x00DADADA
#define VK_INPUT_BK_COLOR			0x00DDD0D0
#define VK_INPUT_CURSOR_COLOR		GUI_BLACK
#define VK_BK_COLOR					GUI_GRAY

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- DATA TYPES ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \typedef Button format
 *
 * \note Format: UNUSED(31:26) | SHIFT_SUPPORT(25) | CAPS_SUPPPORT(24) | AFTER_SHIFT(23:16) | AFTER_CAPS(15:8) | BASIC(7:0)
 */
typedef uint32_t VKButton_t;

/*!
 * \brief Input parameters struct
 */
typedef struct {
	EDIT_Handle inputHandle; 	//!< Source input GUI handle
	const char *inputDesc; 		//!< Input description
	uint8_t maxLen; 			//!< Maximum input length
	bool copyVal; 				//!< True if current value should be copied to the virtual keyboard, false otherwise
	VALIDATORFn_t validatorFn;	//!< Optional validation function, NULL if not used
} VKParams_t;

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Get input from virtual keyboard
 *
 * \param params : Input params. See VK_Params_t for more information
 * \return True if windows creation succeed, false otherwise
 */
bool VK_GetInput(const VKParams_t *params);

#endif /* GUI_VIRTUAL_KEYBOARD_VIRTUAL_KEYBOARD_H_ */
