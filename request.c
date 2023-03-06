#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "request.h"

request_type_t str_to_request_type(const char* str) {
    if (!strcmp(str, "GET"))     return GET;
    if (!strcmp(str, "HEAD"))    return HEAD;
    if (!strcmp(str, "POST"))    return POST;
    if (!strcmp(str, "PUT"))     return PUT;
    if (!strcmp(str, "DELETE"))  return DELETE;
    if (!strcmp(str, "CONNECT")) return CONNECT;
    if (!strcmp(str, "OPTIONS")) return OPTIONS;
    if (!strcmp(str, "TRACE"))   return TRACE;
    if (!strcmp(str, "PATCH"))   return PATCH;
    return -1;
}

int request_parse_header(request_t* request, char* buf) {
    char* token = strtok(buf, " ");
    request_type_t request_type = str_to_request_type(token);
    if (request_type == -1) {
        wslog(ERRR, "Request type (%d) could not be determined", request_type);
    }
    request->type = request_type;

    for (token = strtok(NULL, " "); token != NULL; token = strtok(NULL, " ")) {
        printf("%s\n", token);
    }

    return 0;
}
