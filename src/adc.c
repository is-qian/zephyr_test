#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include "adc.h"

LOG_MODULE_REGISTER(adc);

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

static const struct device *const  vbat_reg = DEVICE_DT_GET(DT_NODELABEL(vbat_pwr));

static int cmd_adc_get(const struct shell *sh, size_t argc, char **argv)
{
	int err;
	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	regulator_disable(vbat_reg);

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			shell_error(sh, "ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			shell_error(sh, "Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
	}

		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			int32_t val_mv;

			shell_print(sh, "channel %d: ",
			       adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read_dt(&adc_channels[i], &sequence);
			if (err < 0) {
				shell_error(sh, "Could not read (%d)\n", err);
				continue;
			}

			/*
			 * If using differential mode, the 16 bit value
			 * in the ADC sample buffer should be a signed 2's
			 * complement value.
			 */
			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}
			shell_print(sh, "		raw:%"PRId32, val_mv);
			err = adc_raw_to_millivolts_dt(&adc_channels[i],
						       &val_mv);
			/* conversion to mV may not be supported, skip if not */
			if (err < 0) {
				shell_error(sh, " (value in mV not available)\n");
			} else {
				shell_print(sh, "		vol = %"PRId32" mV\n", val_mv);
			}
		}

	regulator_disable(vbat_reg);
    return 0;
}

int adc_init(void)
{ 
    int err;
    err = regulator_disable(vbat_reg);
    if (err < 0)
    {
        LOG_ERR("Failed to configure enable pin (%d)", err);
        return err;
    }

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_adc_cmds,
                               SHELL_CMD(get, NULL, "Get ADC voltage", cmd_adc_get),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(adc_read, &sub_adc_cmds, "Get ADC voltage", NULL);