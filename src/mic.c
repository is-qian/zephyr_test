#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/audio/dmic.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/util.h>

#define BITS_PER_BYTE 8

#define SAMPLE_RATE_HZ 16000
#define SAMPLE_BITS    16
#define TIMEOUT_MS     1000
#define CAPTURE_MS     500
#define BLOCK_SIZE     ((SAMPLE_BITS / BITS_PER_BYTE) * (SAMPLE_RATE_HZ * CAPTURE_MS) / 1000)
#define BLOCK_COUNT    4

static const struct device *const dmic = DEVICE_DT_GET(DT_ALIAS(dmic20));
static const struct device *const  pdm_reg = DEVICE_DT_GET(DT_NODELABEL(pdm_imu_pwr));

K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

static struct pcm_stream_cfg stream = {
	.pcm_rate = SAMPLE_RATE_HZ,
	.pcm_width = SAMPLE_BITS,
	.block_size = BLOCK_SIZE,
	.mem_slab = &mem_slab,
};

static struct dmic_cfg cfg = {
	.io =
		{
			.min_pdm_clk_freq = 1000000,
			.max_pdm_clk_freq = 3500000,
			.min_pdm_clk_dc = 40,
			.max_pdm_clk_dc = 60,
		},
	.streams = &stream,
	.channel =
		{
			.req_num_streams = 1,
			.req_num_chan = 1,
		},
};

static bool initialized;

static int cmd_mic_capture(const struct shell *sh, size_t argc, char **argv)
{
	int ret,time = 1;
	void *buffer;
	uint32_t size;
	int16_t max_data = 0, min_data = 0, max_consecutive = 0;
	bool is_print = true;
	
	if (argc > 1) {
		time = atoi(argv[1]);
		// if time is 0, then do not print the data
		if (time < 1) {
			is_print = false;
			time = 1;
		}
	}
	time *= (1000 / CAPTURE_MS);
	if (!initialized) {
		shell_error(sh, "Microphone module not initialized");
		return -EPERM;
	}
	regulator_enable(pdm_reg);
	shell_print(sh, "S");
	ret = dmic_configure(dmic, &cfg);
	if (ret < 0) {
		shell_error(sh, "Failed to configure DMIC(%d)", ret);
		return ret;
	}
	ret = dmic_trigger(dmic, DMIC_TRIGGER_START);
	if (ret < 0) {
		shell_error(sh, "START trigger failed (%d)", ret);
		return ret;
	}
	for (int i = 0; i < time; i++) {
		ret = dmic_read(dmic, 0, &buffer, &size, TIMEOUT_MS);
		if (ret < 0) {
			shell_error(sh, "DMIC read failed (%d)", ret);
			k_mem_slab_free(&mem_slab, buffer);
			dmic_trigger(dmic, DMIC_TRIGGER_STOP);
			regulator_disable(pdm_reg);
			return ret;
		}

        int16_t *pdata = (int16_t *)buffer;
        int consecutive = 1;
        int max_consecutive_local = 1;
        for (int j = 0; j < size / sizeof(int16_t); j++)
        {
            if(j > 100)
            {
                if (pdata[j] > max_data) {
                    max_data = pdata[j];
                }
                if (pdata[j] < min_data) {
                    min_data = pdata[j];
                }
            }
            // 计算连续不变的值最大持续长度
            if (j > 0 && pdata[j] == pdata[j-1]) {
                consecutive++;
                if (consecutive > max_consecutive_local) {
                    max_consecutive_local = consecutive;
                }
            } else {
                consecutive = 1;
            }
            if (is_print)
                shell_print(sh, "%d", pdata[j]);
		}
		k_mem_slab_free(&mem_slab, buffer);
	}
	ret = dmic_trigger(dmic, DMIC_TRIGGER_STOP);
	if (ret < 0) {
		shell_error(sh, "STOP trigger failed (%d)", ret);
		regulator_disable(pdm_reg);
		return ret;
	}
	shell_print(sh, "E");
	shell_print(sh, "audio data Max: %d Min: %d Max consecutive: %d", max_data, min_data, max_consecutive);
	regulator_disable(pdm_reg);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_mic_cmds,
			       SHELL_CMD(capture, NULL, "Capture microphone data", cmd_mic_capture),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(mic, &sub_mic_cmds, "Microphone", NULL);

int mic_init(void)
{
	if (!device_is_ready(dmic)) {
		return -ENODEV;
	}

	cfg.channel.req_chan_map_lo = dmic_build_channel_map(0, 0, PDM_CHAN_LEFT);

	initialized = true;

	return 0;
}
