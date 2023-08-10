/*
 * task.c
 *
 *  Created on: 2022年3月1日
 *      Author: Chengming Li
 */
#include "task.h"
#include "os.h"
#include "em_emu.h"
#include "fifo.h"
#include "glib.h"
#include "dmd.h"
#include "capsense.h"
#include "cmu.h"
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb_speed;
static OS_TCB tcb_direction;
static OS_TCB tcb_monitor;
static OS_TCB tcb_led;
static OS_TCB tcb_display;
static OS_TCB tcb_idle;

static CPU_STK stack_speed[SPEED_TASK_STACK_SIZE];
static CPU_STK stack_direction[DIRECTION_TASK_STACK_SIZE];
static CPU_STK stack_monitor[MONITOR_TASK_STACK_SIZE];
static CPU_STK stack_led[LED_TASK_STACK_SIZE];
static CPU_STK stack_display[DISPLAY_TASK_STACK_SIZE];
static CPU_STK stack_idle[IDLE_TASK_STACK_SIZE];


static OS_FLAG_GRP event_flag_speed_dir;
static OS_FLAG_GRP event_flag_led;
;
static OS_SEM semaphore;

static OS_MUTEX mutex_speed;
static OS_MUTEX mutex_direction;

static struct speed_t speed;
static struct direction_t dir;
static struct node_t* button0;
static struct node_t* button1;

uint32_t button0_state = 1;
uint32_t button1_state = 1;
uint32_t slider_dir;
static volatile uint32_t msTicks = 0;
static int currentLine = 0;
static GLIB_Context_t glibContext;

static bool pad0,pad1,pad2,pad3;
static int pad_state_new = -1;

enum event{

   spped_update = 1,
   direction_update = 2,
   alert_speed = 8,
   alert_direction = 16,
   no_op = 32,
   clear_dir = 64,
   clear_speed = 128
};
enum direction{
	no_dir = 4,
	hard_left = 0,
	hard_right = 1,
	gradual_left = 2,
	gradual_right = 3
};
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void speed_task(void *arg);
static void direction_task(void *arg);
static void monitor_task(void *arg);
static void led_task(void *arg);
static void display_task(void *arg);
static void idle_task(void *arg);
void read_capsense(void);
void SysTick_Handler(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void interrupt_init(void){
    uint32_t stat;
    uint32_t ticks;
    GPIO_IntDisable(CLEAR);// Disable the GPIO Interrupt
    GPIO_IntClear (CLEAR);// Clear the GPIO Interrupt

    GPIO_ExtIntConfig(BUTTON0_port,BUTTON0_pin,BUTTON0_pin,ENABLE,ENABLE,ENABLE);//Enable the PushButton Interrupt
    GPIO_ExtIntConfig(BUTTON1_port,BUTTON1_pin,BUTTON1_pin,ENABLE,ENABLE,ENABLE);

    ticks = CMU_ClockFreqGet(cmuClock_CORE) / 1000;  //Enable the timer to start the timer interrupt
    stat = SysTick_Config(ticks);
    EFM_ASSERT(!stat);

    NVIC_EnableIRQ(GPIO_EVEN_IRQn); //Enable the PushButton Interrupt in NVIC
    NVIC_EnableIRQ(GPIO_ODD_IRQn);


}
/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing of button0
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  uint32_t int_flag;
  RTOS_ERR err;
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  button0_state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);

  push(&button0,button0_state);//Btn event Fifo
  //Post Semaphore
  OSSemPost (&semaphore,
		  	 OS_OPT_POST_ALL,
             &err);
}

/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing of button1
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  uint32_t int_flag;
  RTOS_ERR err;
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  button1_state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);
  push(&button1,button1_state);//Btn1 event Fifo
  OSSemPost (&semaphore,
		  	 OS_OPT_POST_ALL,
             &err);
}
/***************************************************************************//**
 * @brief
 *   task init
 ******************************************************************************/
void blink_init(void)
{
  RTOS_ERR err;

  // Create Blink Task
  OSTaskCreate(&tcb_speed,
               "speed task",
               speed_task,
               DEF_NULL,
               SPEED_TASK_PRIO,
               &stack_speed[0],
               (SPEED_TASK_STACK_SIZE / 10u),
               SPEED_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  OSTaskCreate(&tcb_direction,
               "direction task",
               direction_task,
               DEF_NULL,
               DIRECTION_TASK_PRIO,
               &stack_direction[0],
               (DIRECTION_TASK_STACK_SIZE / 10u),
               DIRECTION_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  OSTaskCreate(&tcb_monitor,
                 "monitor task",
                 monitor_task,
                 DEF_NULL,
                 MONITOR_TASK_PRIO,
                 &stack_monitor[0],
                 (MONITOR_TASK_STACK_SIZE / 10u),
                 MONITOR_TASK_STACK_SIZE,
                 0u,
                 0u,
                 DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_led,
                 "led task",
                 led_task,
                 DEF_NULL,
                 LED_TASK_PRIO,
                 &stack_led[0],
                 (LED_TASK_STACK_SIZE / 10u),
                 LED_TASK_STACK_SIZE,
                 0u,
                 0u,
                 DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));


  uint32_t status;

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
/***************************************************************************//**
 * @brief
 *   itc init
 ******************************************************************************/
void itc_init(){
  RTOS_ERR err;


  OSFlagCreate (&event_flag_speed_dir,
                     "Flag_speed_dir",
                          0,
                     & err);
  OSFlagCreate (&event_flag_led,
                     "Flag_led",
                          0,
                     & err);

  OSSemCreate (&semaphore,
              "Semaphore",
                         0,
                     &err);
  OSMutexCreate (&mutex_speed,
                 "Mutex_speed",
                 &err);
  OSMutexCreate (&mutex_direction,
                 "Mutex_direction",
                 &err);

}
/***************************************************************************//**
 * @brief
 *   data structure init
 ******************************************************************************/
void data_str_init(void){
	button0 = NULL;
	button1 = NULL;

	speed.cnt_dec = 0;
	speed.cnt_inc = 0;
	speed.curr_speed = 0;

	dir.curr_dir = 0;
	dir.left_cnt = 0;
	dir.right_cnt = 0;
	dir.time_in_total = 0;
	dir.pad_state = 4;
}

static void speed_task(void *arg){
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
      while (1)
      {
          OSSemPend (&semaphore,
                      0,
                      OS_OPT_PEND_BLOCKING,
                      NULL,
                      &err);
          EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
          struct node_t* temp0 = peek(button0);
          pop(&button0);
          struct node_t* temp1 = peek(button1);
          pop(&button1);

          switch(temp0->btn)
        	  {
          	  	  case 0:
          	          OSMutexPend (&mutex_speed,
          	                        0,
									OS_OPT_PEND_NON_BLOCKING,
          	                         NULL,
          	                         &err);
          	  			  speed.curr_speed += 5;
          	          OSMutexPost (&mutex_speed,
          	        		  	  OS_OPT_POST_NONE,
          	                       &err);
          	  		  break;
          	  	  case 1:
          	  		  break;
          	  	  default:
          	  		  break;


        	  }
          switch(temp1->btn)
        	  {
          	  	  case 0:
          	          OSMutexPend (&mutex_speed,
          	                        0,
									OS_OPT_PEND_NON_BLOCKING,
          	                         NULL,
          	                         &err);
          	  		  if(speed.curr_speed > 0){
          	  			  speed.curr_speed -= 5;
          	  		  }
          	          OSMutexPost (&mutex_speed,
          	        		  	  OS_OPT_POST_NONE,
          	                       &err);
          	  		  break;
          	  	  case 1:
          	  		  break;
          	  	  default:
          	  		  break;
        	  }
          OSFlagPost (&event_flag_speed_dir,
        		  	  spped_update 	  	  ,
					  OS_OPT_POST_FLAG_SET,
                      &err);

      }
}
static void direction_task(void *arg){
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
    while (1)
     {

        OSTimeDly(10, OS_OPT_TIME_DLY, &err);

        read_capsense();//Mutex is inside the read_capsense() function


        OSFlagPost (&event_flag_speed_dir,
        		        direction_update,
					  OS_OPT_POST_FLAG_SET,
                    &err);

         EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

     }
}
static void monitor_task(void *arg){
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
    int speed_read = 0;
    int direction_read = 0;
    int time_read= 0;
    while(1)
    {
        //OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    	OSFlagPend (&event_flag_speed_dir,
    			    spped_update|direction_update,
    	                 0,
						 OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
						 NULL,
    	             &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

        //Read the speed
        OSMutexPend (&mutex_speed,
                        0,
						OS_OPT_PEND_NON_BLOCKING,
                         NULL,
                         &err);
        speed_read = speed.curr_speed;
        OSMutexPost (&mutex_speed,
        		  	  OS_OPT_POST_NONE,
                       &err);

        //Read the direction and time in total
        OSMutexPend (&mutex_direction,
                        0,
						OS_OPT_PEND_NON_BLOCKING,
                         NULL,
                         &err);
        direction_read = dir.pad_state;
        time_read = dir.time_in_total;
        OSMutexPost (&mutex_direction,
        		  	  OS_OPT_POST_NONE,
                       &err);

        //Post Time violation
        if(speed_read >45 && time_read > TIME_LIM){
                OSFlagPost (&event_flag_led,
                		      alert_speed,
        					  OS_OPT_POST_FLAG_SET,
                            &err);
                OSFlagPost (&event_flag_led,
                		      alert_direction ,
        					  OS_OPT_POST_FLAG_SET,
                            &err);
        }
        else if(speed_read >45 && direction_read != no_dir){
            OSFlagPost (&event_flag_led,
            		      alert_speed ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }
        else if(speed_read>75 && time_read < TIME_LIM){
            OSFlagPost (&event_flag_led,
            		      clear_dir ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }
        else if(speed_read <= 45 && time_read >= TIME_LIM){
            OSFlagPost (&event_flag_led,
            		      clear_speed ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }
        else if(speed_read>75){
            OSFlagPost (&event_flag_led,
            		      alert_speed ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }
        else if (time_read >= TIME_LIM){
            OSFlagPost (&event_flag_led,
            		      alert_direction ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }

        else{
            OSFlagPost (&event_flag_led,
            		      no_op ,
    					  OS_OPT_POST_FLAG_SET,
                        &err);
        }


    }
}
static void led_task(void *arg){
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
    OS_FLAGS flag_read;
    while(1){
    	flag_read = OSFlagPend (&event_flag_led,
    							alert_speed|alert_direction|no_op|clear_speed|clear_dir,
								0,
								OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
								NULL,
								&err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    	switch(flag_read)
    	{
    		case alert_speed & alert_direction:
    			GPIO_PinOutSet(LED0_port, LED0_pin);
    			GPIO_PinOutSet(LED1_port, LED1_pin);
    			break;
    		case alert_speed:
    			GPIO_PinOutSet(LED0_port, LED0_pin);
    			break;
    		case alert_direction:
    			GPIO_PinOutSet(LED1_port, LED1_pin);
    			break;
    		case clear_speed:
    		    GPIO_PinOutClear(LED0_port, LED0_pin);
    			GPIO_PinOutSet(LED1_port, LED1_pin);
    			break;
    		case clear_dir:
    		    GPIO_PinOutClear(LED1_port, LED1_pin);
    			GPIO_PinOutSet(LED0_port, LED0_pin);
    			break;
    		case no_op:
    		    GPIO_PinOutClear(LED0_port, LED0_pin);
    		    GPIO_PinOutClear(LED1_port, LED1_pin);
    			break;
    		default:
    		    GPIO_PinOutClear(LED0_port, LED0_pin);
    		    GPIO_PinOutClear(LED1_port, LED1_pin);
    	}
    }
}
static void display_task(void *arg){
    PP_UNUSED_PARAM(arg);
    RTOS_ERR err;
    int speed_read = 0;
    int direction_read = 4;

    char str_speed[100];
    char str_direction[100];
    while (1)
    {
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
        //Read the speed
                OSMutexPend (&mutex_speed,
                                0,
        						OS_OPT_PEND_NON_BLOCKING,
                                 NULL,
                                 &err);
                speed_read = speed.curr_speed;
                OSMutexPost (&mutex_speed,
                		  	  OS_OPT_POST_NONE,
                               &err);

                //Read the direction and time in total
                OSMutexPend (&mutex_direction,
                                0,
        						OS_OPT_PEND_NON_BLOCKING,
                                 NULL,
                                 &err);
                direction_read = dir.pad_state;

                OSMutexPost (&mutex_direction,
                		  	  OS_OPT_POST_NONE,
                               &err);
        GLIB_clear(&glibContext);

        sprintf(str_speed, "Current Speed is:%d", speed_read);
        switch(direction_read){
        	case hard_left:
        		sprintf(str_direction, "dir is hard_left");
        		break;
        	case hard_right:
        		sprintf(str_direction, "dir is hard_right");
        		break;
        	case gradual_left:
        		sprintf(str_direction, "dir is gradual_left");
        		break;
        	case gradual_right:
        		sprintf(str_direction, "dir is gradual_right");
        		break;
        	case no_dir:
        		sprintf(str_direction, "dir is Forward");
        		break;
        	default:
        		break;

        }

        GLIB_drawStringOnLine(&glibContext,
                              str_speed,
                               currentLine,
                               GLIB_ALIGN_LEFT,
                               5,
                               5,
                               true);
        GLIB_drawStringOnLine(&glibContext,
                              str_direction,
                               1,
                               GLIB_ALIGN_LEFT,
                               5,
                               5,
                               true);
        DMD_updateDisplay();
    }
}
static void idle_task(void *arg)
{
    PP_UNUSED_PARAM(arg);
    while (1)
    {
        EMU_EnterEM1();

    }
}
/***************************************************************************//**
 * @brief
 *   Read the position of a finger on the capacitive touch slider
 ******************************************************************************/
void read_capsense(){

  CAPSENSE_Sense();

  RTOS_ERR err;
  pad0 = CAPSENSE_getPressed(PAD0);
  pad1 = CAPSENSE_getPressed(PAD1);
  pad2 = CAPSENSE_getPressed(PAD2);
  pad3 = CAPSENSE_getPressed(PAD3);
  //Determine the position of a finger on the capacitive touch slider
//	hard_left = 1,
//	hard_right = 2,
//	gradual_left = 3,
//	gradual_right = 4

  if(pad0 == true){
	  dir.left_cnt++;
	  pad_state_new = hard_left;
  }
  else if(pad1 == true){
	  dir.left_cnt++;
	  pad_state_new = gradual_left;
  }
  else if(pad2 == true){
		dir.right_cnt++;
	  pad_state_new = gradual_right;
  }
  else if(pad3 == true){
		dir.right_cnt++;
	  pad_state_new = hard_right;
  }
  else{
	  pad_state_new = 4;
  }
  //When both slider are pressed, no led is lit
  OSMutexPend (&mutex_direction,
                  0,
					OS_OPT_PEND_NON_BLOCKING,
                   NULL,
                   &err);
if(pad0 == true){

	if(pad_state_new == 4){
		dir.pad_state = hard_left;
	}
	if(pad_state_new == dir.pad_state){

		dir.time_in_total = msTicks;
		dir.pad_state = hard_left;
	}
	else{
		msTicks = 0;
		dir.time_in_total = 0;
		dir.pad_state = pad_state_new;
	}


}
else if(pad1 == true){

	if(pad_state_new == 4){

		dir.pad_state = gradual_left;
	}
	if(pad_state_new == dir.pad_state){

		dir.time_in_total = msTicks;
		dir.pad_state = gradual_left;
	}
	else{
		msTicks = 0;
		dir.time_in_total = 0;
		dir.pad_state = pad_state_new;
	}
}
else if(pad2 == true){

	if(pad_state_new == 4){

		dir.pad_state = gradual_right;
	}
	if(pad_state_new == dir.pad_state){

		dir.time_in_total = msTicks;
		dir.pad_state = gradual_right;
	}
	else{
		msTicks = 0;
		dir.time_in_total = 0;
		dir.pad_state = pad_state_new;
	}
}
else if(pad3 == true){

	if(pad_state_new == 4){
		dir.pad_state = hard_right;
	}
	if(pad_state_new == dir.pad_state){

		dir.time_in_total = msTicks;
		dir.pad_state = hard_right;
	}
	else{
		msTicks = 0;
		dir.time_in_total = 0;
		dir.pad_state = pad_state_new;
	}
}
else{
	dir.time_in_total = 0;
	msTicks = 0;
	dir.pad_state = 4;
}

  OSMutexPost (&mutex_direction,
  		  	  OS_OPT_POST_NONE,
                 &err);



}

/***************************************************************************//**
 * @brief
 *   Drives each LED based on the value of BTN0, BTN1, and the slider direction
 ******************************************************************************/
void write_led(void){
  //LED0 - While BTN0 is pressed or while the slider position is left of center, turn off otherwise
  if((button0_state == PRESS && button1_state == PRESS) || (slider_dir == LEFT &&slider_dir == RIGHT)){
      GPIO_PinOutClear(LED0_port, LED0_pin);
      GPIO_PinOutClear(LED1_port, LED1_pin);
  }
  else{
      if((button0_state == PRESS) || slider_dir == LEFT){
          GPIO_PinOutSet(LED0_port, LED0_pin);
      }
      else{
          GPIO_PinOutClear(LED0_port, LED0_pin);
      }
      //LED1 - While BTN1 is pressed or while the slider position is right of center, turn off otherwise
      if ((button1_state == PRESS) || slider_dir == RIGHT ){
          GPIO_PinOutSet(LED1_port, LED1_pin);
      }
      else{
          GPIO_PinOutClear(LED1_port, LED1_pin);
      }
  }


}
void SysTick_Handler(void)
{

	if(pad0 == true){

		if(pad_state_new == dir.pad_state){
			msTicks ++;

		}
		else{
			msTicks = 0;
			msTicks ++;
		}


	}
	else if(pad1 == true){

		if(pad_state_new == dir.pad_state){
			msTicks ++;

		}
		else{
			msTicks = 0;
			msTicks ++;
		}
	}
	else if(pad2 == true){

		if(pad_state_new == dir.pad_state){
			msTicks ++;
		}
		else{
			msTicks = 0;
			msTicks ++;
		}
	}
	else if(pad3 == true){

		if(pad_state_new == dir.pad_state){
			msTicks ++;

		}
		else{
			msTicks = 0;
			msTicks ++;
		}
	}
	else{
		msTicks = 0;
	}


}
