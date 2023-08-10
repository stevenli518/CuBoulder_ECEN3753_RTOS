#include <stdlib.h>
#include "ctest.h"
#include "sjf.h"
#include <time.h>

CTEST_DATA(shortestjobfirst) {
    struct task_t task[3];
    int size;
};

CTEST_SETUP(shortestjobfirst) {
    int execution[] = {1, 2, 3};
    data->size = sizeof(execution) / sizeof(execution[0]);

    init(data->task, execution, data->size);
    shortest_job_first(data->task, data->size);
}

CTEST2(shortestjobfirst, test_process_1) {
    for (int i = 0; i < data->size; i++) {
        ASSERT_EQUAL(i, (int)data->task[i].process_id);
    }
}

// TODO add additional tests to help debug
// Waiting Time Test
//Task[0].waitingtime = 0
//Task[1].waitingtime = 1
//Task[2].waitingtime = 3
CTEST2(shortestjobfirst, test_process_2) {
    ASSERT_EQUAL(data->task[0].waiting_time,0);
    ASSERT_EQUAL(data->task[1].waiting_time,data->task[0].execution_time);
    ASSERT_EQUAL(data->task[2].waiting_time,data->task[0].execution_time + data->task[1].execution_time);
}
//Test Turnaround time
//Task[0].turnaroundtime = 1
//Task[1].turnaroundtime = 3
//Task[2].turnaroundtime = 6
CTEST2(shortestjobfirst, test_process_3) {
    ASSERT_EQUAL(data->task[0].turnaround_time,1);
    ASSERT_EQUAL(data->task[1].turnaround_time,data->task[1].execution_time + data->task[0].execution_time);
    ASSERT_EQUAL(data->task[2].turnaround_time,data->task[2].execution_time + data->task[1].execution_time + data->task[0].execution_time );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTEST_DATA(shortestjobfirst2) {
    struct task_t task[4];
    int size;
};

CTEST_SETUP(shortestjobfirst2) {
    int execution[] = {4, 6, 3, 5};
    data->size = sizeof(execution) / sizeof(execution[0]);

    init(data->task, execution, data->size);

    shortest_job_first(data->task, data->size);

}


//task[2] < task[0] < task[3] < task[1]
CTEST2(shortestjobfirst2, test_process_2) {
    ASSERT_EQUAL(data->task[0].waiting_time,3);
    ASSERT_EQUAL(data->task[1].waiting_time,12);
    ASSERT_EQUAL(data->task[2].waiting_time,0);
    ASSERT_EQUAL(data->task[3].waiting_time,7);
}
//Test Turnaround time
//Task[0].turnaroundtime = 7
//Task[1].turnaroundtime = 18
//Task[2].turnaroundtime = 3
//Task[3].turnaroundtime = 12
CTEST2(shortestjobfirst2, test_process_3) {
    ASSERT_EQUAL(data->task[0].turnaround_time,7);
    ASSERT_EQUAL(data->task[1].turnaround_time,18);
    ASSERT_EQUAL(data->task[2].turnaround_time,3);
    ASSERT_EQUAL(data->task[3].turnaround_time,12);
}