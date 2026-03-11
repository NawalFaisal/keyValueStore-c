#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "keyValue.h"

Node* head = NULL;

int main() {
    load_from_file();
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[256];

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // listen
    listen(server_fd, 3);
    printf("Listening on port 8080...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("Client connected\n");

        pid_t pid = fork();
        if (pid == 0) {
            // child processes handles clients
            close(server_fd);
            while (1) {
                memset(buffer, 0, 256);
                int bytes = recv(client_fd, buffer, 256, 0);
                if (bytes <= 0) {
                    printf("Client disconnected\n");
                    break;
                }
                printf("Received: %s\n", buffer);
                load_from_file();

                char command[10], key[50], value[100];
                memset(command, 0, 10);
                memset(key, 0, 50);
                memset(value, 0, 100);

                int parts = sscanf(buffer, "%s %s %s", command, key, value);

                if (strcmp(command, "SET") == 0 && parts == 3) {
                    SET(key, value);
                    send(client_fd, "OK\n", 3, 0);
                } else if (strcmp(command, "GET") == 0 && parts == 2) {
                    GET(key, client_fd);
                } else if (strcmp(command, "DELETE") == 0 && parts == 2) {
                    DELETE(key);
                    send(client_fd, "OK\n", 3, 0);
                } else {
                    send(client_fd, "ERROR unknown command\n", 22, 0);
                }
            }
            close(client_fd);
            exit(0);
        } else {
            // parent process goes back to accepting
            close(client_fd);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}