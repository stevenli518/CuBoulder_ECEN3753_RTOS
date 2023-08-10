#include <stdlib.h>
#include <stdio.h>
#include "ctest.h"
#include "fcfs.h"

// Note: the name in the first parameter slot must match all tests in that group
CTEST_DATA(firstcomefirstserved) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(firstcomefirstserved) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, data->size);
    first_come_first_served(data->task, data->size);
}

CTEST2(firstcomefirstserved, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug
CTEST2(firstcomefirstserved, test_process_2) {  // Check Average waiting time and waiting time for each task: 
//task[0].waiting = 0
//task[1].waiting = 1
//task[2].watiing = 3
//avg_waiting = 0+1+3/3 =1.3333...
    float avg_wait =  calculate_average_wait_time(data->task, data->size);
    ASSERT_EQUAL(avg_wait, (float)4/(float)3);

    ASSERT_EQUAL((int)data->task[0].waiting_time, 0);
    ASSERT_EQUAL((int)data->task[1].waiting_time, (int)data->task[0].execution_time);
    ASSERT_EQUAL((int)data->task[2].waiting_time, (int)data->task[0].execution_time + (int)data->task[1].execution_time);
}

CTEST2(firstcomefirstserved, test_process_3) {  // Check Average turnaround time and turnaround time for each task: 
//task[0].turnaround = 1
//task[1].turnaround = 3
//task[2].turnaround = 6
//avg_waiting = 1+3+6/3 =3.333...
    float avg_wait =  calculate_average_turn_around_time(data->task, data->size);
    ASSERT_EQUAL((int)data->task[0].turnaround_time, (int)data->task[0].execution_time);
    ASSERT_EQUAL((int)data->task[1].turnaround_time, (int)data->task[0].execution_time+(int)data->task[1].execution_time);
    ASSERT_EQUAL((int)data->task[2].turnaround_time, (int)data->task[0].execution_time+(int)data->task[1].execution_time+(int)data->task[2].execution_time);
    ASSERT_EQUAL(avg_wait, (float)10/(float)3);
}
/*
CTEST2(firstcomefirstserved, test_process_4){
    struct node_t* node1 = data->task[0];
    struct node_t* node2 = 
    struct node_t* node1
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTEST_DATA(firstcomefirstserved_2) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(firstcomefirstserved_2) {
    int execution[] = {10, 6, 4};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, data->size);
    first_come_first_served(data->task, data->size);
}

CTEST2(firstcomefirstserved_2, test_process_2_1) { // Test initialization
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
// Task0.execution time  = 10
// Task1.execution time  = 6 
// Task2.execution time  = 4
    ASSERT_EQUAL(10, (int)data->task[0].execution_time);
    ASSERT_EQUAL(6, (int)data->task[1].execution_time);
    ASSERT_EQUAL(4, (int)data->task[2].execution_time);
// Task0.waiting time  = 0
// Task1.waiting time  = 10 
// Task2.waiting time  = 16
    ASSERT_EQUAL(0, (int)data->task[0].waiting_time);// Task0.waiting time 
    ASSERT_EQUAL((int)data->task[0].execution_time, (int)data->task[1].waiting_time);
    ASSERT_EQUAL((int)data->task[0].execution_time + (int)data->task[1].execution_time, (int)data->task[2].waiting_time);
//Task0. turnaround time = 10
//Task1. turnaround time = 16
//Task2. turnaround time = 20
    ASSERT_EQUAL((int)data->task[0].turnaround_time, (int)data->task[0].execution_time);
    ASSERT_EQUAL((int)data->task[1].turnaround_time, (int)data->task[0].execution_time+(int)data->task[1].execution_time);
    ASSERT_EQUAL((int)data->task[2].turnaround_time, (int)data->task[0].execution_time+(int)data->task[1].execution_time+(int)data->task[2].execution_time);
}