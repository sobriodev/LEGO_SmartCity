#ifndef UTILS_CONV_H_
#define UTILS_CONV_H_

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Compress IPv4 address */
#define CONV_IP4_TO_UINT32(ADDR0, ADDR1, ADDR2, ADDR3) \
	(uint32_t)(((ADDR0) & 0xFF) | (((ADDR1) & 0xFF) << 8) | (((ADDR2) & 0xFF) << 16) | (((ADDR3) & 0xFF) << 24))

/* Get desired octet from compressed IPv4 */
#define CONV_IP4_ADDR0(ADDR) ((ADDR) & 0xFF)
#define CONV_IP4_ADDR1(ADDR) (((ADDR) >> 8) & 0xFF)
#define CONV_IP4_ADDR2(ADDR) (((ADDR) >> 16) & 0xFF)
#define CONV_IP4_ADDR3(ADDR) (((ADDR) >> 24) & 0xFF)

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Convert string to ipv4 address
 *
 * \param str : Input string base address
 * \param addr0 : Octet 0 ([X].x.x.x)
 * \param addr1 : Octet 1 (x.[X].x.x)
 * \param addr2 : Octet 2 (x.x.[X].x)
 * \param addr3	: Octet 3 (x.x.x.[X])
 * \return True if conversion succeed, false otherwise
 */
static inline bool CONV_StrToIP4(const char *str, uint8_t *addr0, uint8_t *addr1, uint8_t *addr2, uint8_t *addr3)
{
	return (sscanf(str, "%d.%d.%d.%d", addr0, addr1, addr2, addr3) == 4);
}

/*!
 * \brief Convert string to int value
 *
 * \param str : Source string base address
 * \param dst : Output int base address
 * \return True if conversion succeed, false otherwise
 */
bool CONV_StrToInt(const char *str, int *dst);

/*!
 * \brief Convert string to double value
 *
 * \param str : Source string base address
 * \param dst : Output double base address
 * \return True if conversion succeed, false otherwise
 */
bool CONV_StrToDouble(const char *str, double *dst);

/*!
 * \brief Convert string to bool value
 *
 * \param str : Source string base address
 * \param dst : Output bool base address
 * \return True if conversion succeed, false otherwise
 */
bool CONV_StrToBool(const char *str, bool *dst);

#endif /* UTILS_CONV_H_ */
