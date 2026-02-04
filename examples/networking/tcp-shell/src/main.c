/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/console/tty.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/dsp/print_format.h>
#include <zephyr/kernel.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util_macro.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "console_server.h"
#include "led.h"

#define WORKQ_STACK_SIZE 1024
#define WORKQ_PRIORITY K_LOWEST_APPLICATION_THREAD_PRIO

K_THREAD_STACK_DEFINE(workq_stack, WORKQ_STACK_SIZE);
static struct k_work_q workq;

K_WORK_DEFINE(led_work, led_work_handler);

void led_timer_handler(struct k_timer *timer_id) {
  k_work_submit_to_queue(&workq, &led_work);
}

K_TIMER_DEFINE(timer, led_timer_handler, NULL);

void start_led_timer() {
  k_timer_start(&timer, K_MSEC(0), K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY));
}
void run_workqueues_and_timers() {
  k_work_queue_start(&workq, workq_stack, K_THREAD_STACK_SIZEOF(workq_stack),
                     WORKQ_PRIORITY, NULL);
  start_led_timer();
}

int main() {
  run_workqueues_and_timers();
  run_server();
  return 0;
}
