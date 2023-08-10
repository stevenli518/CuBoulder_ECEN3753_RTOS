#include "fcfs.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) {
    for (int i = 0; i<size ; i++){
        task[i].process_id = i;
        task[i].execution_time = execution[i];
        task[i].waiting_time = 0;
        task[i].turnaround_time = 0;
    }

}

void first_come_first_served(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array
    // 2. Pop off the front item until there are no more tasks
    struct node_t* node = create_queue(task, size);
    int total_time = 0;
    while (is_empty(&node) != 1){
        node->task->waiting_time = total_time;
        total_time += node->task->execution_time;
        node->task->turnaround_time = total_time;
        struct node_t* temp = node->next;
        pop(&node);
        node = temp;
    }
    
}

float calculate_average_wait_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    float avg = 0;
    for (int i = 0; i< size; i++){
        avg += task[i].waiting_time;
    }
    avg = avg/size;
    return avg;
}

float calculate_average_turn_around_time(struct task_t *task, int size) {
    // return 0.0 so it compiles
    float avg = 0;
    for (int i = 0; i< size; i++){
        avg += task[i].turnaround_time;
    }
    avg = avg/size;
    return avg;
    return 0.0;
}