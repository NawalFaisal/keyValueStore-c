#include "keyValue.h"
#include <sys/socket.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
Node* hashtab[HASHSIZE];

/*hashes key from string into slot number */
unsigned int hash(char *s) {
    unsigned int hashval = 0;
    for (; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

void SET(char* key, char* value, int ttl) {
    pthread_mutex_lock(&lock);
    unsigned int slot = hash(key);

    /* check if it exists update if it doesnt*/
    for (Node* current = hashtab[slot]; current != NULL; current = current->next) {
        if (strcmp(current->data.key, key) == 0) {
            strcpy(current->data.value, value);
            current->created_at = time(NULL);
            current->expires_at = (ttl > 0) ? time(NULL) + ttl : 0;
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
    p->created_at = time(NULL);
    p->expires_at = (ttl > 0) ? time(NULL) + ttl : 0;
    p->next = hashtab[slot];
    hashtab[slot] = p;
    save_to_file();
    pthread_mutex_unlock(&lock);
}

void GET(char* key, int client_fd) {
    pthread_mutex_lock(&lock);
    unsigned int slot = hash(key);

    for (Node* current = hashtab[slot]; current != NULL; current = current->next) {
        if (strcmp(current->data.key, key) == 0) {
            /* check if expired before return */
            if (current->expires_at != 0 && time(NULL) > current->expires_at) {
                send(client_fd, "EXPIRED\n", strlen("EXPIRED\n"), 0);
                pthread_mutex_unlock(&lock);
                return;
            }
            send(client_fd, current->data.value, strlen(current->data.value), 0);
            send(client_fd, "\n", 1, 0);
            pthread_mutex_unlock(&lock);
            return;
        }
    }
    send(client_fd, "NOT FOUND\n", strlen("NOT FOUND\n"), 0);
    pthread_mutex_unlock(&lock);
}

void DELETE(char* key) {
    pthread_mutex_lock(&lock);
    unsigned int slot = hash(key);
    Node* previous = NULL;

    for (Node* current = hashtab[slot]; current != NULL; current = current->next) {
        if (strcmp(current->data.key, key) == 0) {
            if (previous == NULL)
                hashtab[slot] = current->next;
            else
                previous->next = current->next;
            free(current);
            save_to_file();
            pthread_mutex_unlock(&lock);
            return;
        }
        previous = current;
    }
    pthread_mutex_unlock(&lock);
}

void save_to_file() {
    FILE* f = fopen("store.txt", "w");
    if (f == NULL) return;
    for (int i = 0; i < HASHSIZE; i++) {
        for (Node* current = hashtab[i]; current != NULL; current = current->next)
            fprintf(f, "%s %s %ld %ld\n",
                current->data.key,
                current->data.value,
                current->created_at,
                current->expires_at);
    }
    fclose(f);
}

void SET_nosave(char* key, char* value, time_t created_at, time_t expires_at) {
    Node* p = malloc(sizeof(Node));
    if (p == NULL) return;
    strcpy(p->data.key, key);
    strcpy(p->data.value, value);
    p->created_at = created_at;
    p->expires_at = expires_at;
    unsigned int slot = hash(key);
    p->next = hashtab[slot];
    hashtab[slot] = p;
}

void load_from_file() {
    FILE* f = fopen("store.txt", "r");
    if (f == NULL) return;
    char key[MAX_KEY], value[MAX_VALUE];
    time_t created_at, expires_at;
    while (fscanf(f, "%s %s %ld %ld", key, value, &created_at, &expires_at) == 4) {
        SET_nosave(key, value, created_at, expires_at);
    }
    fclose(f);
}