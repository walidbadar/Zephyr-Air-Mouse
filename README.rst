.. _zephyr_air_mouse:

Zephyr Air Mouse
#########

.. figure:: https://drive.usercontent.google.com/download?id=1usPkM_bNB7gP4r-wur2owl-Ud5mZ43VU&export=download&authuser=0&confirm=t&uuid=3e656ce2-1ea7-4254-a8d3-78e9152e911b&at=AEz70l4f4skaLFgPF0CgSEzE6otP:1742583658543

Overview
********

This sample application implements an air mouse using the Zephyr RTOS. It utilizes the Zephyr USB subsystem to create a standard USB HID mouse device and the Sensor API to interface with a LIS2DH accelerometer sensor. The air mouse translates physical movement of the device into cursor movement on a connected computer.

Features
********

* USB HID mouse device implementation
* LIS2DH accelerometer-based motion detection and translation
* Plug-and-play compatibility with most operating systems
* Low latency motion-to-cursor translation
* Button input support for mouse clicks

Hardware Requirements
********************

* STM32F3 Discovery board featuring:

  * STM32F303VC microcontroller
  * USB device capability
  * LIS2DH accelerometer sensor
  * User programmable buttons

Software Requirements
********************

* Zephyr RTOS (version 3.4.0 or newer)
* West build tools
* LIS2DH device driver

Building and Running
*******************

This sample can be built for the STM32F3 Discovery board.

.. zephyr-app-commands::
   :zephyr-app: samples/application_development/air_mouse
   :board: stm32f3_disco
   :goals: build flash
   :compact:

Usage
*****

1. Connect your STM32F3 Discovery board to a computer via USB
2. The device should be recognized as a standard USB mouse
3. Move the board in the air to control the cursor
4. Use the user buttons on the board for mouse clicks (configured in the device tree).

Implementation Details
*********************

The application uses the Zephyr sensor API to read acceleration data from the LIS2DH sensor. This data is processed to filter out noise and calculate cursor movement. The USB HID class is then used to send mouse reports to the connected computer.

Key components:

* LIS2DH accelerometer data acquisition
* Motion processing algorithm
* USB HID mouse report generation
* Button input handling

Troubleshooting
**************

* If the mouse cursor doesn't move, check that the LIS2DH sensor is properly initialized
* Verify that the USB connection is properly established
* Check the system logs for any error messages
* If cursor movement is erratic, try adjusting the sensitivity settings in the code

References
**********

* `LIS2DH Datasheet <https://www.st.com/resource/en/datasheet/lis2dh.pdf>`_
* `Zephyr USB HID Documentation <https://docs.zephyrproject.org/latest/reference/usb/hid.html>`_
* `Zephyr Sensor API Documentation <https://docs.zephyrproject.org/latest/reference/peripherals/sensor.html>`_