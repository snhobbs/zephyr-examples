# Instrument Demo 
Uses the T2 template with a workspace setup script. 

Uses a confs directory that compiles the proj.conf to keep the config separate and composable.

## Features 
+ console connected to USB AMA or UART running scpi-parser
+ shell connected to USB AMA or UART

Running both the shell and the console is preferred. The shell should include a 'help' function that can hold the SCPI programming manual for command discovery.

+ Uses a conf directory with a Makefile to combine subsections into proj.conf.
+ Include littlefs for internal flash or fatfs for external storage. Use the external storage for firmware updates and data storage.
+ MCUBoot first stage bootloader
	+ Update in place over serial or UF2

