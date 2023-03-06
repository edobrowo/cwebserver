#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <stdint.h>

typedef struct {
    uint16_t port;
    int socket;
} http_server_t;

// Create a HTTP server
http_server_t* http_server_create();

// Destroy a HTTP server
int http_server_destroy(http_server_t* server);

// Listen to the given port on localhost
int http_server_listen(http_server_t* server, int port);

#endif // HTTP_SERVER
