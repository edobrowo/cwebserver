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
    response_code_t code;
    char* data;
    size_t length;
} response_t;

// Write the response contents to the response buffer
int response_serialize_content(response_t* response, const char* path);

// Initialize a response given a request
int response_init(response_t* response, request_t* request);

#endif // RESPONSE
