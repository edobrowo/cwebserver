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

request_t* request_create(char* buf) {
    wslog(MORE, "PARSING REQUEST\n%s", buf);

    request_t* request = malloc(sizeof(request_t));
    if (!request) {
        wslog(ERRR, "Could not allocate memory for request");
        return NULL;
    }

    char* type = strtok(buf, " ");
    request->type = str_to_request_type(type);
    if (request->type == -1) {
        wslog(ERRR, "Request type (%d) could not be determined", request->type);
        return NULL;
    }

    char* subdir = strtok(NULL, " ");
    request->subdir = malloc((strlen(subdir) + 1) * sizeof(char));
    if (!request->subdir) {
        wslog(ERRR, "Could not allocate memory for request->subdir");
        return NULL;
    }
    strcpy(request->subdir, subdir);

    char field[2048] = {0};
    char field_name[1024] = {0};
    char field_value[1024] = {0};
    for (char* line = strtok(NULL, "\n"); line != NULL; line = strtok(NULL, "\n")) {
        strcpy(field, line);
        request_extract_field(field, field_name, field_value);

        if (!strcmp(field_name, "Host")) {
            request->host = malloc((strlen(field_value) + 1) * sizeof(char));
            if (!request->host) {
                wslog(ERRR, "Could not allocate memory for request->host");
                return NULL;
            }
            strcpy(request->host, field_value);
        }
    }

    wslog(MORE, "Request type: %s", type);
    wslog(MORE, "Request host: %s", request->host);
    wslog(MORE, "Request subdirectory: %s", request->subdir);

    return request;
}

void request_destroy(request_t* request) {
    if (!request) return;
    free((void *)request->host);
    free((void *)request->subdir);
    free(request);
}
