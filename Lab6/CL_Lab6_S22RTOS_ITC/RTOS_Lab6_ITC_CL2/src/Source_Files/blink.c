/***************************************************************************//**
 * @file
 * @brief Blink examples functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#include "os.h"
#include "blink.h"
#include "em_emu.h"
#include "gpio.h"
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb_blink;
static OS_TCB tcb_button;
static OS_TCB tcb_slider;
static OS_TCB tcb_idle;
static CPU_STK stack_blink[BLINK_TASK_STACK_SIZE];
static CPU_STK stack_button[BUTTON_TASK_STACK_SIZE];
static CPU_STK stack_slider[SLIDER_TASK_STACK_SIZE];
static CPU_STK stack_idle[IDLE_TASK_STACK_SIZE];

static OS_Q message_queue;
static OS_FLAG_GRP event_flag;
static OS_SEM semaphore;
static OS_TMR timer;

uint32_t button0_state = 1;
uint32_t button1_state = 1;
uint32_t slider_dir;

enum event{
   no_op = 0,
   button0 = 1,
   button1 = 2
};
enum message{
  button = 1,
  slider = 2
};
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void blink_task(void *arg);
static void slider_task(void *arg);
static void button_task(void *arg);
static void idle_task(void *arg);
void callback_2 (void* p_tmr, void* p_arg);
void read_capsense();
void write_led();
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Timer call back function.
 ******************************************************************************/
void callback_2 (void* p_tmr, void* p_arg){
  PP_UNUSED_PARAM(p_arg);
  PP_UNUSED_PARAM(p_tmr);
  RTOS_ERR err;
  OSSemPost (&semaphore,
             OS_OPT_POST_1,
             &err);
}

/***************************************************************************//**
 * Initialize blink example.
 ******************************************************************************/
void blink_init(void)
{
  RTOS_ERR err;

  // Create Blink Task
  OSTaskCreate(&tcb_blink,
               "blink task",
               blink_task,
               DEF_NULL,
               BLINK_TASK_PRIO,
               &stack_blink[0],
               (BLINK_TASK_STACK_SIZE / 10u),
               BLINK_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  OSTaskCreate(&tcb_button,
               "button task",
               button_task,
               DEF_NULL,
               BUTTON_TASK_PRIO,
               &stack_button[0],
               (BUTTON_TASK_STACK_SIZE / 10u),
               BUTTON_TASK_STACK_SIZE,
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

  OSQCreate(&message_queue,"Message", 6*sizeof(int),&err);

  OSFlagCreate (&event_flag,
                     "Flag",
                          0,
                     & err);

  OSSemCreate (&semaphore,
              "Semaphore",
                         0,
                     &err);

  OSTmrCreate (&timer,
               "Timer",
               0,
               1,
               OS_OPT_TMR_PERIODIC,
               &callback_2,
               NULL,
               &err);
}

/***************************************************************************//**
 * LED task.
 ******************************************************************************/
static void blink_task(void *arg)
{
    PP_UNUSED_PARAM(arg);
    OS_MSG_SIZE size = sizeof(int);
    RTOS_ERR err;
    while (1)
    {
        OSQPend (&message_queue,
                  0,
                  OS_OPT_PEND_BLOCKING,
                  &size,
                  NULL,
                  &err);
        write_led();
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    }


}
/***************************************************************************//**
 * Button task.
 ******************************************************************************/
static void button_task(void *arg){
  PP_UNUSED_PARAM(arg);
  RTOS_ERR err,err2;
  int button_output = 1;
  while (1)
  {
      OSFlagPend (&event_flag,
                  button0 | button1,
                  0,
                  OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
                  NULL,
                  &err);

      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

      OSQPost (&message_queue,
               &button_output,
               sizeof(int),
               OS_OPT_POST_FIFO,
               &err2);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err2) == RTOS_ERR_NONE));
  }
}
/***************************************************************************//**
 * Slider task.
 ******************************************************************************/
static void slider_task(void *arg){
  PP_UNUSED_PARAM(arg);
  RTOS_ERR err;

  int slider_output = 2;
  timer_start();
  while (1)
  {
      OSSemPend (&semaphore,
                  0,
                  OS_OPT_PEND_BLOCKING,
                  NULL,
                  &err);

      read_capsense();

      OSQPost (&message_queue,
               &slider_output,
               sizeof(int),
               OS_OPT_POST_FIFO,
               &err);


      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  }
}
/***************************************************************************//**
 * idle task.
 ******************************************************************************/
static void idle_task(void *arg)


{
    PP_UNUSED_PARAM(arg);


    while (1)
    {
        EMU_EnterEM1();

    }


}

void interrupt_init(void){
GPIO_IntDisable(CLEAR);// Disable the GPIO Interrupt
GPIO_IntClear (CLEAR);// Clear the GPIO Interrupt

GPIO_ExtIntConfig(BUTTON0_port,BUTTON0_pin,BUTTON0_pin,ENABLE,ENABLE,ENABLE);//Enable the PushButton Interrupt
GPIO_ExtIntConfig(BUTTON1_port,BUTTON1_pin,BUTTON1_pin,ENABLE,ENABLE,ENABLE);


NVIC_EnableIRQ(GPIO_EVEN_IRQn); //Enable the PushButton Interrupt in NVIC
NVIC_EnableIRQ(GPIO_ODD_IRQn);


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
/***************************************************************************//**
 * @brief
 *   Read the position of a finger on the capacitive touch slider
 ******************************************************************************/
void read_capsense(void){

  CAPSENSE_Sense();
  bool pad0,pad1,pad2,pad3;

  pad0 = CAPSENSE_getPressed(PAD0);
  pad1 = CAPSENSE_getPressed(PAD1);
  pad2 = CAPSENSE_getPressed(PAD2);
  pad3 = CAPSENSE_getPressed(PAD3);
  //Determine the position of a finger on the capacitive touch slider
  // 0: N/A
  // 1: LEFT side
  // 2: Right side

  //When both slider are pressed, no led is lit
  if((pad2 == true || pad3 == true) && (pad0 == true || pad1 == true)){
        slider_dir = NA;
    }
  else if(pad0 == true || pad1 == true){
      slider_dir = LEFT;
  }
  else if (pad2 == true || pad3 == true){
      slider_dir = RIGHT;
  }

  else{
      slider_dir = NA;
  }
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
  OSFlagPost (&event_flag,
              button0,
              OS_OPT_POST_FLAG_SET,
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
  OSFlagPost (&event_flag,
              button1,
              OS_OPT_POST_FLAG_SET,
              &err);
}

void timer_start(){
  RTOS_ERR err;
  OSTmrStart (&timer,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}
