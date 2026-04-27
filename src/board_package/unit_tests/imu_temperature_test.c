/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "board_package/unit_tests/unit_tests.h"
#include "drivers/imu/imu_heater.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(test_imu_temp, LOG_LEVEL_INF);

#define IMU_TEMP_TEST_PERIOD_MS 100

static const struct device *const bmi_accel = DEVICE_DT_GET(DT_NODELABEL(bmi088_accel));

K_THREAD_STACK_DEFINE(imu_temp_test_stack, 1536);
static struct k_thread imu_temp_test_thread_data;
static bool started;

static int read_die_temp(float *temp_c)
{
	struct sensor_value temp;
	int rc;

	if (temp_c == NULL) {
		return -EINVAL;
	}

	rc = sensor_sample_fetch(bmi_accel);
	if (rc < 0) {
		return rc;
	}

	rc = sensor_channel_get(bmi_accel, SENSOR_CHAN_DIE_TEMP, &temp);
	if (rc < 0) {
		return rc;
	}

	*temp_c = (float)sensor_value_to_double(&temp);
	return 0;
}

static void imu_temp_test_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	uint32_t seq = 0;

	while (1) {
		float temp_c;
		imu_heater_state_t heater_state;
		int rc = read_die_temp(&temp_c);

		if (rc == 0) {
			rc = imu_heater_update(temp_c, &heater_state);
		}

		if (rc == 0) {
			LOG_INF("IMU_TEMP seq=%lu temp=%.2f target=40.00 heater=%.1f duty=%.2f%% err=%.2f",
				(unsigned long)seq,
				(double)temp_c,
				(double)heater_state.output,
				(double)heater_state.duty_percent,
				(double)heater_state.error);
		} else {
			LOG_WRN("IMU_TEMP_ERR seq=%lu rc=%d", (unsigned long)seq, rc);
		}

		seq++;
		k_msleep(IMU_TEMP_TEST_PERIOD_MS);
	}
}

int test_imu_temp_start(void)
{
	int rc;

	if (started) {
		return 0;
	}

	if (!device_is_ready(bmi_accel)) {
		LOG_ERR("bmi088-accel not ready");
		return -ENODEV;
	}

	rc = imu_heater_init();
	if (rc < 0) {
		LOG_ERR("imu_heater_init failed: %d", rc);
		return rc;
	}

	k_thread_create(&imu_temp_test_thread_data, imu_temp_test_stack,
			K_THREAD_STACK_SIZEOF(imu_temp_test_stack),
			imu_temp_test_thread, NULL, NULL, NULL,
			7, 0, K_NO_WAIT);
	k_thread_name_set(&imu_temp_test_thread_data, "test_imu_temp");

	started = true;
	LOG_INF("IMU temperature/heater test started");
	return 0;
}
