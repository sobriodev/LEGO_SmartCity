#include "board_conf.h"
#include "gui_conf.h"
#include "rtos.h"

/* Logger */
#include "logger.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

#include "lwip/opt.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

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
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

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
#define configMAC_ADDR                     \
    {                                      \
        0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
    }

/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS

/* System clock name. */
#define EXAMPLE_CLOCK_NAME kCLOCK_CoreSysClk

/* Memory not usable by ENET DMA. */
#define NON_DMA_MEMORY_ARRAY \
    {                        \
        {0x0U, 0x80000U},    \
        {                    \
            0x0U, 0x0U       \
        }                    \
    }


#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_ON
#endif
#ifndef HTTPD_STACKSIZE
#define HTTPD_STACKSIZE DEFAULT_THREAD_STACKSIZE
#endif
#ifndef HTTPD_PRIORITY
#define HTTPD_PRIORITY DEFAULT_THREAD_PRIO
#endif
#ifndef DEBUG_WS
#define DEBUG_WS 0
#endif

#define CGI_DATA_LENGTH_MAX (96)

#define MDNS_HOSTNAME "lwip-http"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void cgi_urldecode(char *url);
static int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT *param);
static int cgi_example(HTTPSRV_CGI_REQ_STRUCT *param);
static int ssi_date_time(HTTPSRV_SSI_PARAM_STRUCT *param);
static bool cgi_get_varval(char *var_str, char *var_name, char *var_val, uint32_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static struct netif fsl_netif0;
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
static mem_range_t non_dma_memory[] = NON_DMA_MEMORY_ARRAY;
#endif /* FSL_FEATURE_SOC_LPC_ENET_COUNT */
/* FS data.*/
extern const HTTPSRV_FS_DIR_ENTRY httpsrv_fs_data[];

/*
 * Authentication users
 */
static const HTTPSRV_AUTH_USER_STRUCT users[] = {
    {"admin", "admin"}, {NULL, NULL} /* Array terminator */
};

/*
 * Authentication information.
 */
static const HTTPSRV_AUTH_REALM_STRUCT auth_realms[] = {
    {"Please use uid:admin pass:admin to login", "/auth.html", HTTPSRV_AUTH_BASIC, users},
    {NULL, NULL, HTTPSRV_AUTH_INVALID, NULL} /* Array terminator */
};

char cgi_data[CGI_DATA_LENGTH_MAX + 1];

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    {"rtcdata", cgi_rtc_data},
    {"get", cgi_example},
    {"post", cgi_example},
    {0, 0} // DO NOT REMOVE - last item - end of table
};

const HTTPSRV_SSI_LINK_STRUCT ssi_lnk_tbl[] = {{"date_time", ssi_date_time}, {0, 0}};

/*******************************************************************************
 * Code
 ******************************************************************************/
static int cgi_rtc_data(HTTPSRV_CGI_REQ_STRUCT *param)
{
#define BUFF_SIZE sizeof("00\n00\n00\n")
    HTTPSRV_CGI_RES_STRUCT response;
    uint32_t time;
    uint32_t min;
    uint32_t hour;
    uint32_t sec;

    char str[BUFF_SIZE];
    uint32_t length = 0;

    if (param->request_method != HTTPSRV_REQ_GET)
    {
        return (0);
    }

    time = sys_now();

    sec  = time / 1000;
    min  = sec / 60;
    hour = min / 60;
    min %= 60;
    sec %= 60;

    response.ses_handle   = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code  = HTTPSRV_CODE_OK;
    /*
    ** When the keep-alive is used we have to calculate a correct content length
    ** so the receiver knows when to ACK the data and continue with a next request.
    ** Please see RFC2616 section 4.4 for further details.
    */

    /* Calculate content length while saving it to buffer */
    length                  = snprintf(str, BUFF_SIZE, "%ld\n%ld\n%ld\n", hour, min, sec);
    response.data           = str;
    response.data_length    = length;
    response.content_length = response.data_length;
    /* Send response */
    HTTPSRV_cgi_write(&response);
    return (response.content_length);
}

/* Example Common Gateway Interface callback. */
static int cgi_example(HTTPSRV_CGI_REQ_STRUCT *param)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};

    response.ses_handle  = param->ses_handle;
    response.status_code = HTTPSRV_CODE_OK;

    if (param->request_method == HTTPSRV_REQ_GET)
    {
        char *c;

        /* Replace '+' with spaces. */
        while ((c = strchr(cgi_data, '+')) != NULL)
        {
            *c = ' ';
        }
        response.content_type   = HTTPSRV_CONTENT_TYPE_PLAIN;
        response.data           = cgi_data;
        response.data_length    = strlen(cgi_data);
        response.content_length = response.data_length;
        HTTPSRV_cgi_write(&response);
    }
    else if (param->request_method == HTTPSRV_REQ_POST)
    {
        uint32_t length = 0;
        uint32_t read;
        char buffer[sizeof("post_input = ") + CGI_DATA_LENGTH_MAX] = {0};

        length = param->content_length;
        read   = HTTPSRV_cgi_read(param->ses_handle, buffer, (length > sizeof(buffer)) ? sizeof(buffer) : length);

        if (read > 0)
        {
            cgi_get_varval(buffer, "post_input", cgi_data, sizeof(cgi_data));
            cgi_urldecode(cgi_data);
        }

        /* Write the response using chunked transmission coding. */
        response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
        /* Set content length to -1 to indicate unknown content length. */
        response.content_length = -1;
        response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
        response.data_length    = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data        = "<html><head><title>POST successfull!</title>";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data        = "<meta http-equiv=\"refresh\" content=\"0; url=cgi.html\"></head><body></body></html>";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data_length = 0;
        HTTPSRV_cgi_write(&response);
    }

    return (response.content_length);
}

static bool cgi_get_varval(char *src, char *var_name, char *dst, uint32_t length)
{
    char *name;
    bool result;
    uint32_t index;
    uint32_t n_length;

    result = false;
    dst[0] = 0;
    name   = src;

    n_length = strlen(var_name);

    while ((name != NULL) && ((name = strstr(name, var_name)) != NULL))
    {
        if (name[n_length] == '=')
        {
            name += n_length + 1;

            index = strcspn(name, "&");
            if (index >= length)
            {
                index = length - 1;
            }
            strncpy(dst, name, index);
            dst[index] = '\0';
            result     = true;
            break;
        }
        else
        {
            name = strchr(name, '&');
        }
    }

    return (result);
}

/* Example Server Side Include callback. */
static int ssi_date_time(HTTPSRV_SSI_PARAM_STRUCT *param)
{
    if (strcmp(param->com_param, "time") == 0)
    {
        HTTPSRV_ssi_write(param->ses_handle, __TIME__, strlen(__TIME__));
    }
    else if (strcmp(param->com_param, "date") == 0)
    {
        HTTPSRV_ssi_write(param->ses_handle, __DATE__, strlen(__DATE__));
    }
    return (0);
}

/* Decode URL encoded string in place. */
static void cgi_urldecode(char *url)
{
    char *src = url;
    char *dst = url;

    while (*src != '\0')
    {
        if ((*src == '%') && (isxdigit((unsigned char)*(src + 1))) && (isxdigit((unsigned char)*(src + 2))))
        {
            *src       = *(src + 1);
            *(src + 1) = *(src + 2);
            *(src + 2) = '\0';
            *dst++     = strtol(src, NULL, 16);
            src += 3;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

#if HTTPSRV_CFG_WEBSOCKET_ENABLED
/*
 * Echo plugin code - simple plugin which echoes any message it receives back to
 * client.
 */
uint32_t ws_echo_connect(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket echo client connected.\r\n");
#endif
    return (0);
}

uint32_t ws_echo_disconnect(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket echo client disconnected.\r\n");
#endif
    return (0);
}

uint32_t ws_echo_message(void *param, WS_USER_CONTEXT_STRUCT context)
{
    WS_send(&context); /* Send back what was received.*/
#if DEBUG_WS
    if (context.data.type == WS_DATA_TEXT)
    {
        /* Print received text message to console. */
        context.data.data_ptr[context.data.length] = 0;
        PRINTF("WebSocket message received:\r\n%s\r\n", context.data.data_ptr);
    }
    else
    {
        /* Inform user about binary message. */
        PRINTF("WebSocket binary data with length of %d bytes received.", context.data.length);
    }
#endif

    return (0);
}

uint32_t ws_echo_error(void *param, WS_USER_CONTEXT_STRUCT context)
{
#if DEBUG_WS
    PRINTF("WebSocket error: 0x%X.\r\n", context.error);
#endif
    return (0);
}

WS_PLUGIN_STRUCT ws_tbl[] = {{"/echo", ws_echo_connect, ws_echo_message, ws_echo_error, ws_echo_disconnect, NULL},
                             {0, 0, 0, 0, 0, 0}};
#endif /* HTTPSRV_CFG_WEBSOCKET_ENABLED */

/*!
 * @brief Callback function to generate TXT mDNS record for HTTP service.
 */
static void http_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    mdns_resp_add_service_txtitem(service, "path=/", 6);
}

/*!
 * @brief Initializes lwIP stack.
 */
static void stack_init(void)
{
    ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
    ethernetif_config_t fsl_enet_config0 = {
        .phyAddress = EXAMPLE_PHY_ADDRESS,
        .clockName  = EXAMPLE_CLOCK_NAME,
        .macAddress = configMAC_ADDR,
#if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
        .non_dma_memory = non_dma_memory,
#endif /* FSL_FEATURE_SOC_LPC_ENET_COUNT */
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

    LWIP_PLATFORM_DIAG(("\r\n************************************************"));
    LWIP_PLATFORM_DIAG((" HTTP Server example"));
    LWIP_PLATFORM_DIAG(("************************************************"));
    LWIP_PLATFORM_DIAG((" IPv4 Address     : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_ipaddr)[0],
                        ((u8_t *)&fsl_netif0_ipaddr)[1], ((u8_t *)&fsl_netif0_ipaddr)[2],
                        ((u8_t *)&fsl_netif0_ipaddr)[3]));
    LWIP_PLATFORM_DIAG((" IPv4 Subnet mask : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_netmask)[0],
                        ((u8_t *)&fsl_netif0_netmask)[1], ((u8_t *)&fsl_netif0_netmask)[2],
                        ((u8_t *)&fsl_netif0_netmask)[3]));
    LWIP_PLATFORM_DIAG((" IPv4 Gateway     : %u.%u.%u.%u", ((u8_t *)&fsl_netif0_gw)[0], ((u8_t *)&fsl_netif0_gw)[1],
                        ((u8_t *)&fsl_netif0_gw)[2], ((u8_t *)&fsl_netif0_gw)[3]));
    LWIP_PLATFORM_DIAG((" mDNS hostname    : %s", MDNS_HOSTNAME));
    LWIP_PLATFORM_DIAG(("************************************************"));
}

/*!
 * @brief Initializes server.
 */
static void http_server_socket_init(void)
{
    HTTPSRV_PARAM_STRUCT params;
    uint32_t httpsrv_handle;

    /* Init Fs*/
    HTTPSRV_FS_init(httpsrv_fs_data);

    /* Init HTTPSRV parameters.*/
    memset(&params, 0, sizeof(params));
    params.root_dir    = "";
    params.index_page  = "/index.html";
    params.auth_table  = auth_realms;
    params.cgi_lnk_tbl = cgi_lnk_tbl;
    params.ssi_lnk_tbl = ssi_lnk_tbl;
#if HTTPSRV_CFG_WEBSOCKET_ENABLED
    params.ws_tbl = ws_tbl;
#endif

    /* Init HTTP Server.*/
    httpsrv_handle = HTTPSRV_init(&params);
    if (httpsrv_handle == 0)
    {
        LWIP_PLATFORM_DIAG(("HTTPSRV_init() is Failed"));
    }
}

int main(void)
{
	/* Board initialization */
	BOARD_Init();

    /* Set the backlight PWM. */
    BOARD_InitBacklightPWM();

    /* Start emWin library */
    GUI_Start();

    stack_init();
    http_server_socket_init();

    if (!RTOS_TasksCreate() || !RTOS_TimersCreate()) {
    	LOGGER_WRITELN(CRITICAL_LOG, LOG_ERROR, "FreeRTOS tasks creation failed. The program will not start!");
    	while (1) {}
    }

    LOGGER_WRITELN(CRITICAL_LOG, LOG_INFO, "Program initialization succeed. Starting...");

    /* FreeRTOS entry point */
    vTaskStartScheduler();

    /* The program should never reach this line */
    while (1) {}
}
