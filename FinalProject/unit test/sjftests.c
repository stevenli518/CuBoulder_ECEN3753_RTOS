#include <stdlib.h>
#include "ctest.h"
#include "sjf.h"
#include <time.h>

CTEST_DATA(shortestjobfirst) {
    struct mass ball[3];
    struct wall wall[3];
    int size;
};

CTEST_SETUP(shortestjobfirst) {
    int x_init_pos[] = {20, 30, 40};
    int y_init_pos[] = {20, 20, 20};

    int x_init_vel[] = {5, 10, 15};
    int y_init_vel[] = {5, 15, 15};

    int mass[] = {1,2,3} ;
    data->size = sizeof(x_init_pos) / sizeof(x_init_pos[0]);
    data->wall[0].force=30;
    data->wall[1].force=40;
    data->wall[2].force=50;
    data->wall[0].mass= 10;
    data->wall[1].mass= 10;
    data->wall[2].mass= 10;
    init(data->ball,x_init_pos,y_init_pos,x_init_vel,y_init_vel,mass,data->size);
    final_velocity(data->ball,data->size);
    KE_Calc(data->ball,data->size);
    x_pos_calc(data->ball,0.5,data->size);
    y_pos_calc(data->ball,0.5,data->size);
    x_vel_calc(data->ball,0.5,data->size);
    y_vel_calc(data->ball,0.5,data->size);
    // boost(data->ball, 1.25, data->size);
    // without_boost(data->ball, 0.25, data->size);
     find_accel(data->ball, data->wall, data->size);
}

CTEST2(shortestjobfirst, test_process1) {
    for (int i = 0 ; i<3;i++){
        ASSERT_EQUAL(data->ball[i].mass,i+1);
    }
}

// TODO add additional tests to help debug
//Velocity
CTEST2(shortestjobfirst, test_process2) {

    ASSERT_EQUAL(data->ball[0].final_velocity,7);
    ASSERT_EQUAL(data->ball[1].final_velocity,18);
    ASSERT_EQUAL(data->ball[2].final_velocity,21);
}
//KE
CTEST2(shortestjobfirst, test_process3) {
    ASSERT_EQUAL(data->ball[0].ke,24);
    ASSERT_EQUAL(data->ball[1].ke,324);
    ASSERT_EQUAL(data->ball[2].ke,661);
}
//Y-pos
CTEST2(shortestjobfirst, test_process4) {
    ASSERT_EQUAL(data->ball[0].y_init_pos,22);
    ASSERT_EQUAL(data->ball[1].y_init_pos,27);
    ASSERT_EQUAL(data->ball[2].y_init_pos,27);
}
//X-POS
CTEST2(shortestjobfirst, test_process5) {
    ASSERT_EQUAL(data->ball[0].x_init_pos,22);
    ASSERT_EQUAL(data->ball[1].x_init_pos,35);
    ASSERT_EQUAL(data->ball[2].x_init_pos,47);
}

//X-VEL
CTEST2(shortestjobfirst, test_process6) {
    ASSERT_EQUAL(data->ball[0].x_init_vel,9);
    ASSERT_EQUAL(data->ball[1].x_init_vel,14);
    ASSERT_EQUAL(data->ball[2].x_init_vel,19);
}
//Y-VEL
CTEST2(shortestjobfirst, test_process7) {
    ASSERT_EQUAL(data->ball[0].y_init_vel,9);
    ASSERT_EQUAL(data->ball[1].y_init_vel,19);
    ASSERT_EQUAL(data->ball[2].y_init_vel,19);
}

//Accel
CTEST2(shortestjobfirst, test_process8) {
    ASSERT_EQUAL(data->ball[0].acceleration,3);
    ASSERT_EQUAL(data->ball[1].acceleration,4);
    ASSERT_EQUAL(data->ball[2].acceleration,5);
}


CTEST_DATA(shortestjobfirst2) {
    struct mass ball[3];
    struct wall wall[3];
    int size;
};
CTEST_SETUP(shortestjobfirst2){
    int x_init_pos[] = {20, 30, 40};
    int y_init_pos[] = {20, 20, 20};

    int x_init_vel[] = {5, 10, 15};
    int y_init_vel[] = {5, 15, 15};

    int mass[] = {1,2,3} ;
    data->size = sizeof(x_init_pos) / sizeof(x_init_pos[0]);
    init(data->ball,x_init_pos,y_init_pos,x_init_vel,y_init_vel,mass,data->size);
    boost(data->ball, 1.25, data->size);
}
CTEST2(shortestjobfirst2, test_process1) {
    ASSERT_EQUAL(data->ball[0].y_init_vel,6);
    ASSERT_EQUAL(data->ball[1].y_init_vel,18);
    ASSERT_EQUAL(data->ball[2].y_init_vel,18);

    ASSERT_EQUAL(data->ball[0].x_init_vel,6);
    ASSERT_EQUAL(data->ball[1].x_init_vel,12);
    ASSERT_EQUAL(data->ball[2].x_init_vel,18);
}

CTEST_DATA(shortestjobfirst3) {
    struct mass ball[3];
    struct wall wall[3];
    int size;
};
CTEST_SETUP(shortestjobfirst3){
    int x_init_pos[] = {20, 30, 40};
    int y_init_pos[] = {20, 20, 20};

    int x_init_vel[] = {5, 10, 15};
    int y_init_vel[] = {5, 15, 15};

    int mass[] = {1,2,3} ;
    data->size = sizeof(x_init_pos) / sizeof(x_init_pos[0]);
    init(data->ball,x_init_pos,y_init_pos,x_init_vel,y_init_vel,mass,data->size);
    without_boost(data->ball, 0.25, data->size);
}
CTEST2(shortestjobfirst3, test_process1) {
    ASSERT_EQUAL(data->ball[0].y_init_vel,1);
    ASSERT_EQUAL(data->ball[1].y_init_vel,3);
    ASSERT_EQUAL(data->ball[2].y_init_vel,3);
    
    ASSERT_EQUAL(data->ball[0].x_init_vel,1);
    ASSERT_EQUAL(data->ball[1].x_init_vel,2);
    ASSERT_EQUAL(data->ball[2].x_init_vel,3);
}