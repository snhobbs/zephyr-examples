# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(console)

# --------------------------------------------------
# Generate prj.conf using Makefile at configure time
# --------------------------------------------------

set(GENCONF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/confs)
set(PRJ_CONF ${CMAKE_CURRENT_SOURCE_DIR}/prj.conf)

execute_process(
		COMMAND make -f ${GENCONF_DIR}/Makefile ${GENCONF_DIR}/prj.conf
		WORKING_DIRECTORY ${GENCONF_DIR}
		RESULT_VARIABLE GENCONF_RESULT)

if(NOT GENCONF_RESULT EQUAL 0)
	message(FATAL_ERROR "Failed to generate prj.conf")
endif()

file(GLOB app_sources src/main.cpp src/led.c)
target_sources(app PRIVATE ${app_sources})


target_compile_options(app PRIVATE
    -Wall           # All common warnings
    -Wextra         # Extra warnings
    -Wpedantic      # Strict ISO compliance
    -Wconversion    # Implicit conversions
    -Wshadow        # Variable shadowing
    -Wnon-virtual-dtor # Warn if class has virtual methods but no virtual destructor
	#-Wold-style-cast   # Old-style casts (C-style)
)
