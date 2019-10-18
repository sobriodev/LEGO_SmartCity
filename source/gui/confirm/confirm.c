#include "confirm.h"
#include "gui_conf.h"

/* emWin */
#include "WM.h"
#include "BUTTON.h"
#include "TEXT.h"
#include "DIALOG.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

#define CONFIRM_WIDTH 300
#define CONFIRM_HEIGTH 150

#define CONFIRM_BTN_WIDTH	60
#define CONFIRM_BTN_HEIGHT	30

#define CONFIRM_TXT_ID	GUI_ID_TEXT0
#define CONFIRM_YES_ID	GUI_ID_BUTTON0
#define CONFIRM_NO_ID	(CONFIRM_YES_ID + 1)

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Confirm dialog */
static const GUI_WIDGET_CREATE_INFO confirmInfoConfirm[] = {
		{ WINDOW_CreateIndirect, NULL, 0, 0, 0, CONFIRM_WIDTH, CONFIRM_HEIGTH, 0, 0, 0 },
		{ TEXT_CreateIndirect,   NULL, CONFIRM_TXT_ID, 0, 0, CONFIRM_WIDTH - 20, 75, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
		{ BUTTON_CreateIndirect, CONFIRM_YES_TXT, CONFIRM_YES_ID, 70, 97, CONFIRM_BTN_WIDTH, CONFIRM_BTN_HEIGHT, 0, 0, 0},
		{ BUTTON_CreateIndirect, CONFIRM_NO_TXT, CONFIRM_NO_ID, 160, 97, CONFIRM_BTN_WIDTH, CONFIRM_BTN_HEIGHT, 0, 0, 0},
};

/* Alert dialog */
static const GUI_WIDGET_CREATE_INFO confirmInfoAlert[] = {
		{ WINDOW_CreateIndirect, NULL, 0, 0, 0, CONFIRM_WIDTH, CONFIRM_HEIGTH, 0, 0, 0 },
		{ TEXT_CreateIndirect,   NULL, CONFIRM_TXT_ID, 0, 0, CONFIRM_WIDTH - 20, 75, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0, 0},
		{ BUTTON_CreateIndirect, CONFIRM_OK_TXT, CONFIRM_YES_ID, 115, 97, CONFIRM_BTN_WIDTH, CONFIRM_BTN_HEIGHT, 0, 0, 0},
};

static WM_HWIN dialogWin; /* Self win */
static const CONFIRM_Params_t *dialogParams;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

static void CONFIRM_InitWidgets()
{
	TEXT_Handle text = WM_GetDialogItem(dialogWin, CONFIRM_TXT_ID);
	TEXT_SetWrapMode(text, GUI_WRAPMODE_WORD);
	TEXT_SetTextColor(text, CONFIRM_TXT_COLOR);
	TEXT_SetText(text, dialogParams->msg);
}

static void CONFIRM_Callback(WM_MESSAGE *pMsg)
{
	int16_t widgetId;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		dialogWin = pMsg->hWin;
		CONFIRM_InitWidgets();
		break;
	case WM_PAINT:
		GUI_SetBkColor(CONFIRM_BK_COLOR);
		GUI_Clear();
		break;
	case WM_NOTIFY_PARENT:
		widgetId = WM_GetId(pMsg->hWinSrc);

		if (pMsg->Data.v == WM_NOTIFICATION_CLICKED) {
			switch (widgetId) {
			case CONFIRM_YES_ID:
				GUI_EndDialog(dialogWin, CONFIRM_YES); /* User confirmed */
				break;
			case CONFIRM_NO_ID:
				GUI_EndDialog(dialogWin, CONFIRM_NO); /* User rejected */
				break;
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

CONFIRM_Status_t CONFIRM_Exec(const CONFIRM_Params_t *params)
{
	dialogParams = params;
	WM_HWIN dialog = (dialogParams->type == CONFIRM_CONFIRM) ? GUI_CreateDialogBox(confirmInfoConfirm, GUI_COUNTOF(confirmInfoConfirm), CONFIRM_Callback, WM_HBKWIN, 90, 61)
															 : GUI_CreateDialogBox(confirmInfoAlert, GUI_COUNTOF(confirmInfoAlert), CONFIRM_Callback, WM_HBKWIN, 90, 61);

	if (!dialog) {
		return CONFIRM_FAILURE;
	}

	WM_MakeModal(dialog);
	return (CONFIRM_Status_t)GUI_ExecCreatedDialog(dialog);
}
