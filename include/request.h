#ifndef REQUEST
#define REQUEST

typedef enum {
    GET = 0,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
} request_type_t;

typedef struct {
    request_type_t type;
    char* host;
    char* subdir;
} request_t;

// Create a request structure from a header buffer
request_t* request_create(char* buf);

// Destroy a request
void request_destroy(request_t* request);

#endif // REQUEST
