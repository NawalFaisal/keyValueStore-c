#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

extern pthread_mutex_t lock;
typedef struct {
    char key[50];
    char value[100];
} keyValue;

typedef struct Node {
    keyValue data;
    struct Node* next;
} Node;

extern Node* head;

void SET(char* key, char* value);
void GET(char* key, int client_fd);
void DELETE(char* key);
void save_to_file();
void load_from_file();
void SET_nosave(char* key, char* value);

#endif