/*
 * task.h
 *
 *  Created on: 2022年3月1日
 *      Author: Chengming Li
 */

#ifndef SRC_HEADER_FILES_TASK_H_
#define SRC_HEADER_FILES_TASK_H_

#include "gpio.h"

#include "capsense.h"

#define SPEED_TASK_STACK_SIZE     1024

#define SPEED_TASK_PRIO            5

#define DIRECTION_TASK_STACK_SIZE      1024

#define DIRECTION_TASK_PRIO            5

#define MONITOR_TASK_STACK_SIZE      1024

#define MONITOR_TASK_PRIO            10

#define IDLE_TASK_STACK_SIZE      1024

#define IDLE_TASK_PRIO            30

#define LED_TASK_STACK_SIZE      1024

#define LED_TASK_PRIO            20

#define DISPLAY_TASK_STACK_SIZE      1024

#define DISPLAY_TASK_PRIO            20

#define TIME_LIM 5000
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
#define SIZE_DATA 6

void task_init(void);
void itc_init(void);
void interrupt_init(void);
void data_str_init(void);
void blink_init(void);
#endif /* SRC_HEADER_FILES_TASK_H_ */
