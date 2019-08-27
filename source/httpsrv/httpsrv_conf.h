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

/* MAC address configuration. */
#define HTTPSRV_MAC_ADDR 				{0x02, 0x12, 0x13, 0x10, 0x15, 0x11}
/* Address of PHY interface. */
#define HTTPSRV_PHY_ADDRESS 			BOARD_ENET0_PHY_ADDRESS
/* System clock name. */
#define HTTPSRV_CLOCK_NAME 				kCLOCK_CoreSysClk
/* Memory not usable by ENET DMA. */
#define HTTPSRV_NON_DMA_MEMORY_ARRAY	{{0x0U, 0x80000U}, {0x0U, 0x0U}}

/* lwIP specific macros */
#define HTTPD_DEBUG 					LWIP_DBG_ON
#define HTTPD_STACKSIZE					DEFAULT_THREAD_STACKSIZE
#define HTTPD_PRIORITY 					DEFAULT_THREAD_PRIO

#define HTTPSRV_DEFAULT_PAGE			"/index.html"
/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void HTTPSRV_Init(void);

#endif /* HTTPSRV_HTTPSRV_CONF_H_ */
