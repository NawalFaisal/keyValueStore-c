#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//handle magic numbers
#define MAX_BUFFER 256
#define MAX_PORT 65535
#define MAX_VALUE 100
#define MAX_KEY 50
#define MAX_COMMAND 10
#define DEFAULT_PORT 8080
#define BACKLOG 3

//external variable declaration. 
extern pthread_mutex_t lock;

typedef struct {
    char key[MAX_KEY];
    char value[MAX_VALUE];
} keyValue;

typedef struct Node {
    keyValue data;
    struct Node* next;
} Node;

extern Node* head;


// declarations of functions.
void SET(char* key, char* value);
void GET(char* key, int client_fd);
void DELETE(char* key);
void save_to_file();
void load_from_file();
void SET_nosave(char* key, char* value);

#endif