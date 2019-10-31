#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

#include "stdbool.h"
#include "stdint.h"
#include "lcd.h"
#include "validator.h"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- DATA TYPES --------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Settings structure
 */
typedef struct {
	/* Http server */
	uint32_t httpsrvIp;
	uint32_t httpsrvSm;
	uint32_t httpsrvGw;
	char httpsrvDnsName[VALIDATOR_DNS_LEN];
	/* Display */
	uint8_t lcdBrightness;  /* in percentage */
	LCD_DimmingTime_t lcdDimmingTime; /* in milliseconds */
} SETTINGS_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Get settings instance
 *
 * \return Settings struct base address
 */
SETTINGS_t *SETTINGS_GetInstance(void);

/*!
 * \brief Get default settings
 *
 * \return Default settings base address
 */
const SETTINGS_t *SETTINGS_GetDefaults(void);

/*!
 * \brief Replace current settings
 *
 * \param src : New settings base address
 */
void SETTINGS_Replace(const SETTINGS_t *src);

/*!
 * \brief Restore default settings
 */
void SETTINGS_RestoreDefaults(void);

#endif /* SETTINGS_SETTINGS_H_ */
