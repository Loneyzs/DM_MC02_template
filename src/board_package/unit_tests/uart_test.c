/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "board_package/unit_tests/unit_tests.h"

#include <errno.h>
#include <stddef.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(test_uart, LOG_LEVEL_INF);

#define UART_NODE DT_NODELABEL(usart1)

static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE);
static K_MUTEX_DEFINE(uart_lock);

static void uart_write(const char *text)
{
	if (text == NULL) {
		return;
	}

	k_mutex_lock(&uart_lock, K_FOREVER);
	for (size_t i = 0; text[i] != '\0'; i++) {
		uart_poll_out(uart_dev, text[i]);
	}
	k_mutex_unlock(&uart_lock);
}

int test_uart_start(void)
{
	if (!device_is_ready(uart_dev)) {
		LOG_ERR("USART1 test UART is not ready");
		return -ENODEV;
	}

	uart_write("DM_MC02_USART1_TEST_OK\r\n");
	LOG_INF("USART1 minimal test banner sent");
	return 0;
}
