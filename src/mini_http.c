#include <string.h>
#include <stdlib.h>
#ifndef NO_PRINT_FUNC
#include <stdio.h>
#endif // NO_PRINT_FUNC
#include "cassert.h"
#define SAL_IMPLEMENTATION
#include "mini_http.h"

#define TMP_BUFF_SIZE 48

enum{
    RQST_FIRST_LINE,
    RESP_FIRST_LINE,
    HOST,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    APIKEY,
    CONNECTION,
    HTTP_HEADER_VALUE_SIZE
};

static char *http_header_temp[] = {
    [RQST_FIRST_LINE] = "%s %s %s\r\n",
    [RESP_FIRST_LINE] = "%s %d %s\r\n", 
    [HOST] = "Host: %s\r\n",
    [CONTENT_TYPE] = "Content-Type: %s; charset=utf-8\r\n",
    [CONTENT_LENGTH] = "Content-Length: %d\r\n",
    [APIKEY] = "ApiKey: %s\r\n", 
    [CONNECTION] = "Connection: %s\r\n",
};
cassert(HTTP_HEADER_VALUE_SIZE == 7);

const char *HTTPReqStr[] = {
    [HTTP_RQST_GET] = "GET",
    [HTTP_RQST_POST] = "POST",
};
cassert(HTTP_RQST_SIZE == 2);

const char *HTTPVerStr[] = {
    [HTTP_VER_1_0] = "HTTP/1.0",
    [HTTP_VER_1_1] = "HTTP/1.1",
};
cassert(HTTP_VER_SIZE == 2);

#ifndef NO_PRINT_FUNC
static void mini_http_print_rqst(HTTPHeader *h){
    printf("%s %s %s\n", HTTPReqStr[h->request_type], h->target, HTTPVerStr[h->version]);
    if(strlen(h->content_type)) printf("Content-Type: %s\n", h->content_type);
    if(strlen(h->host)) printf("Host: %s\n", h->host);
    printf("Content-Length: %d\n", h->content_length);
    if(strlen(h->apikey)) printf("ApiKey: %s\n", h->apikey);
    printf("Connection: %s\n", h->connection ? "keep-alive": "close");
}

static void mini_http_print_resp(HTTPHeader *h) {
    printf("%s %d %s\n", HTTPVerStr[h->version], h->response_code,
	   h->status_msg);
    printf("Content-Type: %s\n", h->content_type);
    printf("Host: %s\n", h->host);
    printf("Content-Length: %u\n", h->content_length);
    printf("Connection: %s", h->connection ? "keep-alive": "close");
}

int32_t mini_http_print_header(HTTPHeader *h) {
    switch (h->type) {
    case MINI_HTTP_RESP:
	mini_http_print_resp(h);
	break;
    case MINI_HTTP_RQST:
	mini_http_print_rqst(h);
	break;
    default:
	return MINI_HTTP_BAD_ARG;
    }

    return MINI_HTTP_OK;
}
#endif // NO_PRINT_FUNC

HTTPHeader mini_http_rqst_init(bool connection, uint8_t version,
			       uint8_t request_type, uint32_t content_length,
			       char *content_type, char *host, char *target,
			       char *apikey) {
    HTTPHeader res = {0};
    res.type = MINI_HTTP_RQST;
    res.connection = connection;
    res.version = version;
    res.request_type = request_type;
    res.content_length = content_length;
    if(content_type != NULL) strcpy(res.content_type, content_type);
    if(host != NULL) strcpy(res.host, host);
    if(target != NULL) strcpy(res.target, target);
    if(apikey != NULL) strcpy(res.apikey, apikey);

    return res;
}

HTTPHeader mini_http_resp_init(bool connection, uint8_t version,
                               uint16_t response_code, uint32_t content_length,
                               char *content_type, char *host,
                               char *status_msg) {
    HTTPHeader res;
    res.type = MINI_HTTP_RESP;
    res.version = version;
    res.connection = connection;
    res.content_length = content_length;
    res.response_code = response_code;
    if(content_type != NULL) strncpy(res.content_type, content_type, CONTENT_TYPE_SIZE);
    if(host != NULL) strncpy(res.host, host, HOST_NAME_SIZE);
    if(status_msg != NULL) strncpy(res.status_msg, status_msg, STATUS_MSG_SIZE);

    return res;
}

static const char *mini_http_get_status_msg(HTTPRespCode code){
    char *msg = NULL;
    switch (code){
        case HTTP_CONTINUE:
            msg = "Continue";
            break; 
        case HTTP_OK:
            msg = "OK";
            break;
        case HTTP_BAD_REQUEST:
            msg = "Bad Request";
            break;
        case HTTP_NOT_FOUND:
            msg = "Not Found";
            break;
        default:
            msg = "Unknown Response";
            break;
    }
    cassert(HTTP_RESP_CODE_SIZE == 4);

    return msg;
}

sal_str mini_http_gen_header_str(HTTPHeader *h, sal_str str) {
    char tmp_buffer[128] = {0};

    switch (h->type) {
    case MINI_HTTP_RQST:
	sprintf(tmp_buffer, http_header_temp[RQST_FIRST_LINE],
		HTTPReqStr[h->request_type],
		h->target,
		HTTPVerStr[h->version]);
	str = sal_str_add(str, sal_tostr(tmp_buffer));
	if(strlen(h->apikey)) {
	    sprintf(tmp_buffer, http_header_temp[APIKEY], h->apikey);
	    str = sal_str_add(str, sal_tostr(tmp_buffer));
	}
        break;
    case MINI_HTTP_RESP:
	sprintf(tmp_buffer, http_header_temp[RESP_FIRST_LINE],
		HTTPVerStr[h->version],
		h->response_code,
		mini_http_get_status_msg(h->response_code));
	str = sal_str_add(str, sal_tostr(tmp_buffer));
	break;
    }

    sprintf(tmp_buffer, http_header_temp[CONNECTION], h->connection ? "keep-alive": "close");
    str = sal_str_add(str, sal_tostr(tmp_buffer));

        sprintf(tmp_buffer, http_header_temp[CONTENT_LENGTH], h->content_length);
    str = sal_str_add(str, sal_tostr(tmp_buffer));

    if(strlen(h->content_type)) {
	sprintf(tmp_buffer, http_header_temp[CONTENT_TYPE], h->content_type);
	str = sal_str_add(str, sal_tostr(tmp_buffer));
    }

    if(strlen(h->host)) {
	sprintf(tmp_buffer, http_header_temp[HOST], h->host);
	str = sal_str_add(str, sal_tostr(tmp_buffer));
    }

    return str;
}

static const sal_str host = {.begin = "host: ", .size = 6};
static const sal_str content_type = {.begin = "content-type: ", .size = 14};
static const sal_str content_length = {.begin = "content-length: ", .size = 16};
static const sal_str connection = {.begin = "connection: ", .size = 12};
static const sal_str apikey = {.begin = "apikey: ", .size = 8};
static const sal_str line_end = {.begin = "\r\n", .size = 2};
static const sal_str elem_sep = {.begin = " ", .size = 1};
static const sal_str get = {.begin = "get", .size = 3};
static const sal_str post = {.begin = "post", .size = 4};

HTTPHeader mini_http_parse_header(sal_str str) {
    HTTPHeader res = {0};
    sal_str elem = {0};
    sal_str line = sal_str_chop(str, line_end);
    sal_str line_chop = sal_str_save_chop();

    elem = sal_str_chop(line, elem_sep);
    // Detect Header type
    if((sal_str_isequal(sal_str_tolower(elem), get)) ||
       (sal_str_isequal(sal_str_tolower(elem), post))) {
	res.type = MINI_HTTP_RQST;
    } else {
	res.type = MINI_HTTP_RESP;
    }

    // parse first line
    switch (res.type) {
    case MINI_HTTP_RQST:
	// request type
	elem = sal_str_chop(line, elem_sep);
	if(sal_str_isequal(sal_str_tolower(elem), get))
	    res.request_type = HTTP_RQST_GET;
	else
	    res.request_type = HTTP_RQST_POST;

        // target
	elem = sal_str_chop(SAL_NULL_STR, elem_sep);
	sal_str_cpytobuffer(res.target, elem);

        //HTTP version
	elem = sal_str_chop(SAL_NULL_STR, elem_sep);
	if(sal_str_isequal(elem, sal_tostr("HTTP/1.0")))
	    res.version = HTTP_VER_1_0;
	else
	    res.version = HTTP_VER_1_1;
	break;
    case MINI_HTTP_RESP:
	// HTTP version
	elem = sal_str_chop(line, elem_sep);
	if(sal_str_isequal(elem, sal_tostr("HTTP/1.0")))
	    res.version = HTTP_VER_1_0;
	else
	    res.version = HTTP_VER_1_1;

	// response code
	elem = sal_str_chop(SAL_NULL_STR, elem_sep);
	res.response_code = sal_str_toint(elem);

        // status msg
	sal_str tmp = sal_tostr(res.status_msg);
        while(!sal_str_isnull(elem = sal_str_chop(SAL_NULL_STR, elem_sep))){
	    tmp = sal_str_add(tmp, elem);
	    tmp = sal_str_add(tmp, elem_sep);
	}
        break;
    }

    sal_str_load_chop(line_chop); // retrive line chop info
    line = sal_str_chop(SAL_NULL_STR, line_end);
    while(!sal_str_isnull(line)) {
	sal_str key = {0};

        // host
        if(!sal_str_isnull(key = sal_str_isinclude(sal_str_tolower(line), host))) {
	    line.begin += key.size;
	    line.size -= key.size;
	    sal_str_cpytobuffer(res.host, line);
	    continue;
	}

        // content type
	if(!sal_str_isnull(key = sal_str_isinclude(sal_str_tolower(line), content_type))) {
	    line.begin += key.size;
	    line.size -= key.size;
	    sal_str_cpytobuffer(res.content_type, line);
	    continue;
	}

	// content-length
	if(!sal_str_isnull(key = sal_str_isinclude(sal_str_tolower(line), content_length))) {
	    line.begin += key.size;
	    line.size -= key.size;
	    res.content_length = sal_str_toint(line);
	    continue;
	}

	// connection
	if(!sal_str_isnull(key = sal_str_isinclude(sal_str_tolower(line), connection))) {
	    line.begin += key.size;
	    line.size -= key.size;
	    if(sal_str_isequal(sal_str_tolower(line), sal_tostr("close")))
		res.connection = false;
	    else
		res.connection = true;
	    continue;
	}

	// apikey
	if(!sal_str_isnull(key = sal_str_isinclude(sal_str_tolower(line), apikey))) {
	    line.begin += key.size;
	    line.size -= key.size;
	    sal_str_cpytobuffer(res.apikey, line);
	    continue;
	}

        line = sal_str_chop(SAL_NULL_STR, line_end);
    }

    return res;
}

static const sal_str pair_delim = {.begin = "&", .size = 1};
static const sal_str keyval_delim = {.begin = "=", .size = 1};

// if you want to use dynamic memory use this init and deinit funcs
void mini_http_form_arr_init(FormElem *buff, uint32_t buff_size) {
    for(uint32_t i = 0; i < buff_size; ++i) {
	buff[i].key = sal_str_init(NULL);
	buff[i].value = sal_str_init(NULL);
    }
}

void mini_http_form_arr_deinit(FormElem *buff, uint32_t buff_size) {
    for(uint32_t i = 0; i < buff_size; ++i) {
	sal_str_deinit(buff[i].key);
	sal_str_deinit(buff[i].value);
    }
}
// -----------------------------------------------------------------

void mini_http_form_arr_clear(FormElem *buff, uint32_t buff_size) {
    for(uint32_t i = 0; i < buff_size; ++i) {
	buff[i].key.size = 0;
	buff[i].value.size = 0;
    }
}

int32_t mini_http_parse_form_data(sal_str data, FormElem *buff, size_t buff_size, FormElem **end_ptr){
    if(sal_str_isempty(data) || buff_size == 0 || end_ptr == NULL)
        return MINI_HTTP_BAD_ARG;

    sal_str pair = sal_str_chop(data, pair_delim);
    size_t i = 0;
    
    while (!sal_str_isempty(pair)){
	sal_str pair_chop = sal_str_save_chop();

	sal_str key = sal_str_chop(pair, keyval_delim);
	sal_str val = sal_str_chop(SAL_NULL_STR, keyval_delim);

	buff[i].key = sal_str_ncpy(buff[i].key, key, MAX_KEY_LEN);
	buff[i].value = sal_str_ncpy(buff[i].value, val, MAX_VALUE_LEN);
        
        i++;
        if(i == buff_size)
            break;
	sal_str_load_chop(pair_chop);
        pair = sal_str_chop(SAL_NULL_STR, pair_delim);
    }

    *end_ptr = buff + i;

    return MINI_HTTP_OK;
}

sal_str mini_http_gen_form_str(FormElem *data, FormElem *end_ptr, sal_str str){
    if(data == NULL || end_ptr == NULL)
        return SAL_NULL_STR; 

    do{
	str = sal_str_add(str, data->key);
	str = sal_str_add(str, keyval_delim);
	str = sal_str_add(str, data->value);
	if(data + 1 != end_ptr)
	    str = sal_str_add(str, pair_delim);
    }while (++data != end_ptr);

    return str;
}
