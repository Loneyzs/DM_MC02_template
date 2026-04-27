/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_DRIVERS_IMU_HEATER_H_
#define APP_DRIVERS_IMU_HEATER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float target_temp_c;
	float kp;
	float ki;
	float kd;
	float max_output;
	float pwm_scale;
} imu_heater_config_t;

typedef struct {
	float output;
	float duty_percent;
	float error;
} imu_heater_state_t;

int imu_heater_init(void);
int imu_heater_stop(void);
int imu_heater_set_output(float output);
int imu_heater_update(float temp_c, imu_heater_state_t *state);
void imu_heater_reset_controller(void);
void imu_heater_get_state(imu_heater_state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* APP_DRIVERS_IMU_HEATER_H_ */
