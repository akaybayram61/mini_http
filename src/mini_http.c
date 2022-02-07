#include <string.h>
#include <stdlib.h>
#ifndef NO_PRINT_FUNC
#include <stdio.h>
#endif // NO_PRINT_FUNC
#include "cassert.h"
#include "strutil.h"
#include "mini_http.h"

#define TMP_BUFF_SIZE 48

enum{
    REQ_FIRST_LINE,
    RESP_FIRST_LINE,
    HOST,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    APIKEY,
    CONNECTION,
    HTTP_HEADER_VALUE_SIZE
};
cassert(sizeof(HTTPReq) == 176);

static char *http_header_temp[] = {
    [REQ_FIRST_LINE] = "%s %s %s\r\n",
    [RESP_FIRST_LINE] = "%s %d %s\r\n", 
    [HOST] = "Host: %s\r\n",
    [CONTENT_TYPE] = "Content-Type: %s\r\n",
    [CONTENT_LENGTH] = "Content-Length: %d\r\n",
    [APIKEY] = "ApiKey: %s\r\n", 
    [CONNECTION] = "Connection: %s\r\n",
};
cassert(HTTP_HEADER_VALUE_SIZE == 7);
cassert(sizeof(HTTPReq) == 176);

const char *HTTPReqStr[] = {
    [HTTP_REQ_GET] = "GET",
    [HTTP_REQ_POST] = "POST",
};
cassert(HTTP_REQ_SIZE == 2);

const char *HTTPVerStr[] = {
    [HTTP_VER_1_0] = "HTTP/1.0",
    [HTTP_VER_1_1] = "HTTP/1.1",
};
cassert(HTTP_VER_SIZE == 2);

#ifndef NO_PRINT_FUNC
/**
 * prints HTTPReq structure.
 * NOTE: This funtion using printf() function to write stdout
* @param  req  pointer to HTTPReq structure
* @return If structure succesfully printed returns MINI_HTTP_OK, otherwise returns MINI_HTTP_FAIL.
*/
int32_t mini_http_print_req(HTTPReq *req){
    cassert(sizeof(HTTPReq) == 176);
    if(req == NULL)
        return MINI_HTTP_FAIL;
    printf("%s %s %s\n", HTTPReqStr[req->type], req->target, HTTPVerStr[req->version]);
    
    if(strlen(req->content_type) != 0)
        printf("Content-Type: %s\n", req->content_type);
    
    if(strlen(req->host) != 0)
        printf("Host: %s\n", req->host);

    printf("Content-Length: %d\n", req->content_length);
    
    if(strlen(req->apikey) != 0)
        printf("ApiKey: %s\n", req->apikey);
    
    printf("Connection: %s\n", req->connection ? "keep-alive": "close");
    
    cassert(sizeof(HTTPReq) == 176);
    return MINI_HTTP_OK;
}
#endif // NO_PRINT_FUNC
/**
 * Generates HTTP header string from HTTPReq structure.
* @param req pointer to HTTPReq structure.
* @param buff HTTP result string buffer.
* @param buff_size HTTP result string buffer max size.
* @return If generation succesfull returns MINI_HTTP_OK, otherwise returns MINI_HTTP_FAIL.
*/
int32_t mini_http_gen_req_str(HTTPReq *req, char *buff, int32_t buff_size){
    if(buff == NULL || buff_size <= 0)
        return MINI_HTTP_BAD_ARG;
    
    char tmp_buff[128] = {0};
    
    size_t offset = 0;
    size_t elem_size = 0;
    for(size_t i = 0; i < HTTP_HEADER_VALUE_SIZE; ++i){
        memset(tmp_buff, 0, strlen(tmp_buff));
        switch(i){
            case REQ_FIRST_LINE:
                sprintf(tmp_buff, http_header_temp[REQ_FIRST_LINE], 
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
            case CONNECTION:
                sprintf(tmp_buff, req->connection ? "Connection: keep-alive": "Connection: close");
                break;

            cassert(sizeof(HTTPReq) == 176);
        }
        elem_size = strlen(tmp_buff);
        tmp_buff[elem_size] = 0;
        strcpy(buff + offset, tmp_buff);
        offset += elem_size;
        
        if(offset >= (size_t)buff_size)
            return MINI_HTTP_FAIL;
    }

    return MINI_HTTP_OK;
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
    if(strcasestr(line, "POST") != NULL)
        res.type = HTTP_REQ_POST;
    else
        res.type = HTTP_REQ_GET;
    cassert(HTTP_REQ_SIZE == 2);
    
    if(strcasestr(line, "HTTP/1.0") != NULL)
        res.version = HTTP_VER_1_0;
    else
        res.version = HTTP_VER_1_1;
    cassert(HTTP_VER_SIZE == 2);

    char *n;
    if((n = strcasestr(line, "/")) != NULL){
        int32_t count = 0;
        while(*n != ' '){
            res.target[count++] = *n++;
        }
    }
    
    // Parse other lines
    while((line = strtok(NULL, "\r\n")) != NULL){
        
        if(end_ptr != NULL)
            *end_ptr = line;    

        const char *host = "Host: ";
        if(strcasestr(line, host) != NULL){
            strcpy(res.host, line + strlen(host));
            continue;
        }

        const char *content_type = "Content-Type: ";
        if(strcasestr(line, content_type) != NULL){
            strcpy(res.content_type, line + strlen(content_type));
            continue;
        }
        
        const char *content_length = "Content-Length: ";
        if(strcasestr(line, content_length) != NULL){
           res.content_length = atoi(line + strlen(content_length));
           continue;
        }
        
        const char *apikey = "ApiKey: ";
        if(strcasestr(line, apikey) != NULL){
           strcpy(res.apikey, line + strlen(apikey));
           continue;
        }

        const char *connection = "Connection: ";
        if(strcasestr(line, connection) != NULL){
            if(strcasestr(line, "keep-alive"))
                res.connection = true;
            else
                res.connection = false;
            continue;
        } 
        cassert(sizeof(HTTPReq) == 176);
    }
    return res;
}

/**
 * Parse given string to HTTPReq structure and return rest of the string.
* @param code HTTP response code.
* @param buff HTTP response message buffer.
* @return On succes returns MINI_HTTP_OK, returns MINI_HTTP_FAIL unknown code, returns MINI_HTTP_BAD_ARG when buff is NULL.
*/
int32_t mini_http_get_status_msg(HTTPRespCode code, char *buff){
    if (buff == NULL)
        return MINI_HTTP_BAD_ARG;

    char *msg = NULL;
    int32_t res = MINI_HTTP_OK;
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
            res = MINI_HTTP_FAIL;
            break;
    }
    cassert(HTTP_RESP_CODE_SIZE == 4);

    strncpy(buff, msg, STATUS_MSG_SIZE); 
    return res;    
}

int32_t mini_http_print_resp(HTTPResp *resp){
    if(resp == NULL)
        return MINI_HTTP_BAD_ARG;
    printf("%s %d %s\n", HTTPVerStr[resp->version], resp->status_code, resp->status_msg);
    
    if(strlen(resp->content_type) != 0)
        printf("Content-Type: %s\n", resp->content_type);
    
    if(strlen(resp->host) != 0)
        printf("Host: %s\n", resp->host);

    printf("Content-Length: %d\n", resp->content_length);
    
//    No apikey support for response
//    if(strlen(req->apikey) != 0)
//        printf("ApiKey: %s\n", req->apikey);
    
    printf("Connection: %s\n", resp->connection ? "keep-alive": "close");
    
    cassert(sizeof(HTTPResp) == 144);
    return MINI_HTTP_OK;
    return 0;
}

HTTPResp mini_http_parse_resp(char *http_resp, char **end_ptr){
    HTTPResp res = {0};
    char *line = strtok(http_resp, "\r\n");
    
    // make copy for further tokenizing
    char first_line[64];
    strncpy(first_line, line, sizeof first_line);
    
    // Parse other lines
    while((line = strtok(NULL, "\r\n")) != NULL){
        if(end_ptr != NULL)
            *end_ptr = line;    

        const char *host = "Host: ";
        if(strcasestr(line, host) != NULL){
            strcpy(res.host, line + strlen(host));
            continue;
        }

        const char *content_type = "Content-Type: ";
        if(strcasestr(line, content_type) != NULL){
            strcpy(res.content_type, line + strlen(content_type));
            continue;
        }
        
        const char *content_length = "Content-Length: ";
        if(strcasestr(line, content_length) != NULL){
            res.content_length = atoi(line + strlen(content_length));
            continue;
        }

        const char *connection = "Connection: ";
        if(strcasestr(line, connection) != NULL){
            if(strcasestr(line, "keep-alive"))
                res.connection = true;
            else
                res.connection = false;
            continue;
        } 

        cassert(sizeof(HTTPResp) == 144);
    }

    // Parse first line
    char *token = strtok(first_line, " ");

    if(strstr(token, "HTTP/1.0") != NULL)
        res.version = HTTP_VER_1_0;
    else
        res.version = HTTP_VER_1_1;

    token = strtok(NULL, " ");
    res.status_code = atoi(token);
    
    token = strtok(NULL, " ");
    strncpy(res.status_msg, token, STATUS_MSG_SIZE);
    
    return res;
}

int32_t mini_http_gen_resp_str(HTTPResp *resp, char *buff, int32_t buff_size){
    if(buff == NULL || buff_size <= 0)
        return MINI_HTTP_BAD_ARG;
    
    char tmp_buff[128] = {0};
    
    size_t offset = 0;
    size_t elem_size = 0;
    for(size_t i = 0; i < HTTP_HEADER_VALUE_SIZE; ++i){
        memset(tmp_buff, 0, strlen(tmp_buff));
        switch(i){
            case RESP_FIRST_LINE:
                mini_http_get_status_msg(resp->status_code, resp->status_msg);
                sprintf(tmp_buff, http_header_temp[RESP_FIRST_LINE], 
                    HTTPVerStr[resp->version], 
                    resp->status_code, 
                    resp->status_msg);
                break;
                
            case HOST:
                if(resp->host == NULL)
                    break;
                sprintf(tmp_buff, http_header_temp[HOST],
                resp->host);
                break;
                
            case CONTENT_LENGTH:
                sprintf(tmp_buff, http_header_temp[CONTENT_LENGTH],
                resp->content_length);
                break;
            
            case CONTENT_TYPE:
                if(resp->content_type == NULL)
                    break;
                sprintf(tmp_buff, http_header_temp[CONTENT_TYPE],
                resp->content_type);
                break;
            
            case CONNECTION:
                sprintf(tmp_buff, resp->connection ? "Connection: keep-alive": "Connection: close");
                break;

            cassert(sizeof(HTTPResp) == 144);
        }
        elem_size = strlen(tmp_buff);
        tmp_buff[elem_size] = 0;
        strcpy(buff + offset, tmp_buff);
        offset += elem_size;
        
        if(offset >= (size_t)buff_size)
            return MINI_HTTP_FAIL;
    }

    return MINI_HTTP_OK;
}

static char *replace_char_in_str(char *str, char target, char replacement){
    if(str == NULL)
        return NULL;
    
    size_t len = strlen(str);
    size_t i;
    for(i = 0; i < len; ++i){
        if(str[i] == target){
            str[i] = replacement;
            break;
        }
    }
    
    return (str + i + 1);
}

int32_t mini_http_parse_form_data(char *data, FormElem *buff, size_t buff_size, FormElem **end_ptr){
    if(data == NULL || buff_size == 0 || end_ptr == NULL)
        return MINI_HTTP_BAD_ARG;

    char tmp_buff[TMP_BUFF_SIZE];
    memset(buff, 0, buff_size * sizeof(FormElem));
    char *pair = strtok(data, "&"); 

    size_t i = 0;
    while (pair != NULL){
        memset(tmp_buff, 0, TMP_BUFF_SIZE);
        strncpy(tmp_buff, pair, TMP_BUFF_SIZE);
        
        char *value = replace_char_in_str(tmp_buff, '=', 0);
        strncpy(buff[i].key, tmp_buff, MAX_KEY_LEN);
        strncpy(buff[i].value, value, MAX_VALUE_LEN);
        
        // restrict buffer with null terminator
        buff[i].key[MAX_KEY_LEN - 1] = 0;
        buff[i].value[MAX_VALUE_LEN - 1] = 0;
        
        i++;
        if(i == buff_size)
            break;
        pair = strtok(NULL, "&");
    }

    *end_ptr = buff + i;

    return MINI_HTTP_OK;
}

int32_t mini_http_gen_form_str(FormElem *data, FormElem *end_ptr, char *buff, size_t buff_len){
    if(data == NULL || buff == NULL || end_ptr == NULL || buff_len == 0)
        return MINI_HTTP_BAD_ARG; 

    size_t curr_buff_size = 0;
    do{
        size_t key_len = strlen(data->key);
        size_t val_len = strlen(data->value);
        strncpy(buff + curr_buff_size, data->key, MAX_KEY_LEN);
        curr_buff_size += key_len;
        strcpy(buff + curr_buff_size, "=");
        curr_buff_size++;
        strncpy(buff + curr_buff_size, data->value, MAX_VALUE_LEN);
        curr_buff_size += val_len;
        strcpy(buff + curr_buff_size, "&");
        curr_buff_size++;
        if(curr_buff_size >= buff_len)
            return MINI_HTTP_FAIL;
    }while (data++ != end_ptr);

    buff[curr_buff_size - 3] = '\0'; // '\0' terminator
    /********************************************************************/
    /*         +-----+-----+-----+          -1 for zero indexed array   */
    /* buff -> | '&' | '0' |  E  |          -2 for offsets              */
    /*         +-----+-----+-----+         +__________________________  */
    /*                        ^             -3 length of buffer         */
    /*                        |                                         */
    /*                     curr_buff_size - 1                           */
    /********************************************************************/
    return curr_buff_size - 3;
}