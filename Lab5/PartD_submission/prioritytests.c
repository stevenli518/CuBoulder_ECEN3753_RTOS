#include <stdlib.h>
#include "ctest.h"
#include "priority.h"
#include <stdio.h>
CTEST_DATA(priority) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority) {
    int execution[] = {1, 2, 3};
    int priority[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}

CTEST2(priority, test_process) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug
//Waiting time:task[2] > task[1]>task[0]
CTEST2(priority, test_process_1) {
    ASSERT_EQUAL(data->task[0].waiting_time,5);
    ASSERT_EQUAL(data->task[1].waiting_time,3);
    ASSERT_EQUAL(data->task[2].waiting_time,0);
}
CTEST2(priority, test_process_2) {
    ASSERT_EQUAL(data->task[0].turnaround_time,6);
    ASSERT_EQUAL(data->task[1].turnaround_time,5);
    ASSERT_EQUAL(data->task[2].turnaround_time,3 );
}

CTEST_DATA(priority_2) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority_2) {
    int execution[] = {3, 1, 3};
    int priority[] = {3, 4, 1};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}
//Task[1] -> task[0]->task[0]->task[0]->task[2]->task[2]->task[2]
CTEST2(priority_2, test_process_1) {
    ASSERT_EQUAL(data->task[0].turnaround_time,4);
    ASSERT_EQUAL(data->task[1].turnaround_time,1);
    ASSERT_EQUAL(data->task[2].turnaround_time,7 );
}
CTEST2(priority_2, test_process_2) {
    ASSERT_EQUAL(data->task[0].waiting_time,1);
    ASSERT_EQUAL(data->task[1].waiting_time,0);
    ASSERT_EQUAL(data->task[2].waiting_time,4 );
}
CTEST2(priority_2, test_process_3) {
    ASSERT_EQUAL(data->task[0].priority,24);
    ASSERT_EQUAL(data->task[1].priority,4);
    ASSERT_EQUAL(data->task[2].priority,1 );
}

CTEST_DATA(priority_3) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority_3) {
    int execution[] = {3, 1, 3};
    int priority[] = {2, 2, 2};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}
//Task[0] -> task[1]->task[2]->task[2]->task[2]->task[0]->task[0]
CTEST2(priority_3, test_process_1) {
    ASSERT_EQUAL(data->task[0].turnaround_time,7);
    ASSERT_EQUAL(data->task[1].turnaround_time,2);
    ASSERT_EQUAL(data->task[2].turnaround_time,5 );
}
CTEST2(priority_3, test_process_2) {
    ASSERT_EQUAL(data->task[0].waiting_time,4);
    ASSERT_EQUAL(data->task[1].waiting_time,1);
    ASSERT_EQUAL(data->task[2].waiting_time,2 );
}
CTEST2(priority_3, test_process_3) {
    ASSERT_EQUAL(data->task[0].priority,2);
    ASSERT_EQUAL(data->task[1].priority,2);
    ASSERT_EQUAL(data->task[2].priority,8 );
}

CTEST_DATA(priority_4) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(priority_4) {
    int execution[] = {5, 2, 2};
    int priority[] = {1, 3, 2};
    data->size = sizeof(execution) / sizeof(execution[0]);
    init(data->task, execution, priority, data->size);
    priority_schedule(data->task, data->size);
}
//Task[1]-> task[1]->task[2]->task[2]->task[0]->task[0]->task[0]->task[0]->task[0]
CTEST2(priority_4, test_process_1) {
    ASSERT_EQUAL(data->task[0].turnaround_time,9);
    ASSERT_EQUAL(data->task[1].turnaround_time,2);
    ASSERT_EQUAL(data->task[2].turnaround_time,4 );
}
CTEST2(priority_4, test_process_2) {
    ASSERT_EQUAL(data->task[0].waiting_time,4);
    ASSERT_EQUAL(data->task[1].waiting_time,0);
    ASSERT_EQUAL(data->task[2].waiting_time,2 );
}
CTEST2(priority_4, test_process_3) {
    ASSERT_EQUAL(data->task[0].priority,4);
    ASSERT_EQUAL(data->task[1].priority,6);
    ASSERT_EQUAL(data->task[2].priority,2 );
}