#ifndef __MINI_HTTP_H__
#define __MINI_HTTP_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "sal_str.h"

#define STATUS_MSG_SIZE 64
#define CONTENT_TYPE_SIZE 48
#define HOST_NAME_SIZE 64
#define TARGET_SIZE 32
#define APIKEY_SIZE 32
#define MAX_FORM_ELEMENT_LEN 16
#define MAX_KEY_LEN 32
#define MAX_VALUE_LEN 32
#define MAX_HTTP_HEADER_LEN 4096

enum{
    MINI_HTTP_BAD_ARG = -2,
    MINI_HTTP_FAIL,
    MINI_HTTP_OK
};

typedef enum{
    HTTP_RQST_GET,
    HTTP_RQST_POST,
    HTTP_RQST_SIZE,
}HTTPRqstType;

typedef enum{
    HTTP_CONTINUE = 100,
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_RESP_CODE_SIZE = 4, // Update this while adding new value
}HTTPRespCode;

typedef enum {
    HTTP_VER_1_0,
    HTTP_VER_1_1,
    HTTP_VER_SIZE,
} HTTPVersion;

enum {
    MINI_HTTP_RQST,
    MINI_HTTP_RESP,
};

typedef union HTTPHeader {
    struct { // request response common parts
	uint32_t type : 1; // -> response/request
	uint32_t connection : 1; // -> connection close/keep-alive
	uint32_t version : 1; // -> HTTP/1.0 / HTTP/1.1
	uint32_t : 29;
	uint32_t content_length;
	char content_type[CONTENT_TYPE_SIZE];
	char host[HOST_NAME_SIZE];
    };

    struct { // for response specific
	uint32_t : 3;
	uint32_t response_code : 10;
	uint32_t : 19;
	uint32_t : 32;
	char __UNNAMED66__[CONTENT_TYPE_SIZE + HOST_NAME_SIZE];
	char status_msg[STATUS_MSG_SIZE];
    };

    struct { // for request specific
        uint32_t :3;
	uint32_t request_type:1;
	uint32_t :28;
	uint32_t :32;
	char __UNNAMED75__[CONTENT_TYPE_SIZE + HOST_NAME_SIZE];
	char target[TARGET_SIZE];
	char apikey[APIKEY_SIZE];
    };
    
}HTTPHeader;

typedef struct FormElem{
    sal_str key;
    sal_str value;
}FormElem;

int32_t    mini_http_print_header(HTTPHeader *h);
sal_str    mini_http_gen_header_str(HTTPHeader *h, sal_str str);
HTTPHeader mini_http_rqst_init(bool connection, uint8_t version,
                               uint8_t request_type, uint32_t content_length,
                               char *content_type, char *host, char *target,
                               char *apikey);
HTTPHeader mini_http_resp_init(bool connection, uint8_t version,
			       uint16_t response_code, uint32_t content_length,
			       char *content_type, char *host, char *status_msg);
HTTPHeader mini_http_parse_header(sal_str str);
void mini_http_form_arr_init(FormElem *buff, uint32_t buff_size);
void mini_http_form_arr_deinit(FormElem *buff, uint32_t buff_size);
void mini_http_form_arr_clear(FormElem *buff, uint32_t buff_size);
int32_t mini_http_parse_form_data(sal_str data, FormElem *buff, size_t buff_size, FormElem **end_ptr);
sal_str mini_http_gen_form_str(FormElem *data, FormElem *end_ptr, sal_str str);
#endif // __MINI_HTTP_H__
