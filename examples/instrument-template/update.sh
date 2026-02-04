#!/usr/bin/sh

smpmgr --loglevel INFO --port /dev/ttyACM1 upgrade build/rp2350_eth/rp2350a/m33/mcuboot/app/app/zephyr/zephyr.signed.bin
smpmgr --port /dev/ttyACM1 shell "mcuboot request_upgrade"
smpmgr --port /dev/ttyACM1 shell "kernel reboot"
