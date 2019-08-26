#include "settings.h"
#include "string.h"

#include "misc.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Default settings. They will never changed during runtime */
static const Settings_t defaultSettings = {
		/* Http server related */
		.httpsrvIp = IP4_TO_UINT32(192, 168, 0, 105),
		.httpsrvSm = IP4_TO_UINT32(255, 255, 255, 0),
		.httpsrvGw = IP4_TO_UINT32(192, 168, 0, 100),
		.httpsrvDnsName = "lego"
};

/* Current settings. User code and IO tasks can modify them */
static Settings_t settings = defaultSettings;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

Settings_t *SETTINGS_GetInstance(void)
{
	return &settings;
}

const Settings_t *SETTINGS_GetDefaults(void)
{
	return &defaultSettings;
}

void SETTINGS_Replace(const Settings_t *src)
{
	memcpy(&settings, src, sizeof(Settings_t));
}

void SETTINGS_RestoreDefaults(void)
{
	SETTINGS_Replace(&defaultSettings);
}
