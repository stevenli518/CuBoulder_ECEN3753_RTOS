#include "sjf.h"
#include "queue.h"
#include <stdio.h>

// void init(struct task_t *task, int *execution, int size) {

// }

// void shortest_job_first(struct task_t *task, int size) {
//     // Hints:
//     // 1. Create Queue based on the task array in the correct order
//     // 2. Process each "task" until completion
//     // 3. You can process by popping items from the queue
// }

// float calculate_average_wait_time(struct task_t *task, int size) {
//     // return 0.0 so it compiles
//     return 0.0;
// }

// float calculate_average_turn_around_time(struct task_t *task, int size) {
//     // return 0.0 so it compiles
//     return 0.0;
// }
void init(struct mass* ball, int* x_pos,int* y_pos,int*x_vel,int* y_vel,int* masses,int size);
void final_velocity(struct mass* ball,int size );
void KE_Calc(struct mass* ball,int size);
void x_vel_calc(struct mass* ball, float time,int size);
void y_vel_calc(struct mass* ball, float time, int size);

void x_pos_calc(struct mass* ball, float time,int size);
void y_pos_calc(struct mass* ball, float time, int size);
void boost(struct mass* ball, float percent_change, int size);
void without_boost(struct mass* ball, float percent_change, int size);
void find_accel(struct mass* ball, struct wall * wall, int size);

void init(struct mass* ball, int* x_pos,int* y_pos,int*x_vel,int* y_vel,int* masses,int size){
    for(int i = 0; i< size; i++){
        ball[i].x_init_pos = x_pos[i];
        ball[i].y_init_pos = y_pos[i];
        ball[i].x_init_vel = x_vel[i];
        ball[i].y_init_vel = y_vel[i];
        ball[i].mass = masses[i];
        ball[i].acceleration = 9;
    }
}

void final_velocity(struct mass* ball,int size ){
    for(int i = 0; i< size; i++){
        ball[i].final_velocity = sqrt(ball[i].x_init_vel*ball[i].x_init_vel+ ball[i].y_init_vel*ball[i].y_init_vel);
        //printf("The final velocity is %d\n", ball[i].final_velocity);
    }   
}

void KE_Calc(struct mass* ball,int size){
    for(int i = 0; i< size; i++){
        ball[i].ke = 0.5*ball[i].mass*ball[i].final_velocity*ball[i].final_velocity;
        //printf("The final velocity is %d\n", ball[i].ke);
    }
}
void x_vel_calc(struct mass* ball, float time,int size){
    for(int i = 0; i< size; i++){
       ball[i].x_init_vel =  ball[i].x_init_vel + ball[i].acceleration*time;

    }

}
void y_vel_calc(struct mass* ball, float time,int size){
    for(int i = 0; i< size; i++){
        ball[i].y_init_vel = ball[i].y_init_vel + ball[i].acceleration*time;
               
    }
}

void x_pos_calc(struct mass* ball, float time,int size){
    for(int i = 0; i< size; i++){
        ball[i].x_init_pos = ball[i].x_init_pos + ball[i].x_init_vel*time;


    }

}
void y_pos_calc(struct mass* ball, float time ,int size){
    for(int i = 0; i< size; i++){
        ball[i].y_init_pos = ball[i].y_init_pos + ball[i].y_init_vel*time;
    }
}
void boost(struct mass* ball, float percent_change, int size){
       for(int i = 0; i< size; i++){
        ball[i].x_init_vel = ball[i].x_init_vel*percent_change;
        ball[i].y_init_vel = ball[i].y_init_vel*percent_change;


    }
}
void without_boost(struct mass* ball, float percent_change, int size){
       for(int i = 0; i< size; i++){
        ball[i].x_init_vel = ball[i].x_init_vel*percent_change;
        ball[i].y_init_vel = ball[i].y_init_vel*percent_change;
        printf("x_vel is %d, y_vel is %d\n", ball[i].x_init_vel ,ball[i].y_init_vel);
    }
}
void find_accel(struct mass* ball, struct wall * wall, int size){
       for(int i = 0; i< size; i++){
           ball[i].acceleration = wall[i].force/wall[i].mass;
    }
}