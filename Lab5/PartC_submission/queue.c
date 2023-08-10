#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct node_t* create_queue(struct task_t* task, int size) {
    // return null so it compiles
    struct node_t* node_head = create_new_node((&task[0]));
    struct node_t* curr;
    curr = node_head;
    int i = 1;
    while (i < size){
        struct node_t * node_c = create_new_node((&task[i]));
        curr->next  = node_c;       // Link the node
        curr = node_c;              // Update curr node
        i = i+1;
    }
    
    
    return node_head;
}

struct node_t* create_new_node(struct task_t* task) {
    // return null so it compiles
    struct node_t* node = (struct node_t*)malloc( sizeof(struct node_t));
    node-> task = task;
    node->next = NULL;
    return node;
}

struct task_t* peek(struct node_t** head) {
    // return null so it compiles
    if ((is_empty(head))){
        return NULL;
    }
    else {
        return head[0]->task;
    }
    return NULL;
}

void pop(struct node_t** head) {
    if(is_empty(head)){
        return;
    }
    else{
        struct node_t* curr = head[0]->next;
        free(head[0]);
        head[0] = curr;
    }
}

void push(struct node_t** head, struct task_t* task) {
    if(is_empty(head)){
        return;
    }

    struct node_t* curr = head[0];

    while (curr->next != NULL){
        curr = curr->next;
    }
    struct node_t * node_c = create_new_node(task);
    curr->next = node_c;


 
    
}

int is_empty(struct node_t** head) {
    // return 0 so it compiles
    if(head[0]  == NULL){
        return 1;
    }
    return 0;
   
}

void empty_queue(struct node_t** head) {
    if(is_empty(head)){
        return;
    }
    struct node_t* curr = head[0];
    while(curr != NULL){
        struct node_t * temp = curr->next;
        free(curr);
        curr = temp;
    }

}

