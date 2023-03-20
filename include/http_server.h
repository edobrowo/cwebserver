#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <stdint.h>

#include "route_table.h"

typedef struct {
    uint16_t port;
    int socket;
    route_table_t* rt;
} http_server_t;

// Create a HTTP server
http_server_t* http_server_create();

// Destroy a HTTP server
int http_server_destroy(http_server_t* server);

// Listen to a port on localhost
int http_server_listen(http_server_t* server, int port);

// Serve static content given a root path
int http_server_static(http_server_t* server, const char* root, const char* temp);

// Set the default route
int http_server_default(http_server_t* server, const char* path);

// Set the 404 route
int http_server_404(http_server_t* server, const char* path);

#endif // HTTP_SERVER
