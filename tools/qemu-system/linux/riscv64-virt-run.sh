#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# Run qemu
exec qemu-system-riscv64 -M virt -m 512M -name "RiscV64 Virtual Machine" -show-cursor -rtc base=localtime -serial stdio -kernel ${QEMU_DIR}/../../../output/xboot
