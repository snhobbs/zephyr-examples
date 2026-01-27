# Instrument Demo 
Uses the T2 template with a workspace setup script. 


## Features 
### Interfaces
	+ Shell: shell subsystem
		+ By default this is connected with USB AMA
		+ Can be connected out of the box with bluetooth, TCP (MQTT, telnet, websocket), serial (UART or USB AMA), or RPMSG
	+ SCPI: console subsystem 
		+ Can use any driver that presents itself as a uart. Out of the box this includes bluetooth, USB AMA, or a physical UART.

For SCPI devices connected over USB the shell is presented to help with command discovery. The shell should include a 'help' function that can hold the SCPI programming manual for command discovery.

## Settings 
Uses littlefs by default for settings. Settings are added with a macro call that registers the option. These are exposed to the shell interface and selectively over SCPI.

## Device Firmware Updates
+ MCUBoot first stage bootloader
+ Protected shell command for entering UF2 bootloader on RP2XXX devices 
+ Button for entering UF2 bootloader 
+ Shell and SCPI command for entering SMP image uploader 

## Binary Data 
bindesc is enabled with the kernel, build data, and app version data available. `west bindesc dump`
This keeps the build information with the image.

## Workspace Setup Script 
Although not complicated I strongly prefer to have workspace settings backed up in my repo. Setup a new workspace or update the settings with app/setup.sh.

## TODO 
+ Add firmware image streaming over uart.
+ Add an auto generated programming manual for the SCPI available with a shell command.
+ Add a central settings parser and a MACRO to declare entries
+ Add shell and console DFU


