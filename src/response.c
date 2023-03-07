#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "request.h"

#include "response.h"

int response_serialize_content(response_t* response, const char* path) {
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

int response_init(response_t* response, request_t* request) {
    int err;
    if (!request || !request->host || !request->path) {
        wslog(ERRR, "Can not init response with uninitialized request");
        return -1;
    }

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

    // TODO: check if route is valid, 404 if not found

    response->code = OK;

    const char* header_data = "HTTP/1.1 200 OK\r\n\n";
    size_t header_length = strlen(header_data);

    response->length = header_length;
    response->data = malloc(response->length * sizeof(char));
    if (!response->data) {
        wslog(ERRR, "Could not allocate memory for response->data");
        return -1;
    }
    memcpy(response->data, header_data, header_length);

    // Routing isn't implemented yet, so just always respond with index.html
    const char* path = "example/index.html";
    err = response_serialize_content(response, path);
    if (err) {
        wslog(ERRR, "Could not serialize (%s)", path);
        return -1;
    }

    return 0;
}
