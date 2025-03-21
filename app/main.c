/*
 * Copyright (c) 2025, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "accelerometer.h"
#include "usb_dev.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(air_mouse, LOG_LEVEL_INF);

int main(void)
{
	const struct device *hid = DEVICE_DT_GET_ONE(zephyr_hid_device);
	const struct device *const accel_dev = DEVICE_DT_GET_ONE(st_lis2dh);
	const struct gpio_dt_spec hid_sw = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
	struct gpio_callback hid_sw_cb;

	if (gpio_init(&hid_sw, &hid_sw_cb)) {
		LOG_ERR("GPIO intialization failed");
		return -EIO;
	}

	if (accelerometer_init(accel_dev)) {
		LOG_ERR("Accelerometer intialization failed");
		return -EIO;
	}

	if (usb_dev_init(hid)) {
		LOG_ERR("HID Device intialization failed");
		return -EIO;
	}

	while (1) {
		if (usb_dev_write(hid)) {
			LOG_ERR("Failed to write usb report\n");
		}
	}
	return 0;
}
