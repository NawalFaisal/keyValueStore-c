#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "keyValue.h"

void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);
    char buffer[MAX_BUFFER];
    char command[MAX_COMMAND];
    char key[MAX_KEY];
    char value[MAX_VALUE];
    char ttl_str[10];

    while (1) {
        memset(buffer, 0, MAX_BUFFER);
        int bytes = recv(client_fd, buffer, MAX_BUFFER, 0);
        if (bytes <= 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Received: %s\n", buffer);

        memset(command, 0, MAX_COMMAND);
        memset(key, 0, MAX_KEY);
        memset(value, 0, MAX_VALUE);
        memset(ttl_str, 0, 10);

        int parts = sscanf(buffer, "%s %s %s %s", command, key, value, ttl_str);

        if (strcmp(command, "SET") == 0 && parts >= 3) {
            int ttl = (parts == 4) ? atoi(ttl_str) : 0;
            SET(key, value, ttl);
            send(client_fd, "OK\n", strlen("OK\n"), 0);
        } else if (strcmp(command, "GET") == 0 && parts >= 2) {
            GET(key, client_fd);
        } else if (strcmp(command, "DELETE") == 0 && parts >= 2) {
            DELETE(key);
            send(client_fd, "OK\n", strlen("OK\n"), 0);
        } else {
            send(client_fd, "ERROR unknown command\n", strlen("ERROR unknown command\n"), 0);
        }
    }
    close(client_fd);
    return NULL;
}

int main(int argc, char* argv[]) {
    load_from_file();
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int port;

    if (argc == 1) {
        port = DEFAULT_PORT;
    } else if (argc == 2) {
        port = atoi(argv[1]);
        printf("port value is: %d\n", port);
        if (port <= 0 || port > MAX_PORT) {
            printf("Error: invalid port number\n");
            return 1;
        }
    } else {
        printf("Usage: ./server <port>\n");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("ERROR: Cannot Create Socket\n");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        printf("ERROR: Cannot bind to port\n");
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        printf("ERROR: Listening failed\n");
        return 1;
    }
    printf("Listening on port %d...\n", port);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            printf("ERROR: accept failed\n");
            continue;
        }
        printf("Client connected\n");

        int *client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL) {
            close(client_fd);
            continue;
        }
        *client_fd_ptr = client_fd;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, client_fd_ptr);
        pthread_detach(thread);
    }

    close(server_fd);
    return 0;
}