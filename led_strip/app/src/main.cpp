/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/console/console.h>
#include <zephyr/console/tty.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/dsp/print_format.h>
#include <zephyr/kernel.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util_macro.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "led.h"
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>


BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
             "Console device is not ACM CDC UART device");

/* Declare the timer */
#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define WORKQ_STACK_SIZE 1024
#define WORKQ_PRIORITY 5

K_THREAD_STACK_DEFINE(workq_stack, WORKQ_STACK_SIZE);
static struct k_work_q workq;

K_WORK_DEFINE(led_work, led_work_handler);

void led_timer_handler(struct k_timer *timer_id) {
  k_work_submit_to_queue(&workq, &led_work);
}

K_TIMER_DEFINE(timer, led_timer_handler, NULL);


int main(void) {

  k_work_queue_start(&workq, workq_stack, K_THREAD_STACK_SIZEOF(workq_stack),
                     WORKQ_PRIORITY, NULL);
  k_timer_start(&timer, K_MSEC(0), K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY));

  while (true) {
    k_sleep(K_MSEC(10));
  }
  return 0;
}
