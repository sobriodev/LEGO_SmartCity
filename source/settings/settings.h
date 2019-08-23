#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Settings structure
 */
typedef struct {
	/* Http server related */
	int a;
} Settings_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Get settings instance
 *
 * \return Settings struct base address
 */
Settings_t *SETTINGS_GetInstance(void);

/*!
 * \brief Get default settings
 *
 * \return Default settings base address
 */
const Settings_t *SETTINGS_GetDefaults(void);

/*!
 * \brief Replace current settings
 *
 * \param src : New settings base address
 */
void SETTINGS_Replace(const Settings_t *src);

/*!
 * \brief Restore default settings
 */
void SETTINGS_RestoreDefaults(void);

#endif /* SETTINGS_SETTINGS_H_ */
