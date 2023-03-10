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
    const char* host;
    const char* path;
} request_t;

// Initialize a request from a header
// Currently retrieves request type, host, and URL
int request_init(request_t* request, char* buf);

#endif // REQUEST
