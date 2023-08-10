/***************************************************************************//**
 * @file
 * @brief Top level application functions
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
#include "app.h"
#include "os.h"
#include "em_emu.h"
#include "glib.h"
#include "dmd.h"
#include "fifo.h"
#include "sl_board_control.h"
#include "sl_board_control_config.h"
#include <stdio.h>
#include "capsense.h"

void app_init(void)
{

  gpio_open();
  CAPSENSE_Init();
  data_str_init();
  interrupt_init();
  blink_init();
  itc_init();


}
