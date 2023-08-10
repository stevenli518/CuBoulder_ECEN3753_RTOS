//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdlib.h>
#include "em_cmu.h"
#include "cmu.h"
#include "gpio.h"
#include "capsense.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define PRESS 0
#define NPRESS 1
#define PAD0  0
#define PAD1  1
#define PAD2  2
#define PAD3  3

#define ENABLE 1
#define NA 0
#define LEFT 1
#define RIGHT 2
#define CLEAR 0xffffffff
#define delay100ms 100
#define LAB2_USE_INTERRUPT 1
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void app_letimer0_open(void);

void read_button0(void);
void read_button1(void);
void read_capsense(void);
void write_led(void);
