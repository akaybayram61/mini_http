#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define NO_PRINT_FUNC
#include "mini_http.h"
#define BUFF_SIZE 256
#define MAX_HEADER_LEN 4096

char http_header[] = "POST /cgi-bin/process.cgi HTTP/1.1\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n\
Host: www.tutorialspoint.com\r\n\
Content-Type: text/xml; charset=utf-8\r\n\
Content-Length: 100\r\n\
Accept-Language: en-us\r\n\
Accept-Encoding: gzip, deflate\r\n\
Connection: Keep-Alive\r\n\
ApiKey: s2easd4rasd\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n\
<string xmlns=\"http://clearforest.com/\">string</string>\r\n\r\n";

char http_form_header[] = "POST / HTTP/1.0\r\n\
Host: 127.0.0.1\r\n\
Connection: keep-alive\r\n\
Content-Length: 23\r\n\
Cache-Control: max-age=0\r\n\
Upgrade-Insecure-Requests: 1\r\n\
Origin: null\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.71 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Sec-GPC: 1\r\n\
Sec-Fetch-Site: cross-site\r\n\
Sec-Fetch-Mode: navigate\r\n\
Sec-Fetch-User: ?1\r\n\
Sec-Fetch-Dest: document\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\
\r\n\r\n\
fname=bayram&lname=akay&deneme=123&iottrialadsa=birnumara";

char http_resp[] = "HTTP/1.0 200 OK\r\n\
Date: Tue, 27 Jul 2009 12:28:53 GMT\r\n\
Host: www.google.com\r\n\
Server: Apache/2.2.14 (Win32)\r\n\
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n\
Content-Length: 88\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\r\n";

char str_buff[MAX_HEADER_LEN] = {0};
sal_str send_buff = {.begin = str_buff, .size = 0};

#define FORM_ELEM_BUFF_SIZE 10

FormElem form_buff[FORM_ELEM_BUFF_SIZE];
static const sal_str h_delim = {.begin = "\r\n\r\n", .size = 4};

int main() {
    mini_http_form_arr_init(form_buff, FORM_ELEM_BUFF_SIZE);
    /* HTTPHeader h = mini_http_rqst_init(true, HTTP_VER_1_1, HTTP_RQST_GET, */
    /* 				       1024, "application/html", */
    /* 				       "www.google.com", "/index.html",NULL); */
    /* printf("http header sizeof: %lu\n", sizeof h); */

    /* send_buff = mini_http_gen_header_str(&h, send_buff); */
    /* sal_println(send_buff); */
    /* printf("send_buff size: %u\n", send_buff.size); */
    /* HTTPHeader head; */
    /* head = mini_http_parse_header(sal_tostr(http_resp)); */
    /* mini_http_print_header(&head); */

    /* putc('\n', stdout); */
    /* putc('\n', stdout); */
    /* head = mini_http_parse_header(sal_tostr(http_form_header)); */
    /* mini_http_print_header(&head); */
    /* putc('\n', stdout); */
    /* putc('\n', stdout); */
    /* head = mini_http_parse_header(sal_tostr(http_header)); */
    /* mini_http_print_header(&head); */
    sal_str buff = sal_tostr(http_form_header);
    sal_str form_part = sal_str_chop(buff, h_delim);
    form_part = sal_str_chop(SAL_NULL_STR, h_delim);
    FormElem *end_ptr = NULL;
    mini_http_parse_form_data(form_part, form_buff, FORM_ELEM_BUFF_SIZE, &end_ptr);
    send_buff = mini_http_gen_form_str(form_buff, end_ptr, send_buff);
    sal_println(send_buff);
    mini_http_form_arr_deinit(form_buff, FORM_ELEM_BUFF_SIZE);
    return 0;
}
