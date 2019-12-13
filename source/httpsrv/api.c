#include "api.h"
#include "query_string.h"
#include "stdlib.h"
#include "string.h"

/* cJSON */
#include "cJSON.h"

/* LEGO */
#include "lego.h"

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE MACROS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* JSON status */
#define API_JSON_STAT_MAX_LEN						200
#define API_JSON_STAT 								"{\"statusCode\": %d, \"message\": \"%s\"}"
#define API_MAKE_JSON_STAT(RES, CODE, MSG) 			{API_MakeJsonStatus(&(RES),(CODE),(MSG));HTTPSRV_cgi_write(&(RES));return (RES).content_length;}
#define API_MAKE_CJOSN_STAT(OBJ, RES, CODE, MSG)	{cJSON_Delete((OBJ));API_MAKE_JSON_STAT((RES),(CODE),(MSG));}

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[] = {
    {"test", API_TestConn},
	{"lightControl", API_LightControl},
	{"animControl", API_AnimControl},
    {0, 0} /* Do not remove */
};

/* ----------------------------------------------------------------------------- */
/* ------------------------------ PRIVATE FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Return specific HTTP code */
static void API_MakeJsonStatus(HTTPSRV_CGI_RES_STRUCT *response, uint32_t statusCode, const char *msg)
{
	/* cJSON is not used here for better performance */
	static char jsonResponse[API_JSON_STAT_MAX_LEN];
	snprintf(jsonResponse, API_JSON_STAT_MAX_LEN, API_JSON_STAT, statusCode, msg);

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
		response.status_code = HTTPSRV_CODE_OK;
		response.content_type = HTTPSRV_CONTENT_TYPE_JSON;
	    response.data = "{\"response\": \"OK\"}";
	    response.data_length = strlen("{\"response\": \"OK\"}");
	    response.content_length = response.data_length;
	} else {
		API_MakeJsonStatus(&response, HTTPSRV_CODE_BAD_REQ, "Query param missing");
	}

    HTTPSRV_cgi_write(&response);
    return response.content_length;
}

int32_t API_LightControl(HTTPSRV_CGI_REQ_STRUCT *param)
{
	HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle = param->ses_handle;

    QS_Param_t params[3];
    uint8_t paramsFound = QS_Parse(param->query_string, params, 3);
    const QS_Param_t *id = QS_GetParam("id", params, paramsFound);
    const QS_Param_t *mode = QS_GetParam("mode", params, paramsFound);
    const QS_Param_t *op = QS_GetParam("op", params, paramsFound);

    if (!QS_PARAM_EXISTS(id) || !QS_PARAM_EXISTS(mode) || !QS_PARAM_EXISTS(op) || !QS_PARAM_IS_INT(id)) {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "QS param missing");
    }

    LEGO_LightOp_t oper;
    if (!strcmp(op->value.valueStr, "on")) {
    	oper = 0;
    } else if (!strcmp(op->value.valueStr, "off")) {
    	oper = 1;
    } else if (!strcmp(op->value.valueStr, "toggle")) {
    	oper = 2;
    } else {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Unknown light operation");
    }

    LEGO_LightOpRes_t res;
    if (!strcmp(mode->value.valueStr, "group")) {
    	res = LEGO_LightsControl(LEGO_SEARCH_GROUP, id->value.valueInt, oper);
    } else if (!strcmp(mode->value.valueStr, "light")) {
    	res = LEGO_LightsControl(LEGO_SEARCH_ID, id->value.valueInt, oper);
    } else {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Unknown control mode");
    }

    if (res != LEGO_OP_PERFORMED) {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal error");
    }
    API_MAKE_JSON_STAT(response, HTTPSRV_CODE_OK, "Success");
}

int32_t API_AnimControl(HTTPSRV_CGI_REQ_STRUCT *param)
{
	HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle = param->ses_handle;

    QS_Param_t params[2];
    uint8_t paramsFound = QS_Parse(param->query_string, params, 2);
    const QS_Param_t *id = QS_GetParam("id", params, paramsFound);
    const QS_Param_t *state = QS_GetParam("state", params, paramsFound);

    if (!QS_PARAM_EXISTS(id) || !QS_PARAM_EXISTS(state) || !QS_PARAM_IS_INT(id)) {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "QS param missing");
    }

    if (id->value.valueInt < 0 && id->value.valueInt > 2) {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Unknown ID");
    }

    if (!strcmp(state->value.valueStr, "on")) {
    	LEGO_AnimControl(id->value.valueInt, 0);
    } else if (!strcmp(state->value.valueStr, "off")) {
    	LEGO_AnimControl(id->value.valueInt, 1);
    } else {
    	API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Unknown state");
    }

    API_MAKE_JSON_STAT(response, HTTPSRV_CODE_OK, "Success");
}
