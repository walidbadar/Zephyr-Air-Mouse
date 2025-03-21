#
# Copyright (c) 2025, Muhammad Waleed Badar
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(air_mouse)

target_include_directories(app PRIVATE app/inc)
include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

FILE(GLOB app_sources 
     app/*.c 
     app/src/*.c)
target_sources(app PRIVATE ${app_sources})