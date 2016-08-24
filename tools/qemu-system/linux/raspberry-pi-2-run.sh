#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# Run qemu
exec qemu-system-arm -M raspi2 -m 512M -name "Raspberry Pi 2" -show-cursor -localtime -serial stdio -kernel ${QEMU_DIR}/../../../output/xboot
