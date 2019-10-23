#include "settings.h"
#include "string.h"
#include "conv.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Default settings. They will never changed during runtime */
static const SETTINGS_t defaultSettings = {
		/* Http server related */
		.httpsrvIp = CONV_IP4_TO_UINT32(192, 168, 0, 105),
		.httpsrvSm = CONV_IP4_TO_UINT32(255, 255, 255, 0),
		.httpsrvGw = CONV_IP4_TO_UINT32(192, 168, 0, 1),
		.httpsrvDnsName = "lego"
};

/* Current settings. User code and IO tasks can modify them */
static SETTINGS_t settings = defaultSettings;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

SETTINGS_t *SETTINGS_GetInstance(void)
{
	return &settings;
}

const SETTINGS_t *SETTINGS_GetDefaults(void)
{
	return &defaultSettings;
}

void SETTINGS_Replace(const SETTINGS_t *src)
{
	memcpy(&settings, src, sizeof(SETTINGS_t));
}

void SETTINGS_RestoreDefaults(void)
{
	SETTINGS_Replace(&defaultSettings);
}
