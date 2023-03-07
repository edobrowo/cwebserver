#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <unistd.h>

#include "http_server.h"
#include "log.h"
#include "request.h"
#include "response.h"

int main() {
    int err;
    http_server_t *server = http_server_create();
    if (!server) {
        wslog(ERRR, "Server could not be initialized (address = %p)", server);
        return -1;
    }

    err = http_server_listen(server, 8080);
    if (err) {
        wslog(ERRR, "Server could not listen");
        return -1;
    }

    // TODO: routing
    // TODO: QOL serving static content
    // TODO: support larger file sizes
    // TODO: fix path field

    int client_socket;
    while (1) {
        client_socket = accept(server->socket, NULL, NULL);

        char request_data[4096] = {0};
        err = read(client_socket, request_data, 4096 * sizeof(char));
        if (!err) {
            wslog(ERRR, "Could not read request file");
            return -1;
        }

        request_t* request = malloc(sizeof(request_t));
        if (!request) {
            wslog(ERRR, "Could not allocate memory for request");
            return -1;
        }
        err = request_init(request, request_data);
        if (err) {
            wslog(ERRR, "Could not initialize request");
            return -1;
        }

        response_t* response = malloc(sizeof(response_t));
        if (!response) {
            wslog(ERRR, "Could not allocate memory for response");
            return -1;
        }
        err = response_init(response, request);
        if (err) {
            wslog(ERRR, "Could not initialize response");
            return -1;
        }

        err = send(client_socket, response->data, response->length * sizeof(char), 0);
        if (err == -1) {
            wslog(ERRR, "Could not send response data");
            return -1;
        }

        err = close(client_socket);
        if (err == -1) {
            wslog(ERRR, "Could not close client socket");
            return -1;
        }
    }

    err = http_server_destroy(server);
    if (err) {
        wslog(ERRR, "Server could not be closed (socketfd = %d)", server->socket);
        return -1;
    }

    return 0;
}
