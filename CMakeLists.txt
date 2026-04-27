cmake_minimum_required(VERSION 3.20.0)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(dm_mc02_template)

set(APP_SOURCES
    src/main.c
    src/drivers/imu_heater/imu_heater.c
    src/board_package/unit_tests/uart_test.c
    src/board_package/unit_tests/pwm_test.c
    src/board_package/unit_tests/can_loopback_test.c
    src/board_package/unit_tests/bmi088_test.c
    src/board_package/unit_tests/imu_temperature_test.c
)

target_sources(app PRIVATE ${APP_SOURCES})
target_include_directories(app PRIVATE include)

target_compile_options(app PRIVATE
    -Wall
    -Wextra
)
