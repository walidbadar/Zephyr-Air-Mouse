/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "accelerometer.h"
#include "usb_dev.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(accelerometer, LOG_LEVEL_INF);

#define HPCLICK_EN_BIT	BIT(2)  /* High pass filter click enable bit */
#define TAP_THRESHOLD   3U      /* Movement detection threshold - sensitivity for trigger (in g) */
#define TAP_DURATION    10 	    /* Movement detection duration (in samples) */

K_MSGQ_DEFINE(accel_tap_msgq, sizeof(uint64_t), 2, 1);
K_MSGQ_DEFINE(accel_drdy_msgq, sizeof(struct accelerometer_data), 2, 1);

int64_t tap_duration(int64_t current_time) {
    int64_t duration = 0;
    static int64_t last_tap_time;
    
    if (last_tap_time > 0) {
        duration = current_time - last_tap_time;
    }
    
    last_tap_time = current_time;
    return duration;
}

static void accel_tap_handler(const struct device *dev, const struct sensor_trigger *trig)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(trig);
    int ret;
    uint8_t tap_event = 1;

    ret = k_msgq_put(&accel_tap_msgq, &tap_event, K_NO_WAIT);
    if (ret < 0) {
        LOG_ERR("Message queue full, tap event lost");
    }

    LOG_DBG("TAP Detected");
}

static void accel_drdy_handler(const struct device *dev, const struct sensor_trigger *trig)
{
    ARG_UNUSED(trig);
    struct sensor_value drdy_event[3];
    struct accelerometer_data accel_data;

    if (sensor_sample_fetch(dev)) {
		LOG_ERR("sensor_sample_fetch failed\n");
	}

    if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, drdy_event)) {
		LOG_ERR("sensor_channel_get failed\n");
	}
    
    accel_data.x = drdy_event[0].val1;
    accel_data.y = drdy_event[1].val1;
    accel_data.z = drdy_event[2].val1;

    if (k_msgq_put(&accel_drdy_msgq, &accel_data, K_NO_WAIT)) {
        LOG_ERR("Message queue full, tap event lost");
    }
}

int accelerometer_init(const struct device *dev)
{
    struct sensor_value tap_threshold = {0};
    struct sensor_value tap_duration = {0};
    struct sensor_value hp_filter = {0};
    struct sensor_trigger tap_trig = {0};
    struct sensor_trigger drdy_trig = {0};
    int ret;
    
    if (!device_is_ready(dev)) {
		LOG_ERR("Device %s is not ready", dev->name);
		return -ENODEV;
	}

    hp_filter.val1 = HPCLICK_EN_BIT;
    hp_filter.val2 = 0;
    
    ret = sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ,
                         SENSOR_ATTR_CONFIGURATION,
                         &hp_filter);
    if (ret != 0) {
        printf("Failed to set high pass filter: %d\n", ret);
        return ret;
    }

    tap_threshold.val1 = TAP_THRESHOLD; /* 2.99g */
    tap_threshold.val2 = 0;
    
    ret = sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ,
                         SENSOR_ATTR_SLOPE_TH,
                         &tap_threshold);
    if (ret != 0) {
        printf("Failed to set tap threshold: %d\n", ret);
        return ret;
    }

    tap_duration.val1 = TAP_DURATION; /* 10 samples = 100 mili seconds */
    tap_duration.val2 = 0;
    
    ret = sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ,
                         SENSOR_ATTR_SLOPE_DUR,
                         &tap_duration);
    if (ret != 0) {
        printf("Failed to set tap duration: %d\n", ret);
        return ret;
    }

    /* Set up the tap trigger */
    tap_trig.type = SENSOR_TRIG_TAP;

    ret = sensor_trigger_set(dev, &tap_trig, accel_tap_handler);
    if (ret < 0) {
        LOG_ERR("Failed to set tap trigger: %d", ret);
        return ret;
    }

     /* Set up the data read trigger */
    drdy_trig.type = SENSOR_TRIG_DATA_READY;
    drdy_trig.chan = SENSOR_CHAN_ACCEL_XYZ;

    ret = sensor_trigger_set(dev, &drdy_trig, accel_drdy_handler);
    if (ret < 0) {
        LOG_ERR("Failed to set data ready trigger: %d", ret);
        return ret;
    }

    return 0;
}
