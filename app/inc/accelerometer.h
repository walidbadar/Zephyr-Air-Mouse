/*
 * Copyright (c) 2025, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ACCELEROMETER_H__
#define __ACCELEROMETER_H__

#include <stdio.h>
#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

extern struct k_msgq accel_tap_msgq;
extern struct k_msgq accel_drdy_msgq;

struct accelerometer_data {
	int8_t x;
	int8_t y;
	int8_t z;
};

uint32_t tap_duration(uint32_t current_time);
int accelerometer_init(const struct device *dev);

#endif
