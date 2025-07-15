#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <nrfx_power.h>
#include <zephyr/sys/poweroff.h>
#include <zephyr/drivers/gpio.h>
#include "mic.h"
#include "button.h"
#include "adc.h"
#include "imu.h"
#include "systemoff.h"

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static int init_module(void)
{
	int ret;

	nrfx_power_constlat_mode_request();
	
	ret = mic_init();
	if (ret < 0)
	{
		printk("Failed to initialize mic module (%d)\n", ret);
	}

	ret = adc_init();
	if (ret < 0)
	{
		printk("Failed to initialize battery module (%d)\n", ret);
	}

	ret = imu_init();
	if (ret < 0)
	{
		printk("Failed to initialize imu module (%d)\n", ret);
	}
	return 0;
}

int main(void)
{
	if (init_module() < 0)
	{
		return -1;
	}
	printf("please start test for %s\n", CONFIG_BOARD_TARGET);
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE | GPIO_OUTPUT_HIGH);

	while (1)
	{
		shell_execute_cmd(NULL, "button check");
		gpio_pin_toggle_dt(&led0);
		if(sys_off_flag == true)
		{
			gpio_pin_set_dt(&led0, 1);
			sys_poweroff();
		}
	}
	return 0;
}
