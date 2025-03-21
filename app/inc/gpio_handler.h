#ifndef __GPIO_HANDLER_H__
#define __GPIO_HANDLER_H__

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

int gpio_init(const struct gpio_dt_spec *sw, struct gpio_callback *sw_cb);

#endif
