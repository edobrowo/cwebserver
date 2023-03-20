#include "http_server.h"
#include "log.h"

int main() {
    int err;
    http_server_t *server = http_server_create();
    if (!server) {
        wslog(ERRR, "Server could not be initialized", server);
        return -1;
    }

    // Serve static content
    http_server_404(server, "example/404.html");
    http_server_default(server, "example/index.html");
    http_server_static(server, "example/index.html", "/index.html");

    const int port = 8080;
    err = http_server_listen(server, port);
    if (err) {
        wslog(ERRR, "Server could not listen on port (%d)", port);
        return -1;
    }

    err = http_server_destroy(server);
    if (err) {
        wslog(ERRR, "Server could not be closed (socketfd = %d)", server->socket);
        return -1;
    }

    return 0;
}
