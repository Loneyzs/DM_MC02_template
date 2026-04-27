/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "board_package/unit_tests/unit_tests.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(test_bmi088, LOG_LEVEL_INF);

#define BMI088_TEST_PERIOD_MS 200

static const struct device *const bmi_accel = DEVICE_DT_GET(DT_NODELABEL(bmi088_accel));
static const struct device *const bmi_gyro = DEVICE_DT_GET(DT_NODELABEL(bmi088_gyro));

K_THREAD_STACK_DEFINE(bmi088_test_stack, 1536);
static struct k_thread bmi088_test_thread_data;
static bool started;

static int fetch_vec3(const struct device *dev, enum sensor_channel chan,
		      struct sensor_value out[3])
{
	int rc = sensor_sample_fetch(dev);

	if (rc < 0) {
		return rc;
	}

	return sensor_channel_get(dev, chan, out);
}

static double sensor_to_double(const struct sensor_value *value)
{
	return sensor_value_to_double(value);
}

static void bmi088_test_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	uint32_t seq = 0;

	while (1) {
		struct sensor_value accel[3];
		struct sensor_value gyro[3];
		struct sensor_value temp;
		int rc_acc = fetch_vec3(bmi_accel, SENSOR_CHAN_ACCEL_XYZ, accel);
		int rc_gyr = fetch_vec3(bmi_gyro, SENSOR_CHAN_GYRO_XYZ, gyro);
		int rc_tmp = sensor_channel_get(bmi_accel, SENSOR_CHAN_DIE_TEMP, &temp);

		if (rc_acc < 0 || rc_gyr < 0) {
			LOG_WRN("BMI088 ERR seq=%lu acc_rc=%d gyr_rc=%d",
				(unsigned long)seq, rc_acc, rc_gyr);
		} else {
			LOG_INF("BMI088 seq=%lu acc=(%.3f,%.3f,%.3f) gyro=(%.3f,%.3f,%.3f) temp=%.2f rc_tmp=%d",
				(unsigned long)seq,
				sensor_to_double(&accel[0]),
				sensor_to_double(&accel[1]),
				sensor_to_double(&accel[2]),
				sensor_to_double(&gyro[0]),
				sensor_to_double(&gyro[1]),
				sensor_to_double(&gyro[2]),
				(rc_tmp == 0) ? sensor_to_double(&temp) : 0.0,
				rc_tmp);
		}

		seq++;
		k_msleep(BMI088_TEST_PERIOD_MS);
	}
}

int test_bmi088_start(void)
{
	if (started) {
		return 0;
	}

	if (!device_is_ready(bmi_accel)) {
		LOG_ERR("bmi088-accel not ready");
		return -ENODEV;
	}
	if (!device_is_ready(bmi_gyro)) {
		LOG_ERR("bmi088-gyro not ready");
		return -ENODEV;
	}

	k_thread_create(&bmi088_test_thread_data, bmi088_test_stack,
			K_THREAD_STACK_SIZEOF(bmi088_test_stack),
			bmi088_test_thread, NULL, NULL, NULL,
			7, 0, K_NO_WAIT);
	k_thread_name_set(&bmi088_test_thread_data, "test_bmi088");

	started = true;
	LOG_INF("BMI088 upstream-driver test started");
	return 0;
}
