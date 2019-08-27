#include "httpsrv_api.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[] = {
    {"test", API_TestConn},
    {0, 0} /* Do not remove */
};

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

int32_t API_TestConn(HTTPSRV_CGI_REQ_STRUCT *param)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};

    response.ses_handle = param->ses_handle;
    response.status_code = HTTPSRV_CODE_OK;

    response.content_type = HTTPSRV_CONTENT_TYPE_JSON;
    response.data = "{response: \"OK\"}";
    response.data_length = strlen("{response: \"OK\"}");
    response.content_length = response.data_length;

    HTTPSRV_cgi_write(&response);
    return response.content_length;
}
