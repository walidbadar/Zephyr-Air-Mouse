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

int64_t tap_duration(int64_t current_time);
int accelerometer_init(const struct device *dev);

#endif
