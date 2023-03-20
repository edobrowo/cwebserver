#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "request.h"

#include "response.h"

response_t* response_create(request_t* request) {
    response_t* response = malloc(sizeof(response_t));
    if (!response) {
        wslog(ERRR, "Could not allocate memory for response");
        return NULL;
    } 

    response->request = request;

    switch (request->type) {
        case GET: break;
        // Other handlers aren't implemented, so treat as GET :)
        case HEAD: 
        case POST:
        case PUT:
        case DELETE:
        case CONNECT:
        case OPTIONS:
        case TRACE:
        case PATCH: break;
    }

    const char* header_data = "HTTP/1.1 200 OK\r\n\n";
    response->length = strlen(header_data);
    response->data = malloc(response->length * sizeof(char));
    if (!response->data) {
        wslog(ERRR, "Could not allocate memory for response->data");
        return NULL;
    }
    memcpy(response->data, header_data, response->length);

    return response;
}

void response_destroy(response_t* response) {
    if (!response) return;
    request_destroy(response->request);
    free(response->data);
    free(response);
}

int response_serialize(response_t* response, const char* path) {
    int err;
    FILE* fp;
    long int fs;
    
    fp = fopen(path, "r");
    if (!fp) {
        wslog(ERRR, "Could not open file (%s)", path);
        return -1;
    }

    err = fseek(fp, 0L, SEEK_END);
    if (err) {
        wslog(ERRR, "Could not seek file (%s)", path);
        return -1;
    }

    fs = ftell(fp);
    if (fs == -1) {
        wslog(ERRR, "Could not tell file position (%s)", path);
        return -1;
    }

    err = fseek(fp, 0L, SEEK_SET);
    if (err) {
        wslog(ERRR, "Could not seek file (%s)", path);
        return -1;
    }

    char* response_ptr_after_header = response->data + response->length;
    response->length += fs + 1;
    response->data = realloc(response->data, response->length * sizeof(char));
    if (!response->data) {
        wslog(ERRR, "Could not reallocate memory for response->data");
        return -1;
    }

    err = fread(response_ptr_after_header, sizeof(char), fs + 1, fp);
    if (!err) {
        wslog(ERRR, "Could not read from requested file");
        return -1;
    }

    err = fclose(fp);
    if (err) {
        wslog(ERRR, "Could not close requested file");
        return -1;
    }

    return 0;
}
