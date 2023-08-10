/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"
#include "main.h"
#include "app.h"

static volatile uint32_t msTicks = 0;


/***************************************************************************//**
 * @brief
 *   Read capacitive touch sensor and drive the LEDs based on the reading every 100ms.
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////
  if(msTicks == 100){
      CAPSENSE_Sense();
      read_capsense();
      write_led();
      msTicks = 0;
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
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  read_button0();
}

/***************************************************************************//**
 * @brief
 *   Interrupt handler to service pressing of button1
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
//TODO///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  uint32_t int_flag;
  int_flag = GPIO->IF & GPIO->IEN;
  GPIO->IFC = int_flag;
  read_button1();
}

int main(void)
{

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFRCO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

  /* Call application program to open / initialize all required peripheral */
  app_peripheral_setup();//TODO GPIO BUTTON SETUP//////////////////////////////////////////////////////////////

  #if LAB2_USE_INTERRUPT //SWITCH definition can be found in app.h
    GPIO_IntDisable(CLEAR);// Disable the GPIO Interrupt
    GPIO_IntClear (CLEAR);// Clear the GPIO Interrupt

    GPIO_ExtIntConfig(BUTTON0_port,BUTTON0_pin,BUTTON0_pin,ENABLE,ENABLE,ENABLE);//Enable the PushButton Interrupt
    GPIO_ExtIntConfig(BUTTON1_port,BUTTON1_pin,BUTTON1_pin,ENABLE,ENABLE,ENABLE);


    NVIC_EnableIRQ(GPIO_EVEN_IRQn); //Enable the PushButton Interrupt in NVIC
    NVIC_EnableIRQ(GPIO_ODD_IRQn);

    uint32_t stat;
    uint32_t ticks;
    /* Setup SysTick Timer for 1 msec interrupts  */
     ticks = CMU_ClockFreqGet(cmuClock_CORE) / 1000;  //Enable the timer to start the timer interrupt
     stat = SysTick_Config(ticks);
     EFM_ASSERT(!stat);
  #endif
  CAPSENSE_Init();


//TODO reccommend looking through the functions in capsense.c and capsense.h to determine best way to read slider
  //Hint you will need to call CAPSENSE_Sense(); followed by another a function you create that likely calls like CAPSENSE_getPressed(int) to
  //determine slider position.
  while(1)
    {

  #if LAB2_USE_INTERRUPT
      EMU_EnterEM1();
  #endif
  #if !(LAB2_USE_INTERRUPT)
  read_button0();
  for (int i = 0; i < 1000000; i++);
  read_button1();
  for (int i = 0; i < 1000000; i++);
  CAPSENSE_Sense();
  read_capsense();
  for (int i = 0; i < 1000000; i++);
  write_led();
  #endif
    }

}
