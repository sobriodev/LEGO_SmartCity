#include "api.h"
#include "query_string.h"
#include "cJSON.h"

#include "fsl_debug_console.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* JSON error */
#define HTTPSRV_JSON_ERR_MAX_LEN	200
#define HTTPSRV_JSON_ERR 			"{\"statusCode\": %d, \"message\": \"%s\"}"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[] = {
    {"test", API_TestConn},
    {0, 0} /* Do not remove */
};

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Return specific HTTP error code */
static void HTTPSRV_MakeJsonError(HTTPSRV_CGI_RES_STRUCT *response, uint32_t statusCode, const char *msg)
{
	/* cJSON is not used here for better performance */
	static char jsonResponse[HTTPSRV_JSON_ERR_MAX_LEN];
	snprintf(jsonResponse, HTTPSRV_JSON_ERR_MAX_LEN, HTTPSRV_JSON_ERR, statusCode, msg);

	response->status_code = statusCode;
	response->content_type = HTTPSRV_CONTENT_TYPE_JSON;
	response->data = jsonResponse;
	response->data_length = strlen(jsonResponse);
	response->content_length = response->data_length;
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

int32_t API_TestConn(HTTPSRV_CGI_REQ_STRUCT *param)
{
	HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle = param->ses_handle;

	QS_Param_t params[2];
	uint8_t paramsFound = QS_Parse(param->query_string, params, 2);
	const QS_Param_t *home = QS_GetParam("home", params, paramsFound);
	const QS_Param_t *led = QS_GetParam("led", params, paramsFound);

	if (QS_PARAM_EXISTS(home) && QS_PARAM_EXISTS(led) && QS_PARAM_IS_INT(home) && QS_PARAM_IS_BOOL(led)) {
		response.status_code = 200;
		response.content_type = HTTPSRV_CONTENT_TYPE_JSON;
	    response.data = "{\"response\": \"OK\"}";
	    response.data_length = strlen("{\"response\": \"OK\"}");
	    response.content_length = response.data_length;
	} else {
		HTTPSRV_MakeJsonError(&response, 404, "LED not found!");
	}

    HTTPSRV_cgi_write(&response);
    return response.content_length;
}
