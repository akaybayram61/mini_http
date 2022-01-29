#include <string.h>
#include <stdlib.h>
#ifndef NO_PRINT_FUNC
#include <stdio.h>
#endif // NO_PRINT_FUNC
#include "mini_http.h"

enum{
    FIRST_LINE,
    HOST,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    APIKEY,
    HTTP_HEADER_VALUE_SIZE
};

static char *http_header_temp[] = {
    [FIRST_LINE] = "%s %s %s\r\n",
    [HOST] = "Host: %s\r\n",
    [CONTENT_TYPE] = "Content-Type: %s\r\n",
    [CONTENT_LENGTH] = "Content-Length: %d\r\n",
    [APIKEY] = "ApiKey: %s\r\n", 
};

const char *HTTPReqStr[] = {
    [HTTP_REQ_GET] = "GET",
    [HTTP_REQ_POST] = "POST",
};

const char *HTTPVerStr[] = {
    [HTTP_VER_1_0] = "HTTP/1.0",
    [HTTP_VER_1_1] = "HTTP/1.1",
};

#ifndef NO_PRINT_FUNC
/**
 * prints HTTPReq structure.
 * NOTE: This funtion using printf() function to write stdout
* @param  req  pointer to HTTPReq structure
* @return If structure succesfully printed returns 0, otherwise returns -1.
*/
int32_t mini_http_print_req(HTTPReq *req){
    if(req == NULL)
        return -1;
    printf("%s %s %s\n", HTTPReqStr[req->type], req->target, HTTPVerStr[req->version]);
    
    if(strlen(req->content_type) != 0)
        printf("Content-Type: %s\n", req->content_type);
    
    if(strlen(req->host) != 0)
        printf("Host: %s\n", req->host);

    printf("Content-Length: %d\n", req->content_length);
    
    if(strlen(req->apikey) != 0)
        printf("ApiKey: %s\n", req->apikey);
    
    return 0;
}
#endif // NO_PRINT_FUNC
/**
 * Generates HTTP header string from HTTPReq structure.
* @param req pointer to HTTPReq structure.
* @param buff HTTP result string buffer.
* @param buff_size HTTP result string buffer max size.
* @return If generation succesfull returns 0, otherwise returns -1.
*/
int32_t mini_http_gen_str(HTTPReq *req, char *buff, int32_t buff_size){
    if(buff == NULL || buff_size <= 0)
        return -1;
    
    char tmp_buff[128] = {0};
    
    size_t offset = 0;
    size_t elem_size = 0;
    for(size_t i = 0; i < HTTP_HEADER_VALUE_SIZE; ++i){
        memset(tmp_buff, 0, strlen(tmp_buff));
        switch(i){
            case FIRST_LINE:
                sprintf(tmp_buff, http_header_temp[FIRST_LINE], 
                    HTTPReqStr[req->type], 
                    req->target, 
                    HTTPVerStr[req->version]);
                break;
                
            case HOST:
                if(req->host == NULL)
                    break;
                sprintf(tmp_buff, http_header_temp[HOST],
                req->host);
                break;
                
            case CONTENT_LENGTH:
                sprintf(tmp_buff, http_header_temp[CONTENT_LENGTH],
                req->content_length);
                break;
            
            case CONTENT_TYPE:
                if(req->content_type == NULL)
                    break;
                sprintf(tmp_buff, http_header_temp[CONTENT_TYPE],
                req->content_type);
                break;
            
            case APIKEY:
                if(req->apikey == NULL)
                    break; 
                sprintf(tmp_buff, http_header_temp[APIKEY],
                req->apikey);
                break;
        }
        elem_size = strlen(tmp_buff);
        tmp_buff[elem_size] = 0;
        strcpy(buff + offset, tmp_buff);
        offset += elem_size;
        
        if(offset >= (size_t)buff_size)
            return -1;
    }

    return 0;
}

/**
 * Parse given string to HTTPReq structure and return rest of the string.
* @param http_req pointer to HTTP request string.
* @param end_ptr pointer to HTTP body string. Can be given NULL.
* @return Filled HTTPReq structure
*/
HTTPReq mini_http_parse_req(char *http_req, char **end_ptr){
    HTTPReq res = {0};
    char *line = strtok(http_req, "\r\n");
    
    // Parse first line
    if(strstr(line, "POST") != NULL)
        res.type = HTTP_REQ_POST;
    else
        res.type = HTTP_REQ_GET;
    
    if(strstr(line, "HTTP/1.0") != NULL)
        res.version = HTTP_VER_1_0;
    else
        res.version = HTTP_VER_1_1;

    char *n;
    if((n = strstr(line, "/")) != NULL){
        int32_t count = 0;
        while(*n != ' '){
            res.target[count++] = *n++;
        }
    }
    
    // Parse other lines
    while((line = strtok(NULL, "\r\n")) != NULL){
        *end_ptr = line;    

        const char *host = "Host: ";
        if(strstr(line, host) != NULL){
            strcpy(res.host, line + strlen(host));
            continue;
        }

        const char *content_type = "Content-Type: ";
        if(strstr(line, content_type) != NULL){
            strcpy(res.content_type, line + strlen(content_type));
            continue;
        }
        
        const char *content_length = "Content-Length: ";
        if(strstr(line, content_length) != NULL){
           res.content_length = atoi(line + strlen(content_length));
           continue;
        }
        
        const char *apikey = "ApiKey: ";
        if(strstr(line, apikey) != NULL){
           strcpy(res.apikey, line + strlen(apikey));
           continue;
        }
    }
    return res;
}