#include "priority.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int *priority, int size) {
    for (int i = 0; i<size ; i++){
        task[i].execution_time = execution[i];
        task[i].left_to_execute = execution[i];
        task[i].priority = priority[i];
        task[i].process_id = i;
        task[i].turnaround_time = 0;
        task[i].waiting_time = 0;
    }
}
void priority_update(struct node_t* node, int time_insys){
    if (node->task->execution_time == time_insys){
        node->task->priority = node->task->priority * 4;
    }
    if (node->task->left_to_execute == time_insys){
        node->task->priority = node->task->priority * 2;
    }
}
//Citation:Adapted from Geeks for Geeks
void swap (struct node_t* node1, struct node_t* node2){

    struct task_t * temp_task = node1->task ;
    node1->task = node2->task;
    node2->task = temp_task;
}
void bubble_sort (struct node_t * node){
    int flag;
    struct node_t* temp;
    struct node_t* lnode = NULL;
    if(node == NULL){
        return;
    }
    do
    {
        flag = 0;
        temp = node;
        while(temp->next != lnode ){
            if(temp->task->priority < temp->next->task->priority){
                swap(temp,temp->next);
                flag = 1;
            }
            temp =temp->next;
        }
        lnode = temp;
    } while (flag);
    

}
void print(struct task_t *task,int size){
    for(int i = 0;i<size;i++){
        printf("\ntask id  = %d, priority = %d",task[i].process_id,task[i].priority);
    }

}
void print_node(struct node_t * node){
    printf("\ntask[%d] turnaround time = %d, left_to_execute =%d, priority = %d", node->task->process_id,node->task->turnaround_time,node->task->left_to_execute,node->task->priority);
}
void priority_schedule(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Each task can be processed for a time interval of 1 (i.e quantum time of 1)
    // 3. You can process by pushing and popping items from the queue
    // 4. You must recalculate the priorities after every turn
    //print(task,size);
    struct node_t* node = create_queue(task, size);
    bubble_sort(node);
    int total_time = 0;
    struct node_t* temp = NULL;
    while (node != NULL){

        if (node->task->left_to_execute > 1){
            node->task->left_to_execute -= 1;           //Execute
            total_time += 1;
            priority_update(node,total_time);           //Updatae Priority
            push(&node, node->task);                    //Push current node to the end of list
            temp = node->next;                          //Store node->next information
            pop(&node);                                 //current Finish execution for 1 cycle
            node =temp;                                 //Go to next node
            bubble_sort(node);                          //Rearrange the list
        } 
        else{
            node->task->left_to_execute -= 1;           //Base case
            total_time += 1;                            //Execute
            node->task->turnaround_time = total_time;   //Finding turnaround time
            node->task->waiting_time = node->task->turnaround_time - node->task->execution_time;//And Finding waiting time
            temp = node->next;                          //Store node->next information
            pop(&node);                                 //Pop current node
            node = temp;                                //Go to next node
            
        }
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