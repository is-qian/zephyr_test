#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx.h>
#include <zephyr/dt-bindings/gpio/nordic-nrf-gpio.h>

static const struct device *const imu = DEVICE_DT_GET(DT_NODELABEL(lsm6dso));
static const struct gpio_dt_spec lsm6dso_en = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(pdm_imu_en_pin), gpios, {0});

static int cmd_imu_get(const struct shell *sh, size_t argc, char **argv)
{
    int ret;
    struct sensor_value accel_data[3];
    struct sensor_value gyro_data[3];
    struct sensor_value odr_attr;
    /* set accel/gyro sampling frequency to 12.5 Hz */
    odr_attr.val1 = 12.5;
    odr_attr.val2 = 0;
    gpio_pin_set_dt(&lsm6dso_en, 1);

    if (!device_is_ready(imu)) {
        shell_error(sh, "Device not ready\n");
        return -ENODEV;
    }

    ret = sensor_attr_set(imu, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
    if (ret)
    {
        shell_error(sh, "Failed to set accel sampling frequency\n");
        return ret;
    }

    ret = sensor_attr_set(imu, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr);
    if (ret)
    {
        shell_error(sh, "Failed to set gyro sampling frequency\n");
        return ret;
    }

    ret = sensor_sample_fetch(imu);
    if (ret)
    {
        shell_error(sh, "Failed to fetch sample\n");
        return ret;
    }

    ret = sensor_channel_get(imu, SENSOR_CHAN_ACCEL_XYZ, accel_data);
    if (ret)
    {
        shell_error(sh, "Failed to get accel data\n");
        return ret;
    }

    ret = sensor_channel_get(imu, SENSOR_CHAN_GYRO_XYZ, gyro_data);
    if (ret)
    {
        shell_error(sh, "Failed to get gyro data\n");
        return ret;
    }

    gpio_pin_set_dt(&lsm6dso_en, 0);
    shell_print(sh, "accel data: %d.%06d, %d.%06d, %d.%06d", accel_data[0].val1, accel_data[0].val2, accel_data[1].val1, accel_data[1].val2, accel_data[2].val1, accel_data[2].val2);
    shell_print(sh, "gyro data: %d.%06d, %d.%06d, %d.%06d", gyro_data[0].val1, gyro_data[0].val2, gyro_data[1].val1, gyro_data[1].val2, gyro_data[2].val1, gyro_data[2].val2);
    return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_imu_cmds, 
					SHELL_CMD(get, NULL, "Get sensor data", cmd_imu_get),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(imu, &sub_imu_cmds, "IMU sensor", NULL);

int imu_init(void)
{
	if (!device_is_ready(imu)) {
		return -ENODEV;
	}
    gpio_pin_set_dt(&lsm6dso_en, 0);
	return 0;
}

static int imu_poweron(void)
{
    int ret;

    ret = gpio_pin_configure_dt(&lsm6dso_en, (GPIO_OUTPUT | NRF_GPIO_DRIVE_S0H1));
    if (ret < 0)
    {
        return ret;
    }

    ret = gpio_pin_set_dt(&lsm6dso_en, 1);
    if (ret < 0)
    {
        return ret;
    }
    k_sleep(K_MSEC(50));

    return 0;
}

SYS_INIT(imu_poweron, POST_KERNEL, 89);