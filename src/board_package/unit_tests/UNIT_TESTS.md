# DM-MC02 最小测试

测试入口统一放在 `src/board_package/unit_tests`，公共声明在
`include/board_package/unit_tests/unit_tests.h`。

当前保留这块板实际具备且已完成硬件适配的最小测试：

| 文件 | 入口 | 目的 |
|---|---|---|
| `uart_test.c` | `test_uart_start()` | 通过 USART1 输出测试字符串 |
| `pwm_test.c` | `test_pwm_start()` | 通过 TIM2_CH1/PA0 输出 1ms/2ms 交替 PWM |
| `can_loopback_test.c` | `test_can_loopback_start()` | FDCAN1 内部 loopback 收发 |
| `bmi088_test.c` | `test_bmi088_start()` | 通过 Zephyr 上游 `bosch,bmi08x-*` 驱动读取 BMI088 |
| `imu_temperature_test.c` | `test_imu_temp_start()` | 读取 BMI088 温度并驱动 TIM3_CH4/PB1 加热 PWM |

`IST8310` 和九轴融合 IMU 不属于 DM-MC02 本板硬件/本次范围，未移植。
