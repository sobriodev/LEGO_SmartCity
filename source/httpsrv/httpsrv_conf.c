#include "httpsrv_conf.h"
#include "logger.h"

#include "settings.h"
#include "conv.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Server internal config */
static struct netif fslNetif;
static mem_range_t nonDmaMemory[] = HTTPSRV_NON_DMA_MEMORY_ARRAY;

/* FS data generated automatically */
extern const HTTPSRV_FS_DIR_ENTRY httpsrv_fs_data[];

/* Routes table */
extern const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_apiTable[];

/* IP, Netmask and Gateway addresses */
static ip4_addr_t fslNetifIpAddr;
static ip4_addr_t fslNetifNetmask;
static ip4_addr_t fslNetifGw;

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Callback function to generate TXT mDNS record for HTTP service */
static void HTTPSRV_Text(struct mdns_service *service, void *txt_userdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", 6);
}

/* Init lwIP stack */
static void HTTPSRV_StackInit(void)
{
    ethernetif_config_t fsl_enet_config0 = {
        .phyAddress = HTTPSRV_PHY_ADDRESS,
        .clockName  = HTTPSRV_CLOCK_NAME,
        .macAddress = HTTPSRV_MAC_ADDR,
        .non_dma_memory = nonDmaMemory,
    };

    tcpip_init(NULL, NULL);

    SETTINGS_t *settings = SETTINGS_GetInstance();

    IP4_ADDR(&fslNetifIpAddr,
    		CONV_IP4_ADDR0(settings->httpsrvIp),
			CONV_IP4_ADDR1(settings->httpsrvIp),
			CONV_IP4_ADDR2(settings->httpsrvIp),
			CONV_IP4_ADDR3(settings->httpsrvIp));

    IP4_ADDR(&fslNetifNetmask,
    		CONV_IP4_ADDR0(settings->httpsrvSm),
			CONV_IP4_ADDR1(settings->httpsrvSm),
			CONV_IP4_ADDR2(settings->httpsrvSm),
			CONV_IP4_ADDR3(settings->httpsrvSm));

    IP4_ADDR(&fslNetifGw,
    		CONV_IP4_ADDR0(settings->httpsrvGw),
			CONV_IP4_ADDR1(settings->httpsrvGw),
			CONV_IP4_ADDR2(settings->httpsrvGw),
			CONV_IP4_ADDR3(settings->httpsrvGw));

    netifapi_netif_add(&fslNetif, &fslNetifIpAddr, &fslNetifNetmask, &fslNetifGw, &fsl_enet_config0,
                       ethernetif0_init, tcpip_input);
    netifapi_netif_set_default(&fslNetif);
    netifapi_netif_set_up(&fslNetif);

    mdns_resp_init();
    mdns_resp_add_netif(&fslNetif, settings->httpsrvDnsName, 60);
    mdns_resp_add_service(&fslNetif, settings->httpsrvDnsName, "_http", DNSSD_PROTO_TCP, 80, 300, HTTPSRV_Text, NULL);
}

/* Init server */
static void HTTPSRV_SocketInit(void)
{
    HTTPSRV_PARAM_STRUCT params;
    uint32_t httpsrv_handle;

    /* Init Fs*/
    HTTPSRV_FS_init(httpsrv_fs_data);

    /* Init HTTPSRV parameters.*/
    memset(&params, 0, sizeof(params));
    params.root_dir = "";
    params.index_page = HTTPSRV_DEFAULT_PAGE;
    params.auth_table = NULL;
    params.cgi_lnk_tbl = HTTPSRV_apiTable;
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
    HTTPSRV_StackInit();
    HTTPSRV_SocketInit();

    /* Everything OK. Print server address */
    LOGGER_WRITENL();
    LOGGER_WRITELN(("HTTP Server configuration: "));
    LOGGER_WRITELN(("IPv4        : %u.%u.%u.%u", ((u8_t *)&fslNetifIpAddr)[0], ((u8_t *)&fslNetifIpAddr)[1], ((u8_t *)&fslNetifIpAddr)[2], ((u8_t *)&fslNetifIpAddr)[3]));
    LOGGER_WRITELN(("Subnet mask : %u.%u.%u.%u", ((u8_t *)&fslNetifNetmask)[0], ((u8_t *)&fslNetifNetmask)[1], ((u8_t *)&fslNetifNetmask)[2], ((u8_t *)&fslNetifNetmask)[3]));
    LOGGER_WRITELN(("Gateway     : %u.%u.%u.%u", ((u8_t *)&fslNetifGw)[0], ((u8_t *)&fslNetifGw)[1], ((u8_t *)&fslNetifGw)[2], ((u8_t *)&fslNetifGw)[3]));
    LOGGER_WRITELN(("DNS         : %s", SETTINGS_GetInstance()->httpsrvDnsName));
    LOGGER_WRITENL();
}
