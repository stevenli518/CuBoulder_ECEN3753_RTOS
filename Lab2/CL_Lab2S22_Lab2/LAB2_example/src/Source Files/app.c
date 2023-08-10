/*
 * letimer.c
 *
 *  Created on: May 13, 2019
 *      Author: kgraham
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"

//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************
uint32_t button0_state = 1;
uint32_t button1_state = 1;
uint32_t slider_dir;
//***********************************************************************************
// function
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Setup the cmu and gpio.
 ******************************************************************************/
void app_peripheral_setup(void){
  cmu_open();
  gpio_open();//TODO BUTTON GPIO SETUP///////////////////////////////////////////////////////

}
/***************************************************************************//**
 * @brief
 *   Read the button0 state
 ******************************************************************************/
void read_button0(void){//0: pressed, 1: not pressed
  button0_state = GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);

}
/***************************************************************************//**
 * @brief
 *   Read the button 1 state
 ******************************************************************************/
void read_button1(void){
  button1_state = GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);

}
/***************************************************************************//**
 * @brief
 *   Read the position of a finger on the capacitive touch slider
 ******************************************************************************/
void read_capsense(void){


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
