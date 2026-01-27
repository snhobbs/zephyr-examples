# Instrument Demo 
Uses the T2 template with a workspace setup script. 

Uses a confs directory that compiles the proj.conf to keep the config separate and composable.

## Features 
+ console connected to USB AMA or UART running scpi-parser
+ shell connected to USB AMA or UART

Running both the shell and the console is preferred. The shell should include a 'help' function that can hold the SCPI programming manual for command discovery.

+ Uses a conf directory with a Makefile to combine subsections into proj.conf.
+ Include littlefs for internal flash or fatfs for external storage. Use the external storage for firmware updates and data storage.
	+ settings are stored in the filesystem
+ MCUBoot first stage bootloader
	+ Use MCUMgr over uart for image update and settings
	+ Boot entry button and reboot delay for DFU entry 
	+ Reboot/reset command
	+ NOTE: In the RP2350 calls to serial_recovery starts the UART/USB bootloader.
+ Use settings subsystem for configuration options
+ bindesc is enabled with the kernel, build data, and app version data available. `west bindesc dump`
+ 
