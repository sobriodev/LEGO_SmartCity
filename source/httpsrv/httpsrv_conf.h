#ifndef HTTPSRV_HTTPSRV_CONF_H_
#define HTTPSRV_HTTPSRV_CONF_H_

/* lwIP */
#include "opt.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "ctype.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"
#include "enet_ethernetif.h"
#include "board.h"
#include "httpsrv.h"
#include "mdns.h"
#include "pin_mux.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* ----------------------------------- MACROS ---------------------------------- */
/* ----------------------------------------------------------------------------- */

/* IP address configuration. */
#define configIP_ADDR0 192
#define configIP_ADDR1 168
#define configIP_ADDR2 0
#define configIP_ADDR3 102

/* Netmask configuration. */
#define configNET_MASK0 255
#define configNET_MASK1 255
#define configNET_MASK2 255
#define configNET_MASK3 0

/* Gateway address configuration. */
#define configGW_ADDR0 192
#define configGW_ADDR1 168
#define configGW_ADDR2 0
#define configGW_ADDR3 100

/* MAC address configuration. */
#define configMAC_ADDR {0x02, 0x12, 0x13, 0x10, 0x15, 0x11}

/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS

/* System clock name. */
#define EXAMPLE_CLOCK_NAME kCLOCK_CoreSysClk

/* Memory not usable by ENET DMA. */
#define NON_DMA_MEMORY_ARRAY {{0x0U, 0x80000U}, {0x0U, 0x0U}}

#define HTTPD_DEBUG LWIP_DBG_ON
#define HTTPD_STACKSIZE DEFAULT_THREAD_STACKSIZE
#define HTTPD_PRIORITY DEFAULT_THREAD_PRIO
#define DEBUG_WS 0

#define MDNS_HOSTNAME "lego-city"

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void HTTPSRV_Init(void);

#endif /* HTTPSRV_HTTPSRV_CONF_H_ */
