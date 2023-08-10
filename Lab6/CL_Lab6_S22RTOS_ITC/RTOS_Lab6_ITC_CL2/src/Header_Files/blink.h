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

#ifndef BLINK_H
#define BLINK_H

#include "gpio.h"
#include "capsense.h"
#define BLINK_TASK_STACK_SIZE     512

#define BLINK_TASK_PRIO            20

#define BUTTON_TASK_STACK_SIZE      512

#define BUTTON_TASK_PRIO            15

#define SLIDER_TASK_STACK_SIZE      512

#define SLIDER_TASK_PRIO            15

#define IDLE_TASK_STACK_SIZE      96

#define IDLE_TASK_PRIO            30

#define PAD0  0
#define PAD1  1
#define PAD2  2
#define PAD3  3
#define NA 0
#define LEFT 1
#define RIGHT 2

#define PRESS 0
#define NPRESS 1
#define NA 0
#define LEFT 1
#define RIGHT 2

#define ENABLE 1
#define CLEAR 0xffffffff
/***************************************************************************//**
 * Initialize blink example
 ******************************************************************************/
void blink_init(void);
void itc_init(void);
void interrupt_init(void);
void timer_start(void);

#endif  // BLINK_H
