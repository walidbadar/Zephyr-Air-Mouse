/*
 * Copyright (c) 2025, Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "accelerometer.h"
#include "usb_dev.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(usb_dev, LOG_LEVEL_INF);

const uint8_t hid_report_desc[] = HID_CUSTOM_REPORT_DESC();
UDC_STATIC_BUF_DEFINE(report, REPORT_COUNT);

K_MSGQ_DEFINE(kb_msgq, sizeof(struct input_event), 2, 1);
static bool hid_ready;

static void input_cb(struct input_event *kb_event, void *user_data)
{
	ARG_UNUSED(user_data);

	if (k_msgq_put(&kb_msgq, kb_event, K_NO_WAIT) != 0) {
		LOG_ERR("Failed to put new input event");
	}
}

INPUT_CALLBACK_DEFINE(NULL, input_cb, NULL);

static void hid_iface_ready(const struct device *dev, const bool ready)
{
	LOG_INF("HID device %s interface is %s",
		dev->name, ready ? "ready" : "not ready");
	hid_ready = ready;
}

static int hid_get_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 uint8_t *const buf)
{
	LOG_WRN("Get Report not implemented, Type %u ID %u", type, id);

	return 0;
}

static int hid_set_report(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 const uint8_t *const buf)
{
	if (type != HID_REPORT_TYPE_OUTPUT) {
		LOG_WRN("Unsupported report type");
		return -ENOTSUP;
	}

	return 0;
}

struct hid_device_ops hid_ops = {
	.iface_ready = hid_iface_ready,
	.get_report = hid_get_report,
	.set_report = hid_set_report,
};

static void msg_cb(struct usbd_context *const usbd_ctx,
		   const struct usbd_msg *const msg)
{
	LOG_INF("USBD message: %s", usbd_msg_type_string(msg->type));

	if (msg->type == USBD_MSG_CONFIGURATION) {
		LOG_INF("\tConfiguration value %d", msg->status);
	}

	if (usbd_can_detect_vbus(usbd_ctx)) {
		if (msg->type == USBD_MSG_VBUS_READY) {
			if (usbd_enable(usbd_ctx)) {
				LOG_ERR("Failed to enable device support");
			}
		}

		if (msg->type == USBD_MSG_VBUS_REMOVED) {
			if (usbd_disable(usbd_ctx)) {
				LOG_ERR("Failed to disable device support");
			}
		}
	}
}

static int enable_usb_device_next(void)
{
	struct usbd_context *sample_usbd;
	int ret;

	sample_usbd = sample_usbd_init_device(msg_cb);
	if (sample_usbd == NULL) {
		LOG_ERR("Failed to initialize USB device");
		return -ENODEV;
	}

	if (!usbd_can_detect_vbus(sample_usbd)) {
		ret = usbd_enable(sample_usbd);
		if (ret) {
			LOG_ERR("Failed to enable device support");
			return ret;
		}
	}

	LOG_DBG("USB device support enabled");

	return 0;
}

int kb_fn(void) {
	struct input_event kb_event = {0};
	int ret;

	ret = k_msgq_get(&kb_msgq, &kb_event, K_NO_WAIT);

	if (ret == 0) {
		LOG_DBG("code: %d, value: %d", kb_event.code, kb_event.value);

       	switch (kb_event.code) {
            case INPUT_KEY_1:
                report[REPORT_IDX] = KB_REPORT_IDX;
                report[BTN_REPORT_IDX] = kb_event.value ? HID_KEY_UP : 0;
                break;
                
            case INPUT_KEY_2:
                report[REPORT_IDX] = KB_REPORT_IDX;
                report[BTN_REPORT_IDX] = kb_event.value ? HID_KEY_DOWN : 0;
                break;

			case INPUT_KEY_3:
				report[REPORT_IDX] = CONSUMER_REPORT_IDX;
                report[BTN_REPORT_IDX] = kb_event.value ? HID_CONSUMER_PLAY_PAUSE : 0;
                break;
                
            case INPUT_KEY_4:
                report[REPORT_IDX] = CONSUMER_REPORT_IDX;
                report[BTN_REPORT_IDX] = kb_event.value ? HID_CONSUMER_SCAN_NEXT_TRACK : 0;
                break;

            default:
                LOG_INF("Unknown key code %d", kb_event.code);
                return ret;
        }
	}

	return ret;
}

int mouse_fn(void) {
    struct accelerometer_data accel_data = {0};
    uint64_t duration = 0;
    uint8_t tap_event = 0;
	int ret;

	ret = k_msgq_get(&accel_drdy_msgq, &accel_data, K_NO_WAIT);
    if (ret == 0) {
        LOG_DBG("X: %d, Y: %d", accel_data.x, accel_data.y);
		report[REPORT_IDX] = MOUSE_REPORT_IDX;
        report[X_REPORT_IDX] = -accel_data.y * MOUSE_SENSITIVITY_X;
        report[Y_REPORT_IDX] = -accel_data.x * MOUSE_SENSITIVITY_Y;
		return ret;
    }

    ret = k_msgq_get(&accel_tap_msgq, &tap_event, K_NO_WAIT);
	if (ret == 0) {
        duration = tap_duration(k_uptime_get());
        if (duration > 500) {
            LOG_DBG("TAP Duration: %llu ms", duration);
			report[REPORT_IDX] = MOUSE_REPORT_IDX;
            report[BTN_REPORT_IDX] = MOUSE_BTN_RIGHT;
        }
		return ret;
    }

	return ret;
}

int usb_dev_write(const struct device *hid) {
	int ret;

	/* Clear report buffer*/
	memset(report, 0, sizeof(report));

	ret = mouse_fn();

	ret = kb_fn();

	ret = hid_device_submit_report(hid, REPORT_COUNT, report);
	if (ret < 0) {
		LOG_ERR("HID submit report error, %d", ret);
	}

	return ret;
}

int usb_dev_init(const struct device *hid)
{
	int ret;
	
	if (!device_is_ready(hid)) {
		LOG_ERR("Device %s is not ready", hid->name);
		return -ENODEV;
	}

	ret = hid_device_register(hid,
				  hid_report_desc, sizeof(hid_report_desc),
				  &hid_ops);
	if (ret != 0) {
		LOG_ERR("Failed to register HID Device, %d", ret);
		return ret;
	}

	ret = enable_usb_device_next();
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return -ENODEV;
	}

	return 0;
}