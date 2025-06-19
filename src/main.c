#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <nrfx_power.h>
#include "mic.h"
#include "button.h"
#include "adc.h"
#include "imu.h"

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
	return 0;
}
