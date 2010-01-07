#!/bin/sh
#

# the qemu-system-arm's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# run qemu-system-arm
exec qemu-system-arm -M realview -name "realview" -localtime -serial vc -kernel ${QEMU_DIR}/../../../output/xboot.elf