/*
 * fifo.c
 *
 *  Created on: 2022年3月1日
 *      Author: Chengming Li
 */
#include "fifo.h"
//-------------------------------------------------------
//struct fifo_t* create_fifo(int size);
struct node_t* create_new_node(int btn);
struct node_t* peek(struct node_t* head_of_ll);
void pop(struct node_t**head);
void push(struct node_t**head, int btn);
int is_empty(struct node_t* head);
void empty_queue(struct node_t* head);
//-------------------------------------------------------

//struct fifo_t* create_fifo(int size){
//
//	struct fifo_t* new_fifo = (struct fifo_t*)malloc( sizeof(struct fifo_t));
//	new_fifo->size = size;
//	new_fifo->head = NULL;
//
//	return new_fifo;
//
//}
struct node_t* create_new_node(int btn){
    struct node_t* temp = (struct node_t*)malloc( sizeof(struct node_t));
    temp->btn = btn;
    temp->next = NULL;
    return temp;
}
struct node_t* peek(struct node_t* head_of_ll){
	if(head_of_ll == NULL){
		return NULL;
	}
	else{
		return head_of_ll;
	}
	return NULL;
}
void pop(struct node_t**head){
    if(is_empty(*head)){
        return;
    }
    else{
        struct node_t* curr = (*head)->next;
        free(*head);
        *head = curr;
    }

}
void push(struct node_t**head, int btn){
    if(*head == NULL){
    	struct node_t * node_c = create_new_node(btn);
    	*head = node_c;
    	return;
    }
    struct node_t* curr = *head;
    while (curr->next != NULL){
        curr = curr->next;
    }

    struct node_t * node_c = create_new_node(btn);
    curr->next = node_c;
}
int is_empty(struct node_t* head){
    // return 0 so it compiles
    if(head  == NULL){
        return 1;
    }
    return 0;
}
void empty_queue(struct node_t* head){
    if(is_empty(head)){
        return;
    }
    struct node_t* curr = head;
    while(curr != NULL){
        struct node_t * temp = curr->next;
        free(curr);
        curr = temp;
    }
}
