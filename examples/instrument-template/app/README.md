# Instrument Demo

Uses the T2 template with a workspace setup script.

## Zephyr Features

- bindesc is enabled with the kernel, build data, and app version data available. Use `west bindesc dump` to read them. These values can be made available to the application.
- MCUBoot bootloader with two slots
- MCUMGR for updating firmware images. The shell transport layer is used so this is available over TCP, bluetooth, USB, and serial as needed.
- SCPI over console. This is likely to be changed preferring dedicated SCPI interfaces for different phys as needed. If using a connection that can have multiple clients then this will need a context for each possible user or use dynamic allocation.
- Shell with MCUMGR
  - The shell should help with command discovery for the SCPI interface. The shell should include a 'help' function that can hold the SCPI programming manual for command discovery.
- Settings subsystem on littlefs.
- Telnet shell
- TCP socket server. This is intended to be used with SCPI. The RP2350 example uses ethernet over USB.
  - SCPI socket on standard port 5025

## Workspace Setup Script

Although not complicated I strongly prefer to have workspace settings backed up in my repo. Setup a new workspace or update the settings with app/setup.sh.

## Multi-Processing

For most instruments that need to run servers we will want multiple cores or processors.
In the case of multi-core MCUs like a cortex-m use one for the servers and IO and one for data taking and real time processes.

## Similar Projects

- <https://github.com/ehunck/lxi-zephyr-demo>

## TODO

- Add an auto generated programming manual for the SCPI available with a shell command.
- Add a central settings parser and a MACRO to declare entries
- Register services with mDNS

## Setup

- Use the shell to set the device options like setting the IP address and credentials for the servers.
- Expose a limited shell with a socket or over ssh.
- Expose the SCPI console over a socket
- Potentially expose an HTTP server. All of the servers should be capable of being disabled using the USB interface.

- For development the ethernet is connected through the USB with the different servers connected.

There are VXI-11 libraries available which would make the device discoverable by visa.

- <https://hpwiki.mcguirescientificservices.com/_media/application_notes:an-1465-20.pdf>
- <https://github.com/lxi-tools/liblxi>
