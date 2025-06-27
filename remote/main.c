/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/printk.h>
#include <nrfx_power.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx.h>
#include <zephyr/dt-bindings/gpio/nordic-nrf-gpio.h>

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
int main(void)
{
	nrfx_power_constlat_mode_request();
	printk("Hello world from %s\n", CONFIG_BOARD_TARGET);
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE | GPIO_OUTPUT_HIGH);
	for (int i = 0; i < 10; i++){
		gpio_pin_toggle_dt(&led0);
		k_sleep(K_MSEC(500));
	}
	gpio_pin_set_dt(&led0, 1);

#if defined(CONFIG_CONSOLE)
    const struct device *const cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (!device_is_ready(cons))
    {
        printk("%s: device not ready.\n", cons->name);
        return 0;
    }
#endif


#if defined(CONFIG_CONSOLE)
    if (pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND) < 0)
    {
        printk("Could not suspend console\n");
        return 0;
    }
#endif
    nrfx_power_constlat_mode_free();
    // sys_reboot();
	return 0;
}