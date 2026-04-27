/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_UNIT_TESTS_H_
#define APP_UNIT_TESTS_H_

#ifdef __cplusplus
extern "C" {
#endif

int test_uart_start(void);
int test_pwm_start(void);
int test_can_loopback_start(void);
int test_bmi088_start(void);
int test_imu_temp_start(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_UNIT_TESTS_H_ */
