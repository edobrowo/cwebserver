#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#include <unistd.h>

#include "http_server.h"
#include "log.h"

#define NUM_CONNECTIONS_MAX 8

http_server_t* http_server_create() {
    int err;

    http_server_t* server = malloc(sizeof(http_server_t));
    if (!server) {
        wslog(ERRR, "Memory for http_server_t could not be allocated");
        return NULL;
    }

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        wslog(ERRR, "Server socket could not be created");
        return NULL;
    }

    server->socket = server_socket;
    server->port = 0;

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
    wslog(INFO, "Server with socketfd (%d) destroyed", server_socket);
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

    wslog(INFO, "Server socket (%d) listening on port (%d)", server->socket, server->port);
    return 0;
}
