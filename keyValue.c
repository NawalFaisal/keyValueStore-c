#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
 char key[50];
 char value[100];
} keyValue;


typedef struct Node{
keyValue data;
struct Node* next;
}Node;

Node* head = NULL;
void save_to_file();
void load_from_file();

void SET(char* key, char* value){
Node* p = malloc(sizeof(Node));
strcpy(p-> data.key, key);
strcpy(p->data.value,value);
p->next = head;
head = p;
save_to_file();

}

void GET(char* key) {
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->data.key, key) == 0) {
            printf("%s\n", current->data.value);
            return;
        }
        current = current->next;
    }
    printf("NOT FOUND\n");
}

void DELETE(char* key) {
    Node* current = head;
    Node* previous = NULL;
    while (current != NULL) {
        if (strcmp(current->data.key, key) == 0) {
            if (previous == NULL) {
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            save_to_file();
            return;
        }
        previous = current;
        current = current->next;
    }
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

void load_from_file() {
    FILE* f = fopen("store.txt", "r");
    if (f == NULL) return;
    char key[50], value[100];
    while (fscanf(f, "%s %s", key, value) == 2) {
        SET(key, value);
    }
    fclose(f);
}

int main() {
    load_from_file();
    SET("name", "Nawal");
    SET("city", "Calgary");

    GET("name");     
    GET("city");      
    
    DELETE("name");
  
    GET("name");    

    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    return 0;
}


