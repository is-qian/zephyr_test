#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/regulator.h>
#include <nrfx.h>

static const struct device *const imu = DEVICE_DT_GET(DT_NODELABEL(lsm6dso));
static const struct device *const  lsm6dso_reg = DEVICE_DT_GET(DT_NODELABEL(pdm_imu_pwr));

static int cmd_imu_get(const struct shell *sh, size_t argc, char **argv)
{
    int ret;
    struct sensor_value accel_data[3];
    struct sensor_value gyro_data[3];
    struct sensor_value odr_attr;
    /* set accel/gyro sampling frequency to 12.5 Hz */
    odr_attr.val1 = 12.5;
    odr_attr.val2 = 0;
    regulator_enable(lsm6dso_reg);

    k_sleep(K_MSEC(100));
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
    k_sleep(K_MSEC(500));
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

    regulator_disable(lsm6dso_reg);
    for (int i = 0; i < 3; i++) {
        if (accel_data[i].val1 < 0 && accel_data[i].val2 > 0) {
            accel_data[i].val2 = 1000000 - accel_data[i].val2;
        }
        if (gyro_data[i].val1 < 0 && gyro_data[i].val2 > 0) {
            gyro_data[i].val2 = 1000000 - gyro_data[i].val2;
        }
    }
    
    shell_print(sh, "accel data: %d.%06u, %d.%06u, %d.%06u",
              accel_data[0].val1, (uint32_t)abs(accel_data[0].val2),
              accel_data[1].val1, (uint32_t)abs(accel_data[1].val2),
              accel_data[2].val1, (uint32_t)abs(accel_data[2].val2));
    shell_print(sh, "gyro data: %d.%06u, %d.%06u, %d.%06u",
              gyro_data[0].val1, (uint32_t)abs(gyro_data[0].val2),
              gyro_data[1].val1, (uint32_t)abs(gyro_data[1].val2),
              gyro_data[2].val1, (uint32_t)abs(gyro_data[2].val2));
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
    regulator_disable(lsm6dso_reg);
	return 0;
}