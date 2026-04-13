#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include "keyValue.h"




Node* head = NULL;

int main(int argc, char* argv[]) {
    load_from_file();
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER];
    int port;

    // command line argument

    if (argc == 1) {
    port = define;}
    
    else if (argc == 2) {
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




    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    // listen
    listen(server_fd, 3);
    printf("Listening on port %d...\n", port);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("Client connected\n");

        pid_t pid = fork();
        if (pid == 0) {
            // child processes handles clients
            close(server_fd);
            while (1) {
                memset(buffer, 0, MAX_BUFFER);
                int bytes = recv(client_fd, buffer, MAX_BUFFER, 0);
                if (bytes <= 0) {
                    printf("Client disconnected\n");
                    break;
                }
                printf("Received: %s\n", buffer);
                load_from_file();

                char command[MAX_COMMAND], key[MAX_KEY], value[MAX_VALUE];
                memset(command, 0, MAX_COMMAND);
                memset(key, 0, MAX_KEY);
                memset(value, 0, MAX_VALUE);

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