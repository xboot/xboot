#!/bin/sh
#

# The script root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

# running ...
exec qemu-system-aarch64 -machine virt -cpu cortex-a57 -m 4096 -smp 1 -name "ARM Virtual Machine For Cortex-A57" -show-cursor -localtime -serial stdio -kernel ${QEMU_DIR}/../../output/xboot
