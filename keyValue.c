#include "keyValue.h"
#include <sys/socket.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void SET(char* key, char* value){
    pthread_mutex_lock(&lock);
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->data.key, key) == 0) {
            strcpy(current->data.value, value);
            save_to_file();
            pthread_mutex_unlock(&lock);
            return;
        }
        current = current->next;
    }
    Node* p = malloc(sizeof(Node));
    strcpy(p->data.key, key);
    strcpy(p->data.value, value);
    p->next = head;
    head = p;
    save_to_file();
    pthread_mutex_unlock(&lock);
}

void GET(char* key, int client_fd) {
    pthread_mutex_lock(&lock);
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->data.key, key) == 0) {
            send(client_fd, current->data.value, strlen(current->data.value), 0);
            send(client_fd, "\n", 1, 0);
            pthread_mutex_unlock(&lock);
            return;
        }
        current = current->next;
    }
    send(client_fd, "NOT FOUND\n", 10, 0);
    pthread_mutex_unlock(&lock);
}

void DELETE(char* key) {
    pthread_mutex_lock(&lock);
    Node* current = head;
    Node* previous = NULL;
    while (current != NULL) 
    {
        if (strcmp(current->data.key, key) == 0) {
            if (previous == NULL) {
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            save_to_file();
            pthread_mutex_unlock(&lock);
            return;
        }
        previous = current;
        current = current->next;
    }
    pthread_mutex_unlock(&lock);
}
//File persistence functions
void save_to_file() {
    FILE* f = fopen("store.txt", "w");
    if (f == NULL) return;
    Node* current = head;
    while (current != NULL) {
        fprintf(f, "%s %s\n", current->data.key, current->data.value);
        current = current->next;
    }
    fclose(f);
}

void SET_nosave(char* key, char* value) {
    Node* p = malloc(sizeof(Node));
    strcpy(p->data.key, key);
    strcpy(p->data.value, value);
    p->next = head;
    head = p;
} 

void load_from_file() {
    FILE* f = fopen("store.txt", "r");
    if (f == NULL) return;
    char key[50], value[100];
    while (fscanf(f, "%s %s", key, value) == 2) {
        SET_nosave(key, value);
    }
    fclose(f);
}



