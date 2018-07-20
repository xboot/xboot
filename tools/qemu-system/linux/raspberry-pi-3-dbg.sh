#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# Run qemu
exec qemu-system-aarch64 -M raspi3 -m 1024M -name "Raspberry Pi 3" -S -gdb tcp::10000,ipv4 -show-cursor -rtc base=localtime -serial stdio -kernel ${QEMU_DIR}/../../../output/xboot
