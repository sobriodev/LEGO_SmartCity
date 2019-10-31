#ifndef UTILS_CONV_H_
#define UTILS_CONV_H_

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Compress IPv4 address */
#define CONV_IP4_TO_UINT32(ADDR3, ADDR2, ADDR1, ADDR0) \
	(uint32_t)(((ADDR0) & 0xFF) | (((ADDR1) & 0xFF) << 8) | (((ADDR2) & 0xFF) << 16) | (((ADDR3) & 0xFF) << 24))

/* Get desired octet from compressed IPv4 */
#define CONV_IP4_ADDR0(ADDR) ((uint8_t)((ADDR) & 0xFF))
#define CONV_IP4_ADDR1(ADDR) ((uint8_t)(((ADDR) >> 8) & 0xFF))
#define CONV_IP4_ADDR2(ADDR) ((uint8_t)(((ADDR) >> 16) & 0xFF))
#define CONV_IP4_ADDR3(ADDR) ((uint8_t)(((ADDR) >> 24) & 0xFF))

/* ----------------------------------------------------------------------------- */
/* ------------------------------ API FUNCTIONS -------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Convert string to ipv4 address
 *
 * \param str : Input string base address
 * \param addr3 : Octet 0 (x.x.x.[x])
 * \param addr2 : Octet 1 (x.x.[x].x)
 * \param addr1 : Octet 2 (x.[x].x.x)
 * \param addr0	: Octet 3 ([x].x.x.x)
 * \return True if conversion succeed, false otherwise
 */
bool CONV_StrToIP4(const char *str, uint8_t *addr3, uint8_t *addr2, uint8_t *addr1, uint8_t *addr0);

/*!
 * \brief Convert compressed ipv4 to string value
 * \param ip : Compressed ip address
 * \param dst : String buffer base address. The buffer has to accept at least 16 character (ip + terminating null)
 */
static inline void CONV_IP4CompresssedToStr(uint32_t ip, char *dst)
{
	snprintf(dst, 16, "%hhu.%hhu.%hhu.%hhu", CONV_IP4_ADDR3(ip), CONV_IP4_ADDR2(ip), CONV_IP4_ADDR1(ip), CONV_IP4_ADDR0(ip));
}

/*!
 * \brief Convert string ip value to compressed uint32_t value
 * \param str : Source string
 * \param dst : Output uint32_t base address
 * \return : True if conversion succeed, false otherwise
 */
bool CONV_IP4StrToCompressed(const char *str, uint32_t *dst);

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
