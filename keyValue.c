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
void SET(char* key, char* value){
Node* p = malloc(sizeof(Node));
strcpy(p-> data.key, key);
strcpy(p->data.value,value);
p->next = head;
head = p;

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
            return;
        }
        previous = current;
        current = current->next;
    }
}

int main() {
    SET("name", "Nawal");
    SET("city", "Calgary");
    
    GET("name");     
    GET("city");      
    
    DELETE("name");
    
    GET("name");      
    
    return 0;
}


