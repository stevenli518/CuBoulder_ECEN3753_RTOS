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
uint32_t button0_state = 1;
uint32_t button1_state = 1;
uint32_t slider_dir;
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void blink_task(void *arg);
static void slider_task(void *arg);
static void button_task(void *arg);
static void idle_task(void *arg);
void write_led(void);
void read_capsense(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

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
  else if((button0_state == PRESS) || slider_dir == LEFT){
      GPIO_PinOutSet(LED0_port, LED0_pin);
  }
  //LED1 - While BTN1 is pressed or while the slider position is right of center, turn off otherwise
  else if ((button1_state == PRESS) || slider_dir == RIGHT ){
      GPIO_PinOutSet(LED1_port, LED1_pin);
  }

  else{
      GPIO_PinOutClear(LED0_port, LED0_pin);
      GPIO_PinOutClear(LED1_port, LED1_pin);
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
 * LED task.
 ******************************************************************************/
static void blink_task(void *arg)
{
    PP_UNUSED_PARAM(arg);

    RTOS_ERR err;
    while (1)
    {
        write_led();
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    }


}
/***************************************************************************//**
 * Button task.
 ******************************************************************************/
static void button_task(void *arg){
  PP_UNUSED_PARAM(arg);

  RTOS_ERR err;
  while (1)
  {
      button0_state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);
      button1_state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);
      OSTimeDly(10, OS_OPT_TIME_DLY, &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  }
}
/***************************************************************************//**
 * Slider task.
 ******************************************************************************/
static void slider_task(void *arg){
  PP_UNUSED_PARAM(arg);

  RTOS_ERR err;
  while (1)
  {
      read_capsense();
      OSTimeDly(10, OS_OPT_TIME_DLY, &err);
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
