/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx.h>
#include <zephyr/dt-bindings/gpio/nordic-nrf-gpio.h>

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
int main(void)
{
	printk("Hello world from %s\n", CONFIG_BOARD_TARGET);
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE | GPIO_OUTPUT_HIGH);
	for (size_t i = 0; i < 20; i++)
	{
		gpio_pin_toggle_dt(&led0);
		k_sleep(K_MSEC(500));
	}
	gpio_pin_set_dt(&led0, 1);
	return 0;
}