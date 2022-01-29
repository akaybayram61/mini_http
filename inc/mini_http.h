#ifndef _MINI_HTTP_
#define _MINI_HTTP_

#include <stdint.h>
#include <string.h>

typedef enum{
    HTTP_REQ_GET,
    HTTP_REQ_POST,
}HTTPReqType;

typedef enum{
    HTTP_VER_1_0,
    HTTP_VER_1_1,
}HTTPVersion;

typedef struct HTTPReq{
    HTTPReqType type;
    char target[32];
    HTTPVersion version;
    char host[64];
    char content_type[32];
    int32_t content_length;
    char apikey[64];
}HTTPReq;

int32_t mini_http_print_req(HTTPReq *req);
int32_t mini_http_gen_str(HTTPReq *req, char *buff, int32_t buff_size);
HTTPReq mini_http_parse_req(char *http_req, char **end_ptr);

#endif // _MINI_HTTP_
