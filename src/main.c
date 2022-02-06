#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define NO_PRINT_FUNC
#include "mini_http.h"
#define BUFF_SIZE 256

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

char http_form_header[] = "POST / HTTP/1.1\r\n\
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
fname=bayram&lname=akay";

char http_resp[] = "HTTP/1.1 200 OK\r\n\
Date: Tue, 27 Jul 2009 12:28:53 GMT\r\n\
Server: Apache/2.2.14 (Win32)\r\n\
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n\
Content-Length: 88\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\r\n";

int main(){
    printf("HTTPReq %ld\n", sizeof(HTTPReq));
    printf("HTTPResp %ld\n", sizeof(HTTPResp));
    char str[256] = {0};
    char *end_ptr;
    HTTPReq req = mini_http_parse_req(http_header, &end_ptr);
    printf("[ HTTP header ]\n");
    mini_http_print_req(&req);
    printf("[ HTTP body ]\n");
    printf("%s\n", end_ptr);  
    int status = mini_http_gen_req_str(&req, str, sizeof str);
    if(status == 0)
        printf("%s\n", str);
    else
        printf("Error\n");
    
    HTTPResp resp = {
        .status_code = 200,
        .version = HTTP_VER_1_1,
        .host = "server.host.com",
        .content_type = "text/html",
        .content_length = 120,
        .connection = false,
    };
    
    mini_http_gen_resp_str(&resp, str, sizeof(str));
    printf("[ HTTP response ]\n");
    printf("%s\n", str);

    HTTPResp a = mini_http_parse_resp(http_resp, NULL);
    mini_http_print_resp(&a);
    return 0;
}
