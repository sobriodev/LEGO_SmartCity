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

/* POST data */
#define API_POST_DATA_MAX_LEN				2000

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[] = {
    {"test", API_TestConn},
	{"lightControl", API_LightControl},
    {0, 0} /* Do not remove */
};

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PRIVATE VARIABLES --------------------------- */
/* ----------------------------------------------------------------------------- */

/* POST read buffer */
static char postBuffer[API_POST_DATA_MAX_LEN];

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

    switch (param->request_method) {
		case HTTPSRV_REQ_GET: {
			API_MAKE_JSON_STAT(response, HTTPSRV_CODE_METHOD_NOT_ALLOWED, "Unsupported HTTP method");
			break;
		}
		case HTTPSRV_REQ_POST: {
			/* Only JSON is supported here */
			if (param->content_type != HTTPSRV_CONTENT_TYPE_JSON) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_METHOD_NOT_ALLOWED, "Only JSON content type is supported");
			}

			/* Read POST data */
			postBuffer[param->content_length] = '\0';
			HTTPSRV_cgi_read(param->ses_handle, postBuffer, (param->content_length > API_POST_DATA_MAX_LEN) ? API_POST_DATA_MAX_LEN : param->content_length);

			cJSON *monitor = cJSON_Parse(postBuffer);
			if (monitor == NULL) {
				API_MAKE_CJOSN_STAT(monitor, response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}

			/* Get light id */
			cJSON *lightId = cJSON_GetObjectItemCaseSensitive(monitor, "lightId");
			cJSON *lightStat = cJSON_GetObjectItemCaseSensitive(monitor, "lightStatus");
			if (!cJSON_IsNumber(lightId) || !cJSON_IsString(lightStat) || lightStat->valuestring == NULL) {
				API_MAKE_CJOSN_STAT(monitor, response, HTTPSRV_CODE_BAD_REQ, "JSON field missing");
			}

			/* Perform desired operation */
			bool res = false;
			if (!strcmp(lightStat->valuestring, "on")) {
				//res = LEGO_LightControl(light, LEGO_LIGHT_ON);
				//res = LEGO_GroupControl(lightId->valueint, LEGO_LIGHT_ON);
			} else if (!strcmp(lightStat->valuestring, "off")) {
				//res = LEGO_LightControl(light, LEGO_LIGHT_OFF);
				//res = LEGO_GroupControl(lightId->valueint, LEGO_LIGHT_OFF);
			} else if (!strcmp(lightStat->valuestring, "toggle")) {
				//res = LEGO_LightControl(light, LEGO_LIGHT_TOGGLE);
				//res = LEGO_GroupControl(lightId->valueint, LEGO_LIGHT_TOGGLE);
			} else {
				API_MAKE_CJOSN_STAT(monitor, response, HTTPSRV_CODE_BAD_REQ, "Unknown light operation");
			}

			if (!res) {
				API_MAKE_CJOSN_STAT(monitor, response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}
			API_MAKE_CJOSN_STAT(monitor, response, HTTPSRV_CODE_OK, "Light status updated successfully");

			break;
		}
		default:
			API_MAKE_JSON_STAT(response, HTTPSRV_CODE_METHOD_NOT_ALLOWED, "Unsupported HTTP method");
    }
}
