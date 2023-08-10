/*
 * task.h
 *
 *  Created on: Mar 29, 2022
 *      Author: LiChe
 */

#ifndef SRC_HEADER_FILES_TASK_H_
#define SRC_HEADER_FILES_TASK_H_

#define SHIELD_TASK_STACK_SIZE     256

#define SHIELD_TASK_PRIO            6

#define SLIDER_TASK_STACK_SIZE      512

#define SLIDER_TASK_PRIO            3

#define PHYSICS_TASK_STACK_SIZE      256

#define PHYSICS_TASK_PRIO            7

#define IDLE_TASK_STACK_SIZE      256

#define IDLE_TASK_PRIO            30

#define LED_TASK_STACK_SIZE      512

#define LED_TASK_PRIO            20

#define DISPLAY_TASK_STACK_SIZE      256

#define DISPLAY_TASK_PRIO            4

//***********************************************************************************
// defined files
//***********************************************************************************
#define PAD0  0
#define PAD1  1
#define PAD2  2
#define PAD3  3
#define ENABLE 1
#define CLEAR 0xffffffff
#define PRESS 0
#define NPRESS 1

//***********************************************************************************
// Data structure
//***********************************************************************************
struct boost{
	float ke_increase_percent;
	uint32_t arming_time;//ms
	uint32_t arming_window;//ms
	uint32_t recharge_time;//ms
	bool ready;
	bool launch;
};
struct laser{
	int num;
};
struct shield{
	float ke_reduce_percent;
	struct boost charge;
	struct laser weapon;
};
struct masses{
	int num;
	int Diameter;	//cm
//	int init_conditions;	//0.127 + are available for user_defined modes
//	signed int init_x_vel;	//cm/s
//	signed int init_y_vel;	//cm/s
//	signed int init_h_pos;		//mm
	//struct masses input[8];
    //ball's init x-axis position
    int x_init_pos;
    //ball's init y-axis position
    int y_init_pos;

    //ball's init x-axis velocity
    signed int x_init_vel;
    //ball's init y-axis velocity
    signed int y_init_vel;
    //sqrt(x^2+y^2)
    signed int final_velocity;
    //Default Acceleration:9
    int acceleration_x;
    int acceleration_y;
    //int KE
    int ke;
    //ball's mass
    int mass;
};
struct bounce{
	bool enabled;
	bool limited;
	int bounce_speed;	//cm/s
};
struct platform{//slider
	signed int maxforce;	//N
	int mass;		//Kg
	int length;		//cm
	struct bounce walls;
	bool auto_ctl;	//T/F
    //ball's init x-axis velocity
    signed int x_init_vel;
    //ball's init y-axis velocity

    signed int acceleration;
    //platform's init x-axis position
    int x_init_pos_l;
    //platform's init y-axis position
    int x_init_pos_r;
};
//***********************************************************************************
// Function Declaration
//***********************************************************************************
void itc_init(void);
void interrupt_init(void);
void blink_init(void);
void data_str_init(void);
#endif /* SRC_HEADER_FILES_TASK_H_ */
