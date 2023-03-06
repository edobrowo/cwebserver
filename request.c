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

void request_extract_field(const char* field, char* name, char* value) {
    while (*field != ':') *name++ = *field++;
    field += 2;
    while (*field != '\0') *value++ = *field++;
    *name = '\0', *value = '\0';
}

int request_parse_header(request_t* request, char* buf) {
    wslog(INFO, "Parsing request...");

    char* type = strtok(buf, " ");
    request_type_t request_type = str_to_request_type(type);
    if (request_type == -1) {
        wslog(ERRR, "Request type (%d) could not be determined", request_type);
        return -1;
    }
    request->type = request_type;

    char field[2048] = {0};
    char field_name[1024] = {0};
    char field_value[1024] = {0};
    for (char* line = strtok(NULL, "\n"); line != NULL; line = strtok(NULL, "\n")) {
        strcpy(field, line);
        request_extract_field(field, field_name, field_value);

        if (!strcmp(field_name, "Host")) {
            char* request_host = malloc(strlen(field_value) * sizeof(char));
            if (!request_host) {
                wslog(ERRR, "Memory for request->host could not be allocated");
                return -1;
            }
            strcpy(request_host, field_value);
            request->host = request_host;
        }
        else if (!strcmp(field_name, "Referer")) {
            char* request_url = malloc(strlen(field_value) * sizeof(char));
            if (!request_url) {
                wslog(ERRR, "Memory for request->url could not be allocated");
                return -1;
            }
            strcpy(request_url, field_value);
            request->url = request_url;
        }
    }

    wslog(MORE, "Request type: %s", type);
    wslog(MORE, "Request host: %s", request->host);
    wslog(MORE, "Request URL: %s", request->url);
    return 0;
}
