#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

static const struct device *const imu = DEVICE_DT_GET(DT_ALIAS(imu0));
static bool initialized;

static int cmd_imu_get(const struct shell *sh, size_t argc, char **argv)
{
	int err;
	struct sensor_value odr, acc_data[3], gyro_data[3];

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!initialized) {
		shell_error(sh, "IMU sensor module not initialized");
		return -EPERM;
	}

	/* ODR: 12.5 Hz */
	odr.val1 = 12;
	odr.val2 = 0;

	err = sensor_attr_set(imu, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (err < 0) {
		return err;
	}

	err = sensor_attr_set(imu, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (err < 0) {
		return err;
	}

	err = sensor_sample_fetch(imu);
	if (err < 0) {
		shell_error(sh, "Failed to fetch sensor data (%d)", err);
		return 0;
	}

	err = sensor_channel_get(imu, SENSOR_CHAN_ACCEL_XYZ, acc_data);
	if (err < 0) {
		shell_error(sh, "Failed to get accelerometer data (%d)", err);
		return 0;
	}

	err = sensor_channel_get(imu, SENSOR_CHAN_GYRO_XYZ, gyro_data);
	if (err < 0) {
		shell_error(sh, "Failed to get gyroscope data (%d)", err);
		return 0;
	}

	shell_print(sh, "Acceleration (m/s2): %.6f, %.6f, %.6f",
		    sensor_value_to_double(&acc_data[0]), sensor_value_to_double(&acc_data[1]),
		    sensor_value_to_double(&acc_data[2]));
	shell_print(sh, "Angular velocity (rad/s): %.6f, %.6f, %.6f",
		    sensor_value_to_double(&gyro_data[0]), sensor_value_to_double(&gyro_data[1]),
		    sensor_value_to_double(&gyro_data[2]));

	/* ODR: 0 (power-down) */
	odr.val1 = 0;
	odr.val2 = 0;

	err = sensor_attr_set(imu, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (err < 0) {
		return err;
	}

	err = sensor_attr_set(imu, SENSOR_CHAN_GYRO_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
	if (err < 0) {
		return err;
	}

	return 0;
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

	initialized = true;

	return 0;
}
