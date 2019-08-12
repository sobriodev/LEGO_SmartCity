#include "httpsrv_conf.h"
#include "logger.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Server internal config */
static struct netif fsl_netif0;
static mem_range_t non_dma_memory[] = NON_DMA_MEMORY_ARRAY;

/* FS data generated automatically */
extern const HTTPSRV_FS_DIR_ENTRY httpsrv_fs_data[];

/* Routes table */
extern const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[];

/* IP, Netmask and Gateway addresses */
static ip4_addr_t fsl_netif0_ipaddr;
static ip4_addr_t fsl_netif0_netmask;
static ip4_addr_t fsl_netif0_gw;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Callback function to generate TXT mDNS record for HTTP service */
static void http_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", 6);
}

/* Init lwIP stack */
static void stack_init(void)
{
    ethernetif_config_t fsl_enet_config0 = {
        .phyAddress = EXAMPLE_PHY_ADDRESS,
        .clockName  = EXAMPLE_CLOCK_NAME,
        .macAddress = configMAC_ADDR,
        .non_dma_memory = non_dma_memory,
    };

    tcpip_init(NULL, NULL);

    IP4_ADDR(&fsl_netif0_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
    IP4_ADDR(&fsl_netif0_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
    IP4_ADDR(&fsl_netif0_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

    netifapi_netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, &fsl_enet_config0,
                       ethernetif0_init, tcpip_input);
    netifapi_netif_set_default(&fsl_netif0);
    netifapi_netif_set_up(&fsl_netif0);

    mdns_resp_init();
    mdns_resp_add_netif(&fsl_netif0, MDNS_HOSTNAME, 60);
    mdns_resp_add_service(&fsl_netif0, MDNS_HOSTNAME, "_http", DNSSD_PROTO_TCP, 80, 300, http_srv_txt, NULL);
}

/* Init server */
static void http_server_socket_init(void)
{
    HTTPSRV_PARAM_STRUCT params;
    uint32_t httpsrv_handle;

    /* Init Fs*/
    HTTPSRV_FS_init(httpsrv_fs_data);

    /* Init HTTPSRV parameters.*/
    memset(&params, 0, sizeof(params));
    params.root_dir = "";
    params.index_page = "/index.html";
    params.auth_table = NULL;
    params.cgi_lnk_tbl = HTTPSRV_ApiTable;
    params.ssi_lnk_tbl = NULL;

    /* Init HTTP Server.*/
    httpsrv_handle = HTTPSRV_init(&params);
    if (httpsrv_handle == 0) {
    	LOGGER_WRITELN(("HTTP Server init failed"));
    }
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

void HTTPSRV_Init(void)
{
    stack_init();
    http_server_socket_init();

    /* Everything OK. Print server address */
    LOGGER_WRITENL();
    LOGGER_WRITELN(("HTTP Server configuration: "));
    LOGGER_WRITELN(("IPv4        : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_ipaddr)[0], ((u8_t *)&fsl_netif0_ipaddr)[1], ((u8_t *)&fsl_netif0_ipaddr)[2], ((u8_t *)&fsl_netif0_ipaddr)[3]));
    LOGGER_WRITELN(("Subnet mask : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_netmask)[0], ((u8_t *)&fsl_netif0_netmask)[1], ((u8_t *)&fsl_netif0_netmask)[2], ((u8_t *)&fsl_netif0_netmask)[3]));
    LOGGER_WRITELN(("Gateway     : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_gw)[0], ((u8_t *)&fsl_netif0_gw)[1], ((u8_t *)&fsl_netif0_gw)[2], ((u8_t *)&fsl_netif0_gw)[3]));
    LOGGER_WRITELN(("DNS         : %s", MDNS_HOSTNAME));
    LOGGER_WRITENL();
}
