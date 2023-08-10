/*
 * task.c
 *
 *  Created on: Mar 29, 2022
 *      Author: LiChe
 */

#include "capsense.h"
#include "glib.h"
#include "dmd.h"
#include "task.h"
#include "os.h"
#include "em_emu.h"
#include "cmu.h"
#include "gpio.h"
#include <math.h>
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
//----------------------------------------------------------------------------
//TCB
//----------------------------------------------------------------------------
static OS_TCB tcb_shield;
static OS_TCB tcb_slider;
static OS_TCB tcb_physics;
//static OS_TCB tcb_led;
static OS_TCB tcb_display;
static OS_TCB tcb_idle;

static CPU_STK stack_shield[SHIELD_TASK_STACK_SIZE];
static CPU_STK stack_slider[SLIDER_TASK_STACK_SIZE];
static CPU_STK stack_physics[PHYSICS_TASK_STACK_SIZE];
//static CPU_STK stack_led[LED_TASK_STACK_SIZE];
static CPU_STK stack_display[DISPLAY_TASK_STACK_SIZE];
static CPU_STK stack_idle[IDLE_TASK_STACK_SIZE];
//----------------------------------------------------------------------------
//ITC
//----------------------------------------------------------------------------

static OS_TMR timer_slider;
static OS_TMR timer_recharge;
static OS_TMR timer_physics;
static OS_TMR timer_lcd;
static OS_TMR timer_led_periodic;
static OS_TMR timer_led_oneshot;

static OS_TMR timer_led_periodic_right;
static OS_TMR timer_led_oneshot_right;
static OS_FLAG_GRP event_button;
static OS_SEM shield_semaphore;
static OS_SEM slider_semaphore;
static OS_SEM physics_semaphore;
static OS_SEM lcd_semaphore;
static OS_MUTEX mutex_platform;
static OS_MUTEX mutex_physics;
static OS_MUTEX mutex_slider;

//----------------------------------------------------------------------------
//Variable
//----------------------------------------------------------------------------
enum{
	success = 1,
	fail= 0,
	activate =2,
	next=3,
};
int index ;
int flag;

static struct boost boost_btn;
static struct shield shield;
static struct masses ball[3];
static struct bounce wall;
static struct platform platform;
static struct laser weapon;

static GLIB_Context_t glibContext;
OS_TICK timer_str=0;
OS_TICK timer_end =0;
//Canyon Wall
#define wall_left 8
#define wall_right 120
#define tau 0.3
#define min_speed 10
#define time_update 10
//shield position

static bool pad0,pad1,pad2,pad3;
uint32_t button0_state = 1;
uint32_t button1_state = 1;
static unsigned int oneshot;
float oneshot_right;
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void slider_task(void *arg);
static void physics_task(void *arg);
static void shield_task(void *arg);
static void display_task(void *arg);
//static void led_task(void *arg);
static void idle_task(void *arg);
void read_capsense();
void callback_recharge (void* p_tmr, void* p_arg);
void callback_slider (void* p_tmr, void* p_arg);
void callback_physics (void* p_tmr, void* p_arg);
void callback_led_periodic (void* p_tmr, void* p_arg);
void callback_led_oneshot (void* p_tmr, void* p_arg);
static signed int final_velocity(struct masses* ball );
static signed int KE_Calc(struct masses* ball);
static signed int  y_vel_out(struct masses * ball, float percent);
static signed int x_vel_calc(struct masses* ball, float time);
static signed int y_vel_calc(struct masses* ball, float time);

static signed int x_pos_calc(struct masses* ball, float time);
static signed int y_pos_calc(struct masses* ball, float time);

void calc_a(int force, int mass, struct platform *pl);
static signed int x_vel_cal(int xvel, float time ,int acc );
static signed int x_pos_cal(int xpos, float time ,int vel );
float find_t(struct masses* ball, int y_final_pos);
float find_force_percent(float time,struct platform *platform);
void init(struct masses* ball, int* x_pos,int* y_pos,int*x_vel,int* y_vel,int* masses,int* diameter,int*acc_y,int*acc_x,int size);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/***************************************************************************//**/
void callback_recharge (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;
//    OSMutexPend (&mutex_platform,
//                  0,
//					OS_OPT_PEND_NON_BLOCKING,
//                   NULL,
//                   &err);
    shield.charge.ready = true;
    shield.charge.launch =true;
//    OSMutexPost (&mutex_platform,
//  		  	  OS_OPT_POST_NONE,
//                 &err);

    OSTmrSet (&timer_recharge,
    		time_update,//0.1s
                 0,
                 &callback_recharge,
                 NULL,
                 &err);
	return;
}
void callback_slider (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;
	OSSemPost (&slider_semaphore,
	             OS_OPT_POST_ALL,
	             &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void callback_physics (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;
	OSSemPost (&physics_semaphore,
	             OS_OPT_POST_ALL,
	             &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void callback_lcd (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;
	OSSemPost (&lcd_semaphore,
	             OS_OPT_POST_ALL,
	             &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void callback_led_periodic (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;
    if(oneshot == 0){
        GPIO_PinOutClear(LED1_port, LED1_pin);

    }
    else {
        GPIO_PinOutSet(LED1_port, LED1_pin);
    }

    if(oneshot != 0){
        OSTmrStart (&timer_led_oneshot,
                        &err);
    }
    //EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void callback_led_oneshot (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    GPIO_PinOutClear(LED1_port, LED1_pin);
    RTOS_ERR err;
    if(oneshot!= 0){
        OSTmrSet (&timer_led_oneshot,
                         oneshot,
                         0,
                         &callback_led_oneshot,
                         NULL,
                         &err);
    }
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}


void callback_led_periodic_right (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    RTOS_ERR err;

    GPIO_PinOutSet(LED0_port, LED0_pin);
    if(oneshot != 0){
        OSTmrStart (&timer_led_oneshot_right,
                        &err);
    }
    //EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void callback_led_oneshot_right (void* p_tmr, void* p_arg){
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(p_tmr);
    GPIO_PinOutClear(LED0_port, LED0_pin);
    RTOS_ERR err;
    if(oneshot_right!= 0){
        OSTmrSet (&timer_led_oneshot_right,
                         oneshot_right,
                         0,
                         &callback_led_oneshot,
                         NULL,
                         &err);
    }
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    return;
}
void data_str_init(void){
	flag = -1;
	index = 0;
//Wall Setup
	wall.enabled = true;
	wall.limited = true;
	wall.bounce_speed = 3;
//----------------------------------------------------------------------------
//Boost setup
	boost_btn.arming_window = 500;//1s
	boost_btn.arming_time = 0;
	boost_btn.recharge_time = 10;//disable arming for 0.5s
	boost_btn.ke_increase_percent = 1.4;
	boost_btn.ready = true;
	boost_btn.launch = false;
//Laser weapon
	weapon.num = 1;
//----------------------------------------------------------------------------
//Shield setup
	shield.charge = boost_btn;
	shield.weapon = weapon;
	shield.ke_reduce_percent = 0.7;
//----------------------------------------------------------------------------
//Platform setup
	platform.walls = wall;
	platform.length = 40;
	platform.auto_ctl = false;
	platform.mass = 2;
	platform.maxforce = 20;
	platform.x_init_pos_l = 20;
	platform.x_init_pos_r = 20+	platform.length;
	platform.x_init_vel = 6;
	oneshot = 1;
	oneshot_right = 1;
//----------------------------------------------------------------------------
//Mass setup

	int diameter[3]={10,12,16};
	int mass[] = {1,2,3};
//	ball[0].Diameter=10;
//	ball[0].mass = 1;
	int acc_y[]={9,12,12};
	int acc_x[]={0,0,0};
//	ball[0].acceleration_y=9;
//	ball[0].acceleration_x=0;
	int pos_x[]={20,40,80};
	int pos_y[]={20,20,20};
//	ball[0].x_init_pos = 20;
//	ball[0].y_init_pos = 20;
	int vel_y[]={0,0,0};
	int vel_x[]={4,8,11};
//	ball[0].x_init_vel = 5;
//	ball[0].y_init_vel = 5;
	init(ball, pos_x,pos_y,vel_x,vel_y,mass,diameter,acc_y,acc_x,3);

}
void blink_init(void)
{
  RTOS_ERR err;

  uint32_t status;
  // Create shield Task
  OSTaskCreate(&tcb_shield,"shield task",
               shield_task,
               DEF_NULL,
               SHIELD_TASK_PRIO,
               &stack_shield[0],
               (SHIELD_TASK_STACK_SIZE / 10u),
               SHIELD_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_slider,
               "slider task",
               slider_task,
               DEF_NULL,
               SLIDER_TASK_PRIO,
               &stack_slider[0],
               (SLIDER_TASK_STACK_SIZE / 10u),
               SLIDER_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_physics,
               "physics task",
               physics_task,
               DEF_NULL,
               PHYSICS_TASK_PRIO,
               &stack_physics[0],
               (PHYSICS_TASK_STACK_SIZE / 10u),
               PHYSICS_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

//  OSTaskCreate(&tcb_led,
//               "led task",
//               led_task,
//               DEF_NULL,
//               LED_TASK_PRIO,
//               &stack_led[0],
//               (LED_TASK_STACK_SIZE / 10u),
//               LED_TASK_STACK_SIZE,
//               0u,
//               0u,
//               DEF_NULL,
//               (OS_OPT_TASK_STK_CLR),
//               &err);
//  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  /* Enable the memory lcd */
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glibContext);

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNarrow6x8);

  // Create Blink Task
  OSTaskCreate(&tcb_display,
               "display task",
               display_task,
               DEF_NULL,
               DISPLAY_TASK_PRIO,
               &stack_display[0],
               (DISPLAY_TASK_STACK_SIZE / 10u),
               DISPLAY_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_idle,
                   "idle task",
                   idle_task,
                   DEF_NULL,
                   IDLE_TASK_PRIO,
                   &stack_idle[0],
                   (IDLE_TASK_STACK_SIZE / 10u),
                   IDLE_TASK_STACK_SIZE,
                   0u,
                   0u,
                   DEF_NULL,
                   (OS_OPT_TASK_STK_CLR),
                   &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}
void itc_init(){
  RTOS_ERR err;
  //Event flag
  OSFlagCreate (&event_button, "Flag", 0,& err);
//  OSFlagCreate (&result, "Result", 0,& err);
  //---------------------------------------------------------------------------
  //Timer
  //For slider
  OSTmrCreate (&timer_slider,"Timer_slider", 0, time_update,OS_OPT_TMR_PERIODIC, &callback_slider, NULL,  &err);
  OSSemCreate (&slider_semaphore,"slider_Semaphore", 0, &err);
  //For platform
  OSTmrCreate (&timer_recharge,"Timer_recharge",boost_btn.recharge_time , 0,OS_OPT_TMR_ONE_SHOT, &callback_recharge, NULL,&err);
  //For Physics
  OSTmrCreate (&timer_physics,"Timer_physics", 0, time_update,OS_OPT_TMR_PERIODIC, &callback_physics, NULL,  &err);
  OSSemCreate (&physics_semaphore,"physics_Semaphore", 0, &err);
  //For LCD
  OSTmrCreate (&timer_lcd,"Timer_lcd", 0, time_update,OS_OPT_TMR_PERIODIC, &callback_lcd, NULL,  &err);
  OSSemCreate (&lcd_semaphore,"LCD_Semaphore", 0, &err);
  //For LED
  OSTmrCreate (&timer_led_periodic,"Timer_led_periodic", 0, time_update,OS_OPT_TMR_PERIODIC, &callback_led_periodic, NULL,  &err);
  OSTmrCreate (&timer_led_oneshot,"Timer_led_oneshot", 1, 0,OS_OPT_TMR_ONE_SHOT, &callback_led_oneshot, NULL,  &err);

  OSTmrCreate (&timer_led_periodic_right,"Timer_led_periodic_right", 0, time_update,OS_OPT_TMR_PERIODIC, &callback_led_periodic_right, NULL,  &err);
  OSTmrCreate (&timer_led_oneshot_right,"Timer_led_oneshot_right", 1, 0,OS_OPT_TMR_ONE_SHOT, &callback_led_oneshot_right, NULL,  &err);
  //---------------------------------------------------------------------------
  //Mutex
  OSMutexCreate (&mutex_platform,"Mutex_platform",&err);
  OSMutexCreate (&mutex_physics,"Mutex_physics",&err);
  OSMutexCreate (&mutex_slider,"Mutex_slider",&err);
  //---------------------------------------------------------------------------
  //Semaphore
  OSSemCreate (&shield_semaphore,"shield_Semaphore", 0, &err);


}
void interrupt_init(void){

    GPIO_IntDisable(CLEAR);// Disable the GPIO Interrupt
    GPIO_IntClear (CLEAR);// Clear the GPIO Interrupt

    GPIO_ExtIntConfig(BUTTON0_port,BUTTON0_pin,BUTTON0_pin,ENABLE,ENABLE,ENABLE);//Enable the PushButton Interrupt
    GPIO_ExtIntConfig(BUTTON1_port,BUTTON1_pin,BUTTON1_pin,ENABLE,ENABLE,ENABLE);


    NVIC_EnableIRQ(GPIO_EVEN_IRQn); //Enable the PushButton Interrupt in NVIC
    NVIC_EnableIRQ(GPIO_ODD_IRQn);

}
void GPIO_EVEN_IRQHandler(void)
{
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  uint32_t int_flag;
  RTOS_ERR err;
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  button0_state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);
  //push(&button0,button0_state);//Btn event Fifo
  if (button0_state == PRESS){
	  timer_str = OSTimeGet(&err);
  }
  else if(button0_state == NPRESS){
	  timer_end = OSTimeGet(&err);
	  OSSemPost (&shield_semaphore,OS_OPT_POST_ALL, &err);
  }
}

void GPIO_ODD_IRQHandler(void)
{
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  uint32_t int_flag;
  //RTOS_ERR err;
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  button1_state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);
  if ((button1_state == PRESS) &&shield.weapon.num>0 ){
	  flag = activate;
	  shield.weapon.num--;
  }


}
static void shield_task(void *arg)
{
    RTOS_ERR err;
    PP_UNUSED_PARAM(arg);
    while (1)
    {
          OSSemPend (&shield_semaphore, 0,OS_OPT_PEND_BLOCKING,NULL,&err);
          EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
          OSMutexPend (&mutex_platform,0,OS_OPT_PEND_BLOCKING,NULL, &err);
          EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

          shield.charge.arming_time =timer_end-timer_str;

          OSMutexPost (&mutex_platform,OS_OPT_POST_NONE,&err);
          EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
          //Re-charge the weapon
          //If arming time greater than arming_window, disable the arming
          if( shield.charge.arming_time >shield.charge.arming_window ){
              OSMutexPend (&mutex_platform, 0,OS_OPT_PEND_NON_BLOCKING,NULL, &err);
              EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

              //shield.charge.ready =false;
              shield.charge.launch = false;

              OSMutexPost (&mutex_platform,OS_OPT_POST_NONE, &err);
              EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
              OSTmrStart (&timer_recharge, &err);
              EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
          }
          else if (shield.charge.arming_time <= shield.charge.arming_window){
              OSMutexPend (&mutex_platform, 0,OS_OPT_PEND_NON_BLOCKING,NULL, &err);
              EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

              //shield.charge.ready =true;
              shield.charge.launch = true;

              OSMutexPost (&mutex_platform,OS_OPT_POST_NONE, &err);
          }
    }
}

static void slider_task(void *arg)
{
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
    //OS_FLAGS    flags;
    OSTmrStart (&timer_slider,
                &err);
    OSTmrStart (&timer_led_periodic,
                    &err);
    OSTmrStart (&timer_led_periodic_right,
                    &err);
    //signed int dist=0;
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    while (1)
    {
    	//OSTimeDly(100, OS_OPT_TIME_DLY, &err);//Same frequency as LCD

        OSSemPend (&slider_semaphore, 0,OS_OPT_PEND_BLOCKING,NULL,&err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        read_capsense();
        OSMutexPend (&mutex_slider,0,OS_OPT_PEND_BLOCKING,NULL, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    	if (pad0 == true){
    		calc_a(-platform.maxforce,platform.mass, &platform);
    		oneshot = (abs(platform.acceleration))/(platform.maxforce/platform.mass)*time_update;
    		if ((platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)) < 8){
    			//dist = 8- platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)*0.5;
    			platform.x_init_pos_l =8;
    			platform.x_init_pos_r =platform.x_init_pos_l+platform.length;
    			platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else{
        		platform.x_init_vel =  x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
        		platform.x_init_pos_l = x_pos_cal(platform.x_init_pos_l,0.5,platform.x_init_vel);
        		platform.x_init_pos_r = platform.x_init_pos_l + platform.length;
    		}

    	}
    	else if (pad1 == true){
    		calc_a(-0.5*platform.maxforce,platform.mass, &platform);
    		oneshot = (abs(platform.acceleration))/(platform.maxforce/platform.mass)*time_update;
    		if ((platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)) < 8){
    			//dist = 8- platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)*0.5;
    			platform.x_init_pos_l =8;
    			platform.x_init_pos_r =platform.x_init_pos_l+platform.length;
    			platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else{
        		platform.x_init_vel =  x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
        		platform.x_init_pos_l = x_pos_cal(platform.x_init_pos_l,0.5,platform.x_init_vel);
        		platform.x_init_pos_r = platform.x_init_pos_l + platform.length;
    		}
    	}
    	else if (pad2 == true){
    		calc_a(0.5*platform.maxforce,platform.mass, &platform);
    		oneshot = (abs(platform.acceleration))/(platform.maxforce/platform.mass)*time_update;
    		if((platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)) > 120){
    			//dist = platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)*0.5-120;
    			platform.x_init_pos_r =120;
    			platform.x_init_pos_l =platform.x_init_pos_r-platform.length;
        		platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else{
        		platform.x_init_vel =  x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
        		platform.x_init_pos_l = x_pos_cal(platform.x_init_pos_l,0.5,platform.x_init_vel);
        		platform.x_init_pos_r = platform.x_init_pos_l + platform.length;
    		}
    	}
        else if (pad3 == true){
    		calc_a(platform.maxforce,platform.mass, &platform);
    		oneshot = (abs(platform.acceleration))/(platform.maxforce/platform.mass)*time_update;
    		if((platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)) > 120){
    			//dist = platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)*0.5-120;
    			platform.x_init_pos_r =120;
    			platform.x_init_pos_l =platform.x_init_pos_r-platform.length;
        		platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else{
        		platform.x_init_vel =  x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
        		platform.x_init_pos_l = x_pos_cal(platform.x_init_pos_l,0.5,platform.x_init_vel);
        		platform.x_init_pos_r = platform.x_init_pos_l + platform.length;
    		}
    	}
    	else{
    		platform.acceleration = 0;
    		oneshot = (abs(platform.acceleration))/(platform.maxforce/platform.mass)*time_update;
    		if((platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)) > 120){
    			//dist = platform.x_init_pos_r + (platform.x_init_vel+platform.acceleration*0.5)*0.5-120;
    			platform.x_init_pos_r =120;
    			platform.x_init_pos_l =platform.x_init_pos_r-platform.length;
        		platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else if ((platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)) < 8){
    			//dist = 8- platform.x_init_pos_l + (platform.x_init_vel+platform.acceleration*0.5)*0.5;
    			platform.x_init_pos_l =8;
    			platform.x_init_pos_r =platform.x_init_pos_l+platform.length;
    			platform.x_init_vel =  -0.5*x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
    		}
    		else{
        		platform.x_init_vel =  x_vel_cal(platform.x_init_vel,0.5,platform.acceleration);
        		platform.x_init_pos_l = x_pos_cal(platform.x_init_pos_l,0.5,platform.x_init_vel);
        		platform.x_init_pos_r = platform.x_init_pos_l + platform.length;
    		}
    	}
        OSMutexPost (&mutex_slider,OS_OPT_POST_NONE,&err);
       EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    }
}
/***************************************************************************//**
 * @brief
 *  Physics TASK
 ******************************************************************************/
void physics_task(void *arg){
    RTOS_ERR err;
    PP_UNUSED_PARAM(arg);
    OSTmrStart (&timer_physics,
                &err);
    while(1){
        OSSemPend (&physics_semaphore,0,OS_OPT_PEND_BLOCKING,NULL,&err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        OSMutexPend (&mutex_physics,0,OS_OPT_PEND_BLOCKING,NULL, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        if (ball[index].y_init_pos  < 0 ||index > 2 ){//0.5*ball[0].Diameter
        	if(index > 2){
        		flag = success;
        	}
        	else{
        		index++;
        	}
        }

        else if (ball[index].y_init_pos  > 128 ){
        	//Didn't catch the ball
        	flag = fail;
			oneshot_right =2;
        }

        else if(ball[index].y_init_pos<= 110 && ball[index].y_init_pos + ball[index].y_init_vel*tau >= 110-0.5*ball[index].Diameter && ball[index].x_init_pos>=platform.x_init_pos_l + platform.x_init_vel*tau && ball[index].x_init_pos<=platform.x_init_pos_r+ platform.x_init_vel*tau){
        	//Catch the ball
        	if(ball[index].y_init_vel  < 12 )//10 is threshold velocity
        	        {
        				ball[index].x_init_pos =  x_pos_calc(&ball[index],tau);
            			ball[index].y_init_pos = y_pos_calc(&ball[index],tau);//For ball
            			oneshot_right =2;
        	        	//flag = fail;
        	        }
        	else if(shield.charge.launch == false){//Without boost
        		shield.charge.ready =true;
            	ball[index].y_init_pos =  110-0.5*ball[index].Diameter;
            	//ball[index].final_velocity = final_velocity(&ball[index] );
            	ball[index].x_init_vel = x_vel_calc(&ball[index],tau);
            	//ball[index].ke = KE_Calc(&ball[index]);
            	//ball[index].y_init_vel = -(y_vel_out(&ball[index], shield.ke_reduce_percent));
            	ball[index].y_init_vel = -(y_vel_calc(&ball[index],tau)*shield.ke_reduce_percent);
        	}
        	else if (shield.charge.launch == true){//With boost
        		shield.charge.launch = false;
        		shield.charge.ready = false;
            	ball[index].y_init_pos =  110-0.5*ball[index].Diameter;
            	//ball[index].final_velocity = final_velocity(&ball[index] );
            	ball[index].x_init_vel = x_vel_calc(&ball[index],tau);
            	//ball[index].ke = KE_Calc(&ball[index]);
            	//ball[index].y_init_vel = -(y_vel_out(&ball[index], shield.charge.ke_increase_percent));
            	ball[index].y_init_vel = -(y_vel_calc(&ball[index],tau)*shield.charge.ke_increase_percent);
        	}
        }
        else if( ball[index].x_init_pos + ball[index].x_init_vel*tau > 120-0.5*ball[index].Diameter){
        	ball[index].x_init_pos = 120-0.5*ball[index].Diameter;
        	ball[index].x_init_vel = -x_vel_calc(&ball[index],tau);
        }
        else if ( ball[index].x_init_pos + ball[index].x_init_vel*tau < 8+0.5*ball[index].Diameter){
        	ball[index].x_init_pos = 8+0.5*ball[index].Diameter;
        	ball[index].x_init_vel = -x_vel_calc(&ball[index],tau);
        }
        else{
        	ball[index].x_init_pos =  x_pos_calc(&ball[index],tau);//For ball
           	ball[index].y_init_pos = y_pos_calc(&ball[index],tau);//For ball
        	ball[index].x_init_vel = x_vel_calc(&ball[index],tau);//For ball
            ball[index].y_init_vel = y_vel_calc(&ball[index],tau);
        }

        OSMutexPost (&mutex_physics,OS_OPT_POST_NONE,&err);
        //float t = find_t(&ball[index], 110);
        //oneshot_right = find_force_percent(t,&platform);
        oneshot_right = 2;
       EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    }
}


/***************************************************************************//**
 * @brief
 *   Display TASK
 ******************************************************************************/
static void display_task(void *arg)
{
    RTOS_ERR err;
    PP_UNUSED_PARAM(arg);
    OSTmrStart (&timer_lcd, &err);
    char ball_speed[100];
    char pf_speed[100];
    char weapon[100];
    while (1)
    {
        OSSemPend (&lcd_semaphore,0,OS_OPT_PEND_BLOCKING,NULL,&err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        GLIB_clear(&glibContext);
    	GLIB_drawLineV	(&glibContext,8,5,120);	//Canyon Wall range 20-110
    	GLIB_drawLineV	(&glibContext,120,5,120);
    	GLIB_drawLineH 	(&glibContext,platform.x_init_pos_l,110,platform.x_init_pos_r);
        GLIB_drawCircle	(&glibContext,ball[index].x_init_pos ,ball[index].y_init_pos , 0.5*(ball[index].Diameter) );
        sprintf(ball_speed, "V_ball:%d", ball[index].y_init_vel);
        GLIB_drawStringOnLine(&glibContext,
                              ball_speed,
                               11,
                               GLIB_ALIGN_LEFT,
                               10,
                               10,
                               true);
        sprintf(pf_speed, "V_pf:%d", platform.x_init_vel);
    	GLIB_drawString	(&glibContext,pf_speed,
    	sizeof(pf_speed),
    	70,
    	120,
    	0
    	);
        sprintf(weapon, "Fire: %d Ready: %d", shield.charge.launch,shield.charge.ready);
    	GLIB_drawString	(&glibContext,weapon,
    	sizeof(weapon),
    	10,
    	10,
    	0
    	);
        sprintf(weapon, "#Laser: %d", shield.weapon.num);
    	GLIB_drawString	(&glibContext,weapon,
    	sizeof(weapon),
    	10,
    	20,
    	0
    	);
        if(flag == success){
            GLIB_clear(&glibContext);
        	GLIB_drawString	(&glibContext,"You Did it !!!",
        	14,
        	30,
        	30,
        	0
        	);
        	DMD_updateDisplay();
        	while(1);
        }
        else if (flag == fail){
            GLIB_clear(&glibContext);
        	GLIB_drawString	(&glibContext,"Ohhhh Noooo!!!",
        	14,
        	30,
        	30,
        	0
        	);
        	DMD_updateDisplay();
        	while(1);
        }
        else if (flag == activate){
        	flag =-1;
        	GLIB_drawString	(&glibContext,"WEAPON ACTIVEATED",
        	17,
        	10,
        	10,
        	0
        	);
        	DMD_updateDisplay();
        	index++;
        	if(index >2){
        		flag = success;
        	}
        }
        else{
        	DMD_updateDisplay();
        }

    }
}
/***************************************************************************//**
 * @brief
 *   IDLE TASk
 ******************************************************************************/
static void idle_task(void *arg)
{
    PP_UNUSED_PARAM(arg);
    while (1)
    {
        EMU_EnterEM1();

    }
}
void read_capsense(){
  CAPSENSE_Sense();

  pad0 = CAPSENSE_getPressed(PAD0);
  pad1 = CAPSENSE_getPressed(PAD1);
  pad2 = CAPSENSE_getPressed(PAD2);
  pad3 = CAPSENSE_getPressed(PAD3);
  return;
}
signed int final_velocity(struct masses * ball ){
	return sqrt(ball->x_init_vel*ball->x_init_vel+ ball->y_init_vel*ball->y_init_vel);
}

signed int  KE_Calc(struct masses * ball){
   //return  0.5*ball->mass*ball->final_velocity*ball->final_velocity;
	return  ball->final_velocity*ball->final_velocity;
}
signed int  y_vel_out(struct masses * ball, float percent){
   int temp;
   temp = ball->ke*percent;
   int vel;
   //vel = (2*temp)/ball->mass;
   vel = temp;
   return sqrt(vel-vel-ball->x_init_vel*ball->x_init_vel);
}
static int x_vel_calc(struct masses* ball, float time){

   return  ball->x_init_vel + ball->acceleration_x*time;


}
static int y_vel_calc(struct masses* ball, float time){

   return ball->y_init_vel + ball->acceleration_y*time;

}

static int x_pos_calc(struct masses* ball, float time){

   return ball->x_init_pos = ball->x_init_pos + ball->x_init_vel*time;

}
static int y_pos_calc(struct masses* ball, float time ){
   return ball->y_init_pos +  + ball->y_init_vel*time;

}
void calc_a(int force, int mass, struct platform *pl){
	pl->acceleration = force/mass;
}
signed int x_vel_cal(int xvel, float time ,int acc ){
	return xvel+acc*time;
}
signed int x_pos_cal(int xpos, float time ,int vel ){
	return xpos+vel*time;
}
float find_t(struct masses* ball, int y_final_pos)
{
	float temp1,temp2;
	temp1 = (ball->y_init_vel+sqrt(ball->y_init_vel*ball->y_init_vel-4*(-ball->acceleration_y)*(y_final_pos-ball->y_init_pos)))/(-2*ball->acceleration_y);
	temp2 = (ball->y_init_vel-sqrt(ball->y_init_vel*ball->y_init_vel-4*(-ball->acceleration_y)*(y_final_pos-ball->y_init_pos)))/(-2*ball->acceleration_y);
	if(temp1>0){
		return temp1;
	}
	else if (temp2>0){
		return temp2;
	}
	else{
		return 0;
	}
}
float find_force_percent(float time,struct platform *platform){
	int force;
	int xf = x_pos_cal(platform->x_init_pos_l+platform->length*0.5, time ,platform->x_init_vel );
	force = platform->mass * ((xf-platform->x_init_pos_l+platform->length*0.5-platform->x_init_vel*time)/(time*time));
	return (force/platform->maxforce)*4;
}
void init(struct masses* ball, int* x_pos,int* y_pos,int*x_vel,int* y_vel,int* masses,int *diameter,int *acc_y,int* acc_x,int size){
	for(int i =0;i<size;i++){
		ball[i].Diameter =diameter[i];
		ball[i].acceleration_x = acc_x[i];
		ball[i].acceleration_y = acc_y[i];
		ball[i].mass = masses[i];
		ball[i].x_init_pos = x_pos[i];
		ball[i].x_init_vel = x_vel[i];
		ball[i].y_init_pos = y_pos[i];
		ball[i].y_init_vel = y_vel[i];
	}
}
