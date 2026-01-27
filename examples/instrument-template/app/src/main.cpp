/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bindesc.h>

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


#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#ifdef CONFIG_APP_BLINKY
#include "led.h"
#else
void setup_leds() {}
void led_work_handler(struct k_work *work) {
	//printk("Blink\n");
}
#endif


#ifdef CONFIG_SETTINGS
#include "settings.h"
#endif

#ifdef CONFIG_APP_SCPI
#include <array>
#include <scpi/scpi.h>
#include "scpi.hpp"
#endif

#define BINDESC_ID_APP_VERSION_STRING   APP_VERSION

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

K_WORK_DEFINE(led_work, led_work_handler);

void led_timer_handler(struct k_timer *timer_id) {
  k_work_submit_to_queue(&workq, &led_work);
}

K_TIMER_DEFINE(timer, led_timer_handler, NULL);


void start_led_timer() {
  k_timer_start(&timer, K_MSEC(0), K_MSEC(CONFIG_SAMPLE_LED_UPDATE_DELAY));
}


#ifdef CONFIG_BOOT_SERIAL_BOOT_MODE
#include <zephyr/shell/shell.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/reboot.h>
static int cmd_mcuboot_serial(const struct shell *sh, size_t argc, char **argv)
{

	shell_print(sh, "\n");
	int rc = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);

	if (rc == 0)
		sys_reboot(SYS_REBOOT_WARM);
	
	return 0;
}

SHELL_CMD_REGISTER(serial_recovery, NULL, "Serial recovery", cmd_mcuboot_serial);
#endif


int main(void) {

  k_work_queue_start(&workq, workq_stack, K_THREAD_STACK_SIZEOF(workq_stack),
                     WORKQ_PRIORITY, NULL);
	start_led_timer();

#ifdef CONFIG_APP_SCPI
  console_init();
  start_scpi_thread();
#endif

#ifdef CONFIG_SETTINGS
	const int retval = app_settings_init();
#endif
  return 0;
}
