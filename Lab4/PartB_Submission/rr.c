#include "rr.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) {
    for (int i = 0; i<size ; i++){
        task[i].execution_time = execution[i];
        task[i].left_to_execute = execution[i];
        task[i].process_id = i;
        task[i].turnaround_time = 0;
        task[i].waiting_time = 0;
    }
}

void round_robin(struct task_t *task, int quantum, int size) {
    // Hints:
    // 1. Create Queue based on the task array
    // 2. Process each "task" in round robin fashion
    // 3. You can process by pushing and popping items from the queue
    struct node_t* node = create_queue(task, size);

    int total_time = 0;
    struct node_t* temp = NULL;
    print("\n 1");
    while (node!=NULL){
        
        if(node->task->left_to_execute <= quantum ){
            total_time += node->task->left_to_execute;
            node->task->turnaround_time = total_time;
            node->task->waiting_time = node->task->turnaround_time -node->task->execution_time;
            node->task->left_to_execute = 0;
            temp = node->next;
            pop(&node);
        }
        else{
            node->task->left_to_execute -= quantum;
            total_time += quantum;
            node->task->turnaround_time = total_time;
            push(&node, node->task);
            temp = node->next;
            pop(&node);
            
        }
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