#!/bin/sh
#

# The script root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# running ...
exec qemu-system-arm -machine virt -cpu cortex-a15 -m 256 -smp 1 -name "ARM Virtual Machine For Cortex-A15" -show-cursor -localtime -serial stdio -kernel ${QEMU_DIR}/../../output/xboot -gdb tcp::10000 -S
