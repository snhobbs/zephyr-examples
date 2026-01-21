# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(shell_module)

file(GLOB app_sources src/main.cpp src/scpi.cpp)
# file(GLOB app_sources src/main.cpp src/led.c) # src/*.cpp)
#file(GLOB scpi_sources
#					${CMAKE_CURRENT_SOURCE_DIR}/../deps/scpi-parser/libscpi/src/*.c)
target_sources(app PRIVATE ${app_sources})# ${scpi_sources})
#target_include_directories(
#		app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../deps/scpi-parser/libscpi/inc)



target_compile_options(app PRIVATE
    -Wall           # All common warnings
    -Wextra         # Extra warnings
    -Wpedantic      # Strict ISO compliance
    -Wconversion    # Implicit conversions
    -Wshadow        # Variable shadowing
    -Wnon-virtual-dtor # Warn if class has virtual methods but no virtual destructor
	#-Wold-style-cast   # Old-style casts (C-style)
)
