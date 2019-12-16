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

/* Post buffer length */
#define API_POST_BUFFER_MAX_LEN						800

/* JSON status */
#define API_JSON_STAT_MAX_LEN						200
#define API_JSON_STAT 								"{\"statusCode\": %d, \"message\": \"%s\"}"
#define API_MAKE_JSON_STAT(RES, CODE, MSG) 			{API_MakeJsonStatus(&(RES),(CODE),(MSG));HTTPSRV_cgi_write(&(RES));return (RES).content_length;}
#define API_MAKE_CJOSN_STAT(OBJ, RES, CODE, MSG)	{cJSON_Delete((OBJ));API_MAKE_JSON_STAT((RES),(CODE),(MSG));}

/* CJSON */
#define API_CJSON_APPEND_HOOK(CODE)					{if((CODE)==NULL){goto end;}}

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC VARIABLES ---------------------------- */
/* ----------------------------------------------------------------------------- */

/* Buffer used in post requests */
static char postBuffer[API_POST_BUFFER_MAX_LEN];

/* Api table */
const HTTPSRV_CGI_LINK_STRUCT HTTPSRV_ApiTable[] = {
	{ API_LIGHT_CONTROL_NAME, API_LightControl },
	{ API_ANIM_STATUS_NAME, API_AnimStatus },
	{ API_ANIM_DELAY_NAME, NULL },
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

/* Create JSON response */
static void API_MakeJsonResponse(HTTPSRV_CGI_RES_STRUCT *response, uint32_t statusCode, char *jsonStr)
{
	response->status_code = statusCode;
	response->content_type = HTTPSRV_CONTENT_TYPE_JSON;
	response->data = jsonStr;
	response->data_length = strlen(jsonStr);
	response->content_length = response->data_length;
}

/* Parse GET method of light control api function */
bool API_LightControlParseGet(char *qs, LEGO_SearchPattern_t *searchPattern, uint32_t *legoId)
{
	/* Parse query string */
	QS_Param_t params[2];
	uint8_t paramsFound = QS_Parse(qs, params, 2);
	const QS_Param_t *mode = QS_GetParam("mode", params, paramsFound);
	const QS_Param_t *id = QS_GetParam("id", params, paramsFound);

	if (!QS_PARAM_EXISTS(mode) || !QS_PARAM_EXISTS(id) || !QS_PARAM_IS_INT(id)) {
		return false;
	}

	*legoId = id->value.valueInt;

	/* Check whether supported search method is used */
	if (!strcmp(mode->value.valueStr, "single")) {
		*searchPattern = LEGO_SEARCH_ID;
	} else if (!strcmp(mode->value.valueStr, "group")) {
		*searchPattern = LEGO_SEARCH_GROUP;
	} else {
		return false;
	}

	return true;
}

/* Parse POST method of light control api function */
bool API_LightControlParsePost(HTTPSRV_CGI_REQ_STRUCT *request, LEGO_SearchPattern_t *searchPattern, uint32_t *legoId, LEGO_LightOp_t *op)
{
	bool success = true;

	/* Read POST data */
	uint32_t length = (request->content_length > API_POST_BUFFER_MAX_LEN) ? API_POST_BUFFER_MAX_LEN : request->content_length;
	uint32_t bytesRead = HTTPSRV_cgi_read(request->ses_handle, postBuffer, length);
	if (!bytesRead) {
		return false;
	}
	postBuffer[request->content_length] = '\0';

	/* Parse JSON */
	cJSON *json = cJSON_Parse(postBuffer);
	if (json == NULL) {
		success = false;
		goto end;
	}

	cJSON *id;
	cJSON *mode;
	cJSON *operation;

	/* Read values */
	id = cJSON_GetObjectItemCaseSensitive(json, "id");
	mode = cJSON_GetObjectItemCaseSensitive(json, "mode");
	operation = cJSON_GetObjectItemCaseSensitive(json, "operation");

	if (id == NULL || mode == NULL || operation == NULL || !cJSON_IsNumber(id) || !cJSON_IsString(mode) || !cJSON_IsString(operation)) {
		success = false;
		goto end;
	}

	/* Check whether supported search method is used */
	if (!strcmp(mode->valuestring, "single")) {
		*searchPattern = LEGO_SEARCH_ID;
	} else if (!strcmp(mode->valuestring, "group")) {
		*searchPattern = LEGO_SEARCH_GROUP;
	} else {
		success = false;
		goto end;
	}

	/* Check whether supported operation is used */
	if (!strcmp(operation->valuestring, "on")) {
		*op = LEGO_LIGHT_ON;
	} else if (!strcmp(operation->valuestring, "off")) {
		*op = LEGO_LIGHT_ON;
	} else if (!strcmp(operation->valuestring, "toggle")) {
		*op = LEGO_LIGHT_TOGGLE;
	}
	else {
		success = false;
		goto end;
	}

	*legoId = id->valueint;

	end:
	cJSON_Delete(json);
	return success;
}

/* Create JSON when using GET method of control api function */
char *API_LightControlCreateJsonGet(const LEGO_LightStatus_t *status, uint8_t len)
{
	char *outputStr = NULL;
	cJSON *json = cJSON_CreateObject();
	API_CJSON_APPEND_HOOK(json);

	/* Add status field */
	API_CJSON_APPEND_HOOK(cJSON_AddNumberToObject(json, "status", HTTPSRV_CODE_OK));

	/* Add status array */
	cJSON *statusArray = cJSON_AddArrayToObject(json, "states");
	API_CJSON_APPEND_HOOK(statusArray);

	for (uint8_t i = 0; i < len; i++) {
		cJSON *row = cJSON_CreateObject();
		API_CJSON_APPEND_HOOK(row);

		/* Id */
		API_CJSON_APPEND_HOOK(cJSON_AddNumberToObject(row, "id", status[i].lightId));
		/* State */
		API_CJSON_APPEND_HOOK(cJSON_AddNumberToObject(row, "state", status[i].state));

		cJSON_AddItemToArray(statusArray, row);
	}

	outputStr = cJSON_Print(json);

	end:
	return outputStr;
	cJSON_Delete(json);
}

/* Parse GET method of animation status api function */
bool API_AnimStatusParseGet(char *qs, LEGO_Anim_t *animId)
{
	/* Parse query string */
	QS_Param_t params[1];
	uint8_t paramsFound = QS_Parse(qs, params, 1);
	const QS_Param_t *id = QS_GetParam("id", params, paramsFound);

	if (!QS_PARAM_EXISTS(id) || !QS_PARAM_IS_INT(id)) {
		return false;
	}

	*animId = id->value.valueInt;
	return true;
}

/* Create JSON when using GET method of animation status api function */
char *API_AnimStatusMakeJsonGet(const LEGO_AnimInfo_t *animInfo)
{
	char *outputStr = NULL;
	cJSON *json = cJSON_CreateObject();
	API_CJSON_APPEND_HOOK(json);

	/* Add status field */
	API_CJSON_APPEND_HOOK(cJSON_AddNumberToObject(json, "status", HTTPSRV_CODE_OK));

	/* Add animation status */
	API_CJSON_APPEND_HOOK(cJSON_AddNumberToObject(json, "animStatus", animInfo->onOff));

	outputStr = cJSON_Print(json);

	end:
	cJSON_Delete(json);
	return outputStr;
}

/* Parse POST method of animation status api function */
bool API_AnimStatusParsePost(HTTPSRV_CGI_REQ_STRUCT *request, LEGO_Anim_t *animId, bool *onOff)
{
	bool success = true;

	/* Read POST data */
	uint32_t length = (request->content_length > API_POST_BUFFER_MAX_LEN) ? API_POST_BUFFER_MAX_LEN : request->content_length;
	uint32_t bytesRead = HTTPSRV_cgi_read(request->ses_handle, postBuffer, length);
	if (!bytesRead) {
		return false;
	}
	postBuffer[request->content_length] = '\0';

	/* Parse JSON */
	cJSON *json = cJSON_Parse(postBuffer);
	if (json == NULL) {
		success = false;
		goto end;
	}

	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	cJSON *op = cJSON_GetObjectItemCaseSensitive(json, "animStatus");

	if (id == NULL || op == NULL || !cJSON_IsNumber(id) || !cJSON_IsNumber(op)) {
		success = false;
		goto end;
	}

	*animId = id->valueint;
	*onOff = op->valueint;

	end:
	cJSON_Delete(json);
	return success;
}

/* ----------------------------------------------------------------------------- */
/* -------------------------------- API FUNCTIONS ------------------------------ */
/* ----------------------------------------------------------------------------- */

/* Light control */
int32_t API_LightControl(HTTPSRV_CGI_REQ_STRUCT *request)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle = request->ses_handle;

    switch (request->request_method) {
		/* GET request - return light state */
		case HTTPSRV_REQ_GET: {

			/* Parse qs */
			LEGO_SearchPattern_t searchPattern;
			uint32_t id;
			if (!API_LightControlParseGet(request->query_string, &searchPattern, &id)) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Missing/invalid query string parameters");
			}

			/* Get results */
			LEGO_LightStatus_t *status;
			uint8_t lightsFound;
			LEGO_LightOpRes_t res = LEGO_GetLightsStatus(searchPattern, id, &status, &lightsFound);

		    char *jsonOutput;
			switch (res) {
			case LEGO_I2C_ERR:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
				break;
			case LEGO_ID_NOT_FOUND:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_NOT_FOUND, "Requested ID not found");
				break;
			case LEGO_OP_PERFORMED:
				jsonOutput = API_LightControlCreateJsonGet(status, lightsFound);
				if (jsonOutput == NULL) {
					API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
				} else {
					/* Write response and free cJSON allocated string */
					API_MakeJsonResponse(&response, 200, jsonOutput);
					HTTPSRV_cgi_write(&response);
					free(jsonOutput);
					return response.content_length;
				}
				break;
			default:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}

			break;
		}
		/* POST request - set light state */
		case HTTPSRV_REQ_POST: {

			/* Only JSON body is supported */
			if (request->content_type != HTTPSRV_CONTENT_TYPE_JSON) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_UNSUPPORTED_MEDIA, "Only JSON content type is supported");
			}

			/* Parse JSON body */
			LEGO_SearchPattern_t searchPattern;
			uint32_t legoId;
			LEGO_LightOp_t op;
			if (!API_LightControlParsePost(request, &searchPattern, &legoId, &op)) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "JSON parsing error. Check fields correctness");
			}

			/* Perform operation */
			LEGO_LightOpRes_t res = LEGO_LightsControl(searchPattern, legoId, op);

			/* Return status */
			switch (res) {
			case LEGO_I2C_ERR:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
				break;
			case LEGO_ID_NOT_FOUND:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_NOT_FOUND, "Requested ID not found");
				break;
			case LEGO_OP_PERFORMED:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_OK, "Success");
				break;
			default:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}

			break;
		}
		default:
			API_MAKE_JSON_STAT(response, HTTPSRV_CODE_METHOD_NOT_ALLOWED, "Only GET and POST methods are supported here");
    }
}

int32_t API_AnimStatus(HTTPSRV_CGI_REQ_STRUCT *request)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle = request->ses_handle;

    switch (request->request_method) {
    	/* GET request - return animation status */
		case HTTPSRV_REQ_GET: {

			/* Parse query string */
			LEGO_Anim_t animId;
			if (!API_AnimStatusParseGet(request->query_string, &animId)) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "Missing/invalid query string parameters");
			}

			/* Perform operation */
			const LEGO_AnimInfo_t *animInfo;
			LEGO_LightOpRes_t res = LEGO_GetAnimInfo(animId, &animInfo);

			char *jsonOutput;
			switch (res) {
			case LEGO_ID_NOT_FOUND:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_NOT_FOUND, "Requested ID not found");
				break;
			case LEGO_OP_PERFORMED:
				jsonOutput = API_AnimStatusMakeJsonGet(animInfo);
				if (jsonOutput == NULL) {
					API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
				} else {
					/* Write response and free cJSON allocated string */
					API_MakeJsonResponse(&response, 200, jsonOutput);
					HTTPSRV_cgi_write(&response);
					free(jsonOutput);
					return response.content_length;
				}
				break;
			default:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}

			break;
		}
		/* POST request - set animation status */
		case HTTPSRV_REQ_POST: {

			/* Only JSON body is supported */
			if (request->content_type != HTTPSRV_CONTENT_TYPE_JSON) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_UNSUPPORTED_MEDIA, "Only JSON content type is supported");
			}

			/* Parse JSON body */
			LEGO_Anim_t animId;
			bool onOff;
			if (!API_AnimStatusParsePost(request, &animId, &onOff)) {
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_BAD_REQ, "JSON parsing error. Check fields correctness");
			}

			/* Perform operation */
			LEGO_LightOpRes_t res = LEGO_AnimControl(animId, onOff);

			switch (res) {
			case LEGO_ID_NOT_FOUND:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_NOT_FOUND, "Requested ID not found");
				break;
			case LEGO_OP_PERFORMED:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_OK, "Success");
				break;
			default:
				API_MAKE_JSON_STAT(response, HTTPSRV_CODE_INTERNAL_ERROR, "Internal server error");
			}

			break;
		}
		default:
			API_MAKE_JSON_STAT(response, HTTPSRV_CODE_METHOD_NOT_ALLOWED, "Only GET method is supported here");
	}
}
