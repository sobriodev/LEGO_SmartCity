#ifndef UTILS_MISC_H_
#define UTILS_MISC_H_

#include "stdio.h"
#include "stdint.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* Compress IPv4 address */
#define IP4_TO_UINT32(ADDR0, ADDR1, ADDR2, ADDR3) \
	(uint32_t)(((ADDR0) & 0xFF) | (((ADDR1) & 0xFF) << 8) | (((ADDR2) & 0xFF) << 16) | (((ADDR3) & 0xFF) << 24))

/* Get desired octet from compressed IPv4 */
#define IP4_ADDR0(ADDR) ((ADDR) & 0xFF)
#define IP4_ADDR1(ADDR) (((ADDR) >> 8) & 0xFF)
#define IP4_ADDR2(ADDR) (((ADDR) >> 16) & 0xFF)
#define IP4_ADDR3(ADDR) (((ADDR) >> 24) & 0xFF)

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

static inline void stringToIPv4(const char *str, uint8_t *addr0, uint8_t *addr1, uint8_t *addr2, uint8_t *addr3)
{
	sscanf(str, "%d.%d.%d.%d", addr0, addr1, addr2, addr3);
}

#endif /* UTILS_MISC_H_ */
