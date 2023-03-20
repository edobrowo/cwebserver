#ifndef RESPONSE
#define RESPONSE

#include <stdlib.h>

#include "request.h"

typedef enum {
    // Success
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    PARTIAL = 203,
    NORESP = 204,

    // Redirection
    MOVED = 301,
    FOUND = 302,
    METHOD = 303,
    NOTMODIFIED = 304,

    // Error
    BADREQ = 400,
    UNAUTHORIZED = 401,
    PAYMENTREQ = 402,
    FORBIDDEN = 403,
    NOTFOUND = 404,
    INTERNALERR = 500,
    NOTIMPL = 501,
    OVERLOADED = 502,
    TIMEOUT = 503

} response_code_t;

typedef struct {
    request_t* request;
    char* data;
    size_t length;
} response_t;

// Create a response given a request
response_t* response_create(request_t* request);

// Destroy a response
void response_destroy(response_t* response);

// Write the response contents to the response buffer
int response_serialize(response_t* response, const char* path);

#endif // RESPONSE
