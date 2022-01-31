#ifndef _MINI_HTTP_
#define _MINI_HTTP_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define STATUS_MSG_SIZE 32
#define CONTENT_TYPE_SIZE 32
#define HOST_NAME_SIZE 64
#define TARGET_SIZE 32
#define APIKEY_SIZE 32

typedef enum{
    HTTP_REQ_GET,
    HTTP_REQ_POST,
    HTTP_REQ_SIZE,
}HTTPReqType;

typedef enum{
    HTTP_CONTINUE = 100,
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_RESP_CODE_SIZE = 4, // Update this while adding new value
}HTTPRespCode;

typedef enum{
    HTTP_VER_1_0,
    HTTP_VER_1_1,
    HTTP_VER_SIZE,
}HTTPVersion;

typedef struct HTTPResp{
    HTTPRespCode status_code;  
    HTTPVersion version;
    char host[HOST_NAME_SIZE];
    char content_type[CONTENT_TYPE_SIZE];
    int32_t content_length;
    char status_msg[STATUS_MSG_SIZE];
    bool connection;
}HTTPResp;

typedef struct HTTPReq{
    HTTPReqType type;
    char target[TARGET_SIZE];
    HTTPVersion version;
    char host[HOST_NAME_SIZE];
    char content_type[CONTENT_TYPE_SIZE];
    int32_t content_length;
    char apikey[APIKEY_SIZE];
    bool connection;
}HTTPReq;

int32_t mini_http_print_req(HTTPReq *req);
int32_t mini_http_gen_req_str(HTTPReq *req, char *buff, int32_t buff_size);
HTTPReq mini_http_parse_req(char *http_req, char **end_ptr);
int32_t mini_http_get_resp_msg(HTTPRespCode code, char *buff);
int32_t mini_http_print_resp(HTTPResp *resp);
HTTPResp mini_http_parse_resp(char *http_resp, char **end_ptr);
int32_t mini_http_gen_resp_str(HTTPResp *resp, char *buff, int32_t buff_size);
#endif // _MINI_HTTP_
