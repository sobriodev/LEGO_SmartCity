#include "api.h"
#include "query_string.h"

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

	QS_Param_t params[2];
	uint8_t paramsFound = QS_Parse(param->query_string, params, 2);
	const QS_Param_t *home = QS_GetParam("home", params, paramsFound);
	const QS_Param_t *led = QS_GetParam("led", params, paramsFound);

	if (QS_PARAM_EXISTS(home) && QS_PARAM_EXISTS(led) && QS_PARAM_IS_INT(home) && QS_PARAM_IS_BOOL(led)) {
	    response.data = "{response: \"OK\"}";
	    response.data_length = strlen("{response: \"OK\"}");
	    response.content_length = response.data_length;
	} else {
	    response.data = "{response: \"ER\"}";
	    response.data_length = strlen("{response: \"ER\"}");
	    response.content_length = response.data_length;
	}

    HTTPSRV_cgi_write(&response);
    return response.content_length;
}
