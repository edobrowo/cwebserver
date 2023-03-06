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
    FILE* html_fp;
    int err;

    html_fp = fopen("example/index.html", "r");
    if (!html_fp) {
        perror("fopen");
        return -1;
    }

    char response_data[1024] = {0};
    err = fread(response_data, sizeof(char), 1024, html_fp);
    if (!err) {
        perror("fread");
        return -1;
    }

    err = fclose(html_fp);
    if (err) {
        perror("fclose");
        return -1;
    }

    char http_header_data[2048] = "HTTP/1.1 200 OK\r\n\n";
    strcat(http_header_data, response_data);

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

    int client_socket;
    while (1) {
        client_socket = accept(server->socket, NULL, NULL);

        char request_data[4096] = {0};
        err = read(client_socket, request_data, 4096 * sizeof(char));
        if (!err) {
            perror("read");
            return -1;
        }
        wslog(INFO, "REQUEST\n%s", request_data);
        request_t* request = malloc(sizeof(request_t));
        request_parse_header(request, request_data);

        send(client_socket, http_header_data, sizeof(http_header_data), 0);

        close(client_socket);
    }

    err = http_server_destroy(server);
    if (err) {
        wslog(ERRR, "Server could not be closed (socketfd = %d)", server->socket);
        return -1;
    }

    return 0;
}
