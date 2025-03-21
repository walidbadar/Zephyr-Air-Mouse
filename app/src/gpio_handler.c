/*
 * Copyright (c) 2025, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio_handler.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gpio_handler, LOG_LEVEL_INF);

static void sw_fn(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int gpio_init(const struct gpio_dt_spec *sw, struct gpio_callback *sw_cb) {
    int ret;

	if (!gpio_is_ready_dt(sw)) {
		printk("Error: button device %s is not ready\n",
		       sw->port->name);
		return -ENODEV;
	}

    ret = gpio_pin_configure_dt(sw, GPIO_INPUT);
	if (ret < 0) {
		LOG_ERR("Failed to configure %s pin %d\n",
		       sw->port->name, sw->pin);
		return -EIO;
	}

	ret = gpio_pin_interrupt_configure_dt(sw, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
		LOG_ERR("Failed to configure interrupt on %s pin %d\n",
			sw->port->name, sw->pin);
		return -EIO;
	}

	gpio_init_callback(sw_cb, sw_fn, BIT(sw->pin));
	gpio_add_callback(sw->port, sw_cb);

    return 0;
}