
#ifndef __SHORTEST_JOB_FIRST__
#define __SHORTEST_JOB_FIRST__
//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Task information
//----------------------------------------------------------------------------------------------------------------------------------
#include <math.h>
struct mass
{
    //ball's init x-axis position
    int x_init_pos;
    //ball's init y-axis position
    int y_init_pos;

    //ball's init x-axis velocity
    int x_init_vel;
    //ball's init y-axis velocity
    int y_init_vel;
    //sqrt(x^2+y^2)
    int final_velocity;
    //Default Acceleration:9
    int acceleration;
    //int KE
    int ke;
    //ball's mass
    int mass;
    
};
struct wall
{
    //ball's init x-axis position
    int force;
    //ball mass
    int mass;
    
};
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

#endif // __SHORTEST_JOB_FIRST__