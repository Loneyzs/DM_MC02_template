/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "board_package/unit_tests/unit_tests.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(test_pwm, LOG_LEVEL_INF);

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct pwm_dt_spec blink_pwm =
	PWM_DT_SPEC_GET_BY_NAME(ZEPHYR_USER_NODE, blink);

K_THREAD_STACK_DEFINE(pwm_test_stack, 768);
static struct k_thread pwm_test_thread_data;
static bool started;

static void pwm_test_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	while (1) {
		(void)pwm_set_dt(&blink_pwm, PWM_MSEC(20), PWM_MSEC(1));
		k_msleep(500);
		(void)pwm_set_dt(&blink_pwm, PWM_MSEC(20), PWM_MSEC(2));
		k_msleep(500);
	}
}

int test_pwm_start(void)
{
	if (started) {
		return 0;
	}

	if (!pwm_is_ready_dt(&blink_pwm)) {
		LOG_ERR("PWM test output is not ready");
		return -ENODEV;
	}

	k_thread_create(&pwm_test_thread_data, pwm_test_stack,
			K_THREAD_STACK_SIZEOF(pwm_test_stack),
			pwm_test_thread, NULL, NULL, NULL,
			6, 0, K_NO_WAIT);
	k_thread_name_set(&pwm_test_thread_data, "test_pwm");

	started = true;
	LOG_INF("PWM minimal test started on TIM2_CH1/PA0");
	return 0;
}
