#include "keyValue.h"
#include <sys/socket.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
Node* hashtab[HASHSIZE];

void SET(char* key, char* value){
    pthread_mutex_lock(&lock);
     unsigned int slot = hash(key); 
    for( Node* current = hashtab[slot]; current != NULL; current = current-> next){
        if (strcmp(current->data.key, key) == 0) {
            strcpy(current->data.value, value);
            save_to_file();
            pthread_mutex_unlock(&lock);
            return;
        }
    }

        Node* p = malloc(sizeof(Node));
        if (p == NULL) {
        pthread_mutex_unlock(&lock);
        return;
        }
        strcpy(p->data.key, key);
        strcpy(p->data.value, value);
        p->next = hashtab[slot];   
        hashtab[slot] = p;        
        save_to_file();
        pthread_mutex_unlock(&lock);


}

    //hash function needs to convert the key string into a number
    //remember a string is just an array of chars. if you want to pass an array into a function just use the pointer to the first element
    unsigned int hash(char *s){
        //this will hold the computated hash number the slot index set it to zero
       unsigned int hashval = 0;
        //create a loop.
        for ( ; *s != '\0'; s++)
            hashval = *s + 31 *hashval;

        return hashval % HASHSIZE;

     }

void GET(char* key, int client_fd) {
    pthread_mutex_lock(&lock);
    unsigned int slot = hash(key);
    for ( Node* current = hashtab[slot]; current != NULL; current = current -> next) {
        if (strcmp(current->data.key, key) == 0) {
            send(client_fd, current->data.value, strlen(current->data.value), 0);
            send(client_fd, "\n", 1, 0);
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    send(client_fd, "NOT FOUND\n", 10, 0);
    pthread_mutex_unlock(&lock);
}

void DELETE(char* key) {
    pthread_mutex_lock(&lock);
    unsigned int slot = hash(key);  
    Node* previous = NULL;
    for (Node* current = hashtab[slot]; current != NULL; current = current -> next){
        if (strcmp(current->data.key, key) == 0) {
            if (previous == NULL) {
                hashtab[slot] = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            save_to_file();
            pthread_mutex_unlock(&lock);
            return;
        }
        previous = current;
    }
    pthread_mutex_unlock(&lock);
}
//File persistence functions
void save_to_file() {
    FILE* f = fopen("store.txt", "w");
    if (f == NULL) return;
    for (int i = 0; i < HASHSIZE; i++) {
        for (Node* current = hashtab[i]; current != NULL; current = current->next)
            fprintf(f, "%s %s\n", current->data.key, current->data.value);
    }
    fclose(f);
}


void SET_nosave(char* key, char* value) {
    Node* p = malloc(sizeof(Node));
    strcpy(p->data.key, key);
    strcpy(p->data.value, value);
    unsigned int slot = hash(key);
    p->next = hashtab[slot];
    hashtab[slot] = p;
} 

void load_from_file() {
    FILE* f = fopen("store.txt", "r");
    if (f == NULL) return;
    char key[MAX_KEY], value[MAX_VALUE];
    while (fscanf(f, "%s %s", key, value) == 2) {
        SET_nosave(key, value);
    }
    fclose(f);
}



