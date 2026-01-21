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
#include <zephyr/drivers/uart.h>
#include <zephyr/dsp/print_format.h>
#include <zephyr/kernel.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/app_version.h>
#include <scpi/scpi.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "scpi.hpp"
#include <array>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

//BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
//             "Console device is not ACM CDC UART device");

/* Declare the timer */
#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#define WORKQ_STACK_SIZE 1024
#define WORKQ_PRIORITY K_LOWEST_APPLICATION_THREAD_PRIO

K_THREAD_STACK_DEFINE(workq_stack, WORKQ_STACK_SIZE);
static struct k_work_q workq;

static std::array<uint8_t, 256> scpi_input_buffer{};
static std::array<scpi_error_t, 8> scpi_error_buffer{};
static size_t SCPI_Write(scpi_t *context, const char *data, size_t len) {
  /*
   * uart or tcp output
   */
  // std::array<char, 1024> buff{};
  // snprintf(buff.data(), buff.size(), "%s", data);
  console_write(NULL, data, len);
  // printk("%s", data);
  // printk("\r\n%s", data);
  return SCPI_RES_OK;
}

static int SCPI_Error(scpi_t *context, int_fast16_t err) {
  std::array<char, 32> buff{};
  snprintf(buff.data(), buff.size(), "SCPI Error: %d\r\n", err);
  console_write(NULL, buff.data(), buff.size());
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl,
                                  scpi_reg_val_t val) {

  std::array<char, 32> buff{};
  snprintf(buff.data(), buff.size(), "SCPI Control: %d\r\n", val);
  console_write(NULL, buff.data(), buff.size());
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t *context) { return SCPI_RES_OK; }
static scpi_result_t SCPI_Flush(scpi_t *context) { return SCPI_RES_OK; }

K_THREAD_STACK_DEFINE(scpi_stack, 512);
struct k_thread scpi_thread_data;

#define BOARD_IDENTITY CONFIG_BOARD CONFIG_BOARD_REVISION
static ScpiInterfaceContainer scpi_interface(
    {"Hobbs ElectroOptics", BOARD_IDENTITY, APP_VERSION_EXTENDED_STRING, ""},
    {scpi_commands_table.data(), scpi_commands_table.size()},
    {scpi_input_buffer.data(), scpi_input_buffer.size()},
    {scpi_error_buffer.data(), scpi_error_buffer.size()},
    {SCPI_Error, SCPI_Write, SCPI_Control, SCPI_Flush, SCPI_Reset});

void scpi_input_thread(void *arg1, void *arg2, void *arg3) {
  ARG_UNUSED(arg1);
  ARG_UNUSED(arg2);
  ARG_UNUSED(arg3);

  while (true) {
    const auto value = console_getchar();
    if (value >= 0) {
	  const char ch = static_cast<char>(value);
	  scpi_interface.input(&ch, 1);
	} else {
      k_sleep(K_MSEC(1));
	}
  }
}

int main(void) {
  k_work_queue_start(&workq, workq_stack, K_THREAD_STACK_SIZEOF(workq_stack),
                     WORKQ_PRIORITY, NULL);

  console_init();
  k_thread_create(&scpi_thread_data, scpi_stack,
                  K_THREAD_STACK_SIZEOF(scpi_stack), scpi_input_thread, NULL,
                  NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);

  return 0;
}
