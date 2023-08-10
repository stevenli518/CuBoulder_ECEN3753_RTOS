#include "sjf.h"
#include "queue.h"
#include <stdio.h>

void init(struct task_t *task, int *execution, int size) {
    for (int i = 0; i<size ; i++){
        task[i].execution_time = execution[i];
        task[i].process_id = i;
        task[i].turnaround_time = 0;
        task[i].waiting_time = 0;
    }
}
//Citation: Adapted from GeeksforGeeks
void merge(struct task_t * task,int l,int m, int h){
    int i,j,k;
    int s1 = m-l+1;
    int s2 = h-m;
    struct task_t task1[s1];
    struct task_t task2[s2];

    for(int i = 0; i< s1; i++){
        task1[i] = task[l+i]; 
    }
    for(int j= 0; j< s2;j++){
        task2[j] = task [m+1+j]; 
    }
    i = 0;
    j = 0;
    k = l;
    while(i<s1 && j <s2){
        if(task1[i].execution_time<= task2[j].execution_time){
            task[k] = task1[i];
            i++;
        }
        else{
            task[k] = task2[j];
            j++;
        }
        k++;
    }
    while(i < s1){
        task[k] = task1[i];
        i++;
        k++;
    }
    while(j < s2){
        task[k] = task2[j];
        j++;
        k++;
    }
}
void mergeSort(struct task_t * task, int l, int h){
    if(l >= h){
        return;
    }
    int m = (l+h)/2;
    mergeSort(task, l,m);
    mergeSort(task,m+1,h);
    merge(task,l,m,h);
}
void shortest_job_first(struct task_t *task, int size) {
    // Hints:
    // 1. Create Queue based on the task array in the correct order
    // 2. Process each "task" until completion
    // 3. You can process by popping items from the queue
    struct task_t task_temp[size] ;
    for(int i = 0;i<size;i++){
        task_temp[i] = task[i];
    }
    mergeSort(task_temp,0,size-1);
    struct node_t* node = create_queue(task_temp, size);
    int total_time = 0;
    struct node_t* temp = NULL;

    while(node!= NULL){
        temp = node->next;
        node->task->waiting_time= total_time;
        total_time += node->task->execution_time;
        node->task->turnaround_time = total_time;
        task[node->task->process_id].turnaround_time = node->task->turnaround_time;
        task[node->task->process_id].waiting_time = node->task->waiting_time;
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