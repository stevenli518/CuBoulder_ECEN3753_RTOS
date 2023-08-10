#include <stdlib.h>
#include "ctest.h"
#include "rr.h"

CTEST_DATA(roundrobin) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(roundrobin) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    int quantum = 2;

    init(data->task, execution, data->size);
    round_robin(data->task, quantum, data->size);
}

CTEST2(roundrobin, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
        ASSERT_EQUAL(i+1, (int)data->task[i].execution_time);//Test initalization
    }
}

// TODO add additional tests to help debug

CTEST2(roundrobin, test_process_T1) {    //For task 1
    ASSERT_EQUAL(0, (int)data->task[0].waiting_time);//Waiting time
    ASSERT_EQUAL(1, (int)data->task[0].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(0, (int)data->task[0].left_to_execute);//Finish Execution
}

CTEST2(roundrobin, test_process_T2) {    //For task 2
    ASSERT_EQUAL(1, (int)data->task[1].waiting_time);//Waiting time
    ASSERT_EQUAL(3, (int)data->task[1].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(0, (int)data->task[1].left_to_execute);//Finish Execution
}

CTEST2(roundrobin, test_process_T3) {    //For task 3

    ASSERT_EQUAL(6, (int)data->task[2].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(3, (int)data->task[2].waiting_time);//Waiting time
    ASSERT_EQUAL(0, (int)data->task[2].left_to_execute);//Finish Execution
}

////////////////////////////////////////////////////////////////////////////////////////////
CTEST_DATA(roundrobin_2) {
    struct task_t task[3];
    int size;
};
CTEST_SETUP(roundrobin_2) {
    int execution[] = {3, 4, 5};
    data->size = sizeof(execution) / sizeof(execution[0]);
    int quantum = 2;

    init(data->task, execution, data->size);
    round_robin(data->task, quantum, data->size);
}

CTEST2(roundrobin_2, test_process_T1) {    //For task 1
    ASSERT_EQUAL(4, (int)data->task[0].waiting_time);//Waiting time
    ASSERT_EQUAL(7, (int)data->task[0].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(0, (int)data->task[0].left_to_execute);//Finish Execution
}

CTEST2(roundrobin_2, test_process_T2) {    //For task 2
    ASSERT_EQUAL(5, (int)data->task[1].waiting_time);//Waiting time
    ASSERT_EQUAL(9, (int)data->task[1].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(0, (int)data->task[1].left_to_execute);//Finish Execution
}

CTEST2(roundrobin_2, test_process_T3) {    //For task 3

    ASSERT_EQUAL(12, (int)data->task[2].turnaround_time);//TUrnaround time
    ASSERT_EQUAL(7, (int)data->task[2].waiting_time);//Waiting time
    ASSERT_EQUAL(0, (int)data->task[2].left_to_execute);//Finish Execution
}