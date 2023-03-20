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
#include "route_table.h"

#define NUM_CONNECTIONS_MAX 8

int http_server_handle_request(http_server_t* server, const int client_socket, request_t* request) {
    int err;
    response_t* response = response_create(request);
    if (!response) {
        wslog(ERRR, "Could not create response");
        return -1;
    }

    // Manually ignore /favicon.co requests for now
    if (!strcmp(request->subdir, "/favicon.co")) {
        return 0;
    }

    char path[4096] = {0};
    err = rt_get_path(server->rt, request->subdir, path);
    if (err) {
        wslog(ERRR, "Could not retrieve path given subdir (%s)", path);
        return -1;
    }

    err = response_serialize(response, path);
    if (err) {
        wslog(ERRR, "Could not serialize contents given path (%s)", path);
        return -1;
    }

    wslog(MORE, "RESPONSE CONTENTS\n%s", response->data);

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

    return 0;
}

http_server_t* http_server_create() {
    http_server_t* server = malloc(sizeof(http_server_t));
    if (!server) {
        wslog(ERRR, "Could not allocate memory for http_server_t");
        return NULL;
    }

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        wslog(ERRR, "Server socket could not be created");
        free(server);
        return NULL;
    }

    server->socket = server_socket;
    server->port = 0;

    server->rt = rt_create();
    if (!server->rt) {
        wslog(ERRR, "Server route table could not be created.");
        close(server->socket);
        free(server);
        return NULL;
    }

    wslog(INFO, "Server initialized with socket (%d)", server->socket);
    return server;
}

int http_server_destroy(http_server_t* server) {
    int err;
    err = close(server->socket);
    if (err == -1) {
        wslog(ERRR, "Server socket (%d) could not be closed", server->socket);
        return err;
    }

    int server_socket = server->socket;
    free(server);

    rt_destroy(server->rt);

    wslog(INFO, "Server with socket (%d) destroyed", server_socket);
    return 0;
}

int http_server_listen(http_server_t* server, int port) {
    int err;

    server->port = port;

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server->port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    err = bind(server->socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (err == -1) {
        wslog(ERRR, "Server socket (%d) could not be bound on port (%d)", server->socket, server->port);
        return err;
    }

    err = listen(server->socket, NUM_CONNECTIONS_MAX);
    if (err == -1) {
        wslog(ERRR, "Socket (%d) could not listen on port (%d)", server->socket, server->port);
        return err;
    }

    wslog(INFO, "Server (%d) listening on port (%d)", server->socket, server->port);

    int client_socket;
    while (1) {
        client_socket = accept(server->socket, NULL, NULL);

        char request_data[4096] = {0};
        err = read(client_socket, request_data, 4095 * sizeof(char));
        if (!err) {
            wslog(ERRR, "Could not read request file");
            return err;
        }

        request_t* request = request_create(request_data);
        if (!request) {
            wslog(ERRR, "Could not create request");
            return -1;
        }

        err = http_server_handle_request(server, client_socket, request);
        if (err) {
            wslog(ERRR, "Could not handle request");
            return -1;
        }
    }
    return 0;
}

int http_server_static(http_server_t* server, const char* root, const char* temp) {
    // TODO: implement recursive pathing, directory support
    rt_add_route(server->rt, temp, root);
    return 0;
}

int http_server_default(http_server_t* server, const char* path) {
    int err = rt_add_route(server->rt, "/", path);

    if (err) {
        wslog(ERRR, "Could not route default subdirectory.");
        return -1;
    }

    return 0;
}

int http_server_404(http_server_t* server, const char* path) {
    int err = rt_set_404(server->rt, path);
    if (err) {
        wslog(ERRR, "Could not set 404 path.");
        return -1;
    }

    return 0;
}
