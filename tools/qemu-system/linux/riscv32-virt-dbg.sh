#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# Run qemu
exec qemu-system-riscv32 -M virt -m 512M -name "RiscV32 Virtual Machine" -S -gdb tcp::10000,ipv4 -show-cursor -rtc base=localtime -serial stdio -kernel ${QEMU_DIR}/../../../output/xboot
