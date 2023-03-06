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

    // TODO: modularize response serialization
    FILE* fp = fopen("example/index.html", "r");
    if (!fp) {
        return -1;
    }

    char response_contents_data[1024] = {0};
    err = fread(response_contents_data, sizeof(char), 1024, fp);
    if (!err) {
        wslog(ERRR, "Could not read from requested file");
        return -1;
    }

    err = fclose(fp);
    if (err) {
        wslog(ERRR, "Could not close requested file");
        return -1;
    }

    char response_data[2048] = {0};
    char response_header_data[1024] = "HTTP/1.1 200 OK\r\n\n";
    strcat(response_data, response_header_data);
    strcat(response_data, response_contents_data);

    int client_socket;
    while (1) {
        client_socket = accept(server->socket, NULL, NULL);

        char request_data[4096] = {0};
        err = read(client_socket, request_data, 4096 * sizeof(char));
        if (!err) {
            wslog(ERRR, "Could not read request file");
            return -1;
        }

        // wslog(INFO, "REQUEST\n%s", request_data);
        request_t* request = malloc(sizeof(request_t));
        if (!request) {
            wslog(ERRR, "Memory for request could not be allocated");
            return -1;
        }

        err = request_parse_header(request, request_data);
        if (err) {
            wslog(ERRR, "Could not parse header data");
            return -1;
        }

        err = send(client_socket, response_data, sizeof(response_data), 0);
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
