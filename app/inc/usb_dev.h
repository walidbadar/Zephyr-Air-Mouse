#ifndef __USB_DEV_H__
#define __USB_DEV_H__

#include <sample_usbd.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>

#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_hid.h>

#define KB_REPORT_IDX 		1
#define MOUSE_REPORT_IDX 	2
#define CONSUMER_REPORT_IDX 3

#define MOUSE_BTN_RIGHT  0x01
#define MOUSE_BTN_LEFT   0x02

#define MOUSE_SENSITIVITY_X  0x01
#define MOUSE_SENSITIVITY_Y  0x01

#define HID_CUSTOM_REPORT_DESC() {				\
	HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),			\
	HID_USAGE(HID_USAGE_GEN_DESKTOP_KEYBOARD),		\
	HID_COLLECTION(HID_COLLECTION_APPLICATION),		\
		HID_REPORT_ID(KB_REPORT_IDX),				        \
		HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP_KEYPAD),	\
		/* HID_USAGE_MIN8(Reserved) */			\
		HID_USAGE_MIN8(0),				\
		/* HID_USAGE_MAX8(Keyboard Application) */	\
		HID_USAGE_MAX8(101),				\
		HID_LOGICAL_MIN8(0),				\
		HID_LOGICAL_MAX8(101),				\
		HID_REPORT_SIZE(8),				\
		HID_REPORT_COUNT(1),				\
		/* HID_INPUT (Data,Ary,Abs) */			\
		HID_INPUT(0x00),				\
	HID_END_COLLECTION,					\
	HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),			\
	HID_USAGE(HID_USAGE_GEN_DESKTOP_MOUSE),			\
	HID_COLLECTION(HID_COLLECTION_APPLICATION),		\
		HID_REPORT_ID(MOUSE_REPORT_IDX),				        \
		HID_USAGE(HID_USAGE_GEN_DESKTOP_POINTER),	\
		HID_COLLECTION(HID_COLLECTION_PHYSICAL),	\
			/* Bits used for button signalling */	\
			HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON),	\
			HID_USAGE_MIN8(1),			\
			HID_USAGE_MAX8(2),			\
			HID_LOGICAL_MIN8(0),			\
			HID_LOGICAL_MAX8(1),			\
			HID_REPORT_SIZE(1),			\
			HID_REPORT_COUNT(2),			\
			/* HID_INPUT (Data,Var,Abs) */		\
			HID_INPUT(0x02),			\
			/* Unused bits */			\
			HID_REPORT_SIZE(8 - 2),		\
			HID_REPORT_COUNT(1),			\
			/* HID_INPUT (Cnst,Ary,Abs) */		\
			HID_INPUT(1),				\
			/* X and Y axis, scroll */		\
			HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),	\
			HID_USAGE(HID_USAGE_GEN_DESKTOP_X),	\
			HID_USAGE(HID_USAGE_GEN_DESKTOP_Y),	\
			HID_USAGE(HID_USAGE_GEN_DESKTOP_WHEEL),	\
			HID_LOGICAL_MIN8(-127),			\
			HID_LOGICAL_MAX8(127),			\
			HID_REPORT_SIZE(8),			\
			HID_REPORT_COUNT(3),			\
			/* HID_INPUT (Data,Var,Rel) */		\
			HID_INPUT(0x06),			\
		HID_END_COLLECTION,				\
	HID_END_COLLECTION,					\
	HID_USAGE_PAGE(0x0C),                 \
    HID_USAGE(0x01),              \
    HID_COLLECTION(HID_COLLECTION_APPLICATION),         \
		HID_REPORT_ID(CONSUMER_REPORT_IDX),				        \
        HID_LOGICAL_MIN8(0),                            \
        HID_LOGICAL_MAX16(0x3c, 0x04),                  \
        HID_USAGE_MIN8(0),                              \
        HID_USAGE_MAX16(0x3c, 0x04),                    \
        HID_REPORT_COUNT(1),                            \
        HID_REPORT_SIZE(16),                            \
        HID_INPUT(0x00),                                \
    HID_END_COLLECTION,                                 \
}

enum report_idx {
	REPORT_IDX = 0,
	BTN_REPORT_IDX = 1,
	X_REPORT_IDX = 2,
	Y_REPORT_IDX = 3,
	WHEEL_REPORT_IDX = 4,
	REPORT_COUNT = 5,
};

int usb_dev_init(const struct device *hid);
int usb_dev_write(const struct device *hid);

#endif
