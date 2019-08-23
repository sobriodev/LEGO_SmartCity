#include "settings.h"
#include "string.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------- PRIVATE VARIABLES ----------------------------- */
/* ----------------------------------------------------------------------------- */

/* Default settings. They will never changed during runtime */
static const Settings_t defaultSettings = {
		.a = 12
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
