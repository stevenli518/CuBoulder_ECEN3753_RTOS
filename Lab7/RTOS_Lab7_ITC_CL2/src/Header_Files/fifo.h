/*
 * fifo.h
 *
 *  Created on: 2022年3月1日
 *      Author: Chengming Li
 */

#ifndef SRC_HEADER_FILES_FIFO_H_
#define SRC_HEADER_FILES_FIFO_H_
#include <stdio.h>
#include <stdlib.h>
struct fifo_t{
	int size;
	struct node_t *head;
};

struct node_t{
	int btn;
	struct node_t *next;
};

struct speed_t{
	int curr_speed;
	int cnt_inc;
	int cnt_dec;
};
struct direction_t{
	int curr_dir;
	int time_in_total;
	int left_cnt;
	int right_cnt;
	int pad_state;
};
//struct fifo_t* create_fifo(int size);
struct node_t* create_new_node(int btn);
struct node_t* peek(struct node_t* head_of_ll);
void pop(struct node_t** head);
void push(struct node_t** head, int btn);
int is_empty(struct node_t* head);
void empty_queue(struct node_t* head);


#endif /* SRC_HEADER_FILES_FIFO_H_ */
