/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "drivers/imu/imu_heater.h"

#include <errno.h>
#include <stdbool.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(imu_heater, LOG_LEVEL_INF);

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct pwm_dt_spec heater_pwm =
	PWM_DT_SPEC_GET_BY_NAME(ZEPHYR_USER_NODE, heater);

static const imu_heater_config_t heater_cfg = {
	.target_temp_c = 40.0f,
	.kp = 100.0f,
	.ki = 50.0f,
	.kd = 10.0f,
	.max_output = 500.0f,
	.pwm_scale = 10000.0f,
};

static K_MUTEX_DEFINE(heater_lock);

static bool initialized;
static float output;
static float err;
static float err_l;
static float err_ll;

static float clampf(float value, float min_value, float max_value)
{
	if (value < min_value) {
		return min_value;
	}
	if (value > max_value) {
		return max_value;
	}
	return value;
}

static int apply_output_locked(float requested_output)
{
	uint32_t pulse;
	double scaled_period;

	output = clampf(requested_output, 0.0f, heater_cfg.max_output);
	scaled_period = (double)heater_pwm.period * (double)output;
	pulse = (uint32_t)(scaled_period / (double)heater_cfg.pwm_scale);

	return pwm_set_dt(&heater_pwm, heater_pwm.period, pulse);
}

int imu_heater_init(void)
{
	int rc;

	if (!pwm_is_ready_dt(&heater_pwm)) {
		LOG_ERR("IMU heater PWM is not ready");
		return -ENODEV;
	}

	k_mutex_lock(&heater_lock, K_FOREVER);
	rc = apply_output_locked(0.0f);
	if (rc == 0) {
		initialized = true;
		err = 0.0f;
		err_l = 0.0f;
		err_ll = 0.0f;
	}
	k_mutex_unlock(&heater_lock);

	return rc;
}

int imu_heater_stop(void)
{
	int rc;

	k_mutex_lock(&heater_lock, K_FOREVER);
	rc = apply_output_locked(0.0f);
	k_mutex_unlock(&heater_lock);

	return rc;
}

int imu_heater_set_output(float requested_output)
{
	int rc;

	if (!initialized) {
		rc = imu_heater_init();
		if (rc < 0) {
			return rc;
		}
	}

	k_mutex_lock(&heater_lock, K_FOREVER);
	rc = apply_output_locked(requested_output);
	k_mutex_unlock(&heater_lock);

	return rc;
}

int imu_heater_update(float temp_c, imu_heater_state_t *state)
{
	float next_output;
	int rc;

	if (!initialized) {
		rc = imu_heater_init();
		if (rc < 0) {
			return rc;
		}
	}

	k_mutex_lock(&heater_lock, K_FOREVER);
	err_ll = err_l;
	err_l = err;
	err = heater_cfg.target_temp_c - temp_c;

	next_output = (heater_cfg.kp * err) +
		      (heater_cfg.ki * (err + err_l + err_ll)) +
		      (heater_cfg.kd * (err - err_l));

	rc = apply_output_locked(next_output);
	if (state != NULL) {
		state->output = output;
		state->duty_percent = (output * 100.0f) / heater_cfg.pwm_scale;
		state->error = err;
	}
	k_mutex_unlock(&heater_lock);

	return rc;
}

void imu_heater_reset_controller(void)
{
	k_mutex_lock(&heater_lock, K_FOREVER);
	err = 0.0f;
	err_l = 0.0f;
	err_ll = 0.0f;
	k_mutex_unlock(&heater_lock);
}

void imu_heater_get_state(imu_heater_state_t *state)
{
	if (state == NULL) {
		return;
	}

	k_mutex_lock(&heater_lock, K_FOREVER);
	state->output = output;
	state->duty_percent = (output * 100.0f) / heater_cfg.pwm_scale;
	state->error = err;
	k_mutex_unlock(&heater_lock);
}
