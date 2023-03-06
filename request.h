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
    const char* url;
} request_t;

// Parse an HTTP request. Currently just retrieves request type, host, and URL.
int request_parse_header(request_t* request, char* buf);

#endif // REQUEST
