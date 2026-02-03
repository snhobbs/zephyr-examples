# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(shell_module)

zephyr_library_sources(src/main.cpp)
zephyr_library_sources_ifdef(CONFIG_APP_BLINKY src/led.c)
zephyr_library_sources_ifdef(CONFIG_APP_SCPI src/scpi.cpp)
zephyr_library_sources_ifdef(CONFIG_SETTINGS src/settings.c)
zephyr_library_sources_ifdef(CONFIG_APP_ECHO_SERVER src/console_server.c)
target_sources(app PRIVATE ${app_sources}) # ${scpi_sources})

target_compile_options(
		app
		PRIVATE -Wall # All common warnings
										-Wextra # Extra warnings
										-Wpedantic # Strict ISO compliance
										-Wconversion # Implicit conversions
										-Wshadow # Variable shadowing
										-Wnon-virtual-dtor # Warn if class has virtual methods but no virtual
										# destructor
		# -Wold-style-cast   # Old-style casts (C-style)
)
