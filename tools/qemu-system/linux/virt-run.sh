#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

if [ ! -e ~/.xboot/sdcard.img ]; then
	mkdir -p ~/.xboot;
	unzip ${QEMU_DIR}/sdcard.zip -d ~/.xboot > /dev/null;
fi

# Run qemu
exec qemu-system-aarch64 -machine virt -cpu cortex-a57 -m 256M -name "ARM Virtual Machine For Cortex-A57" -show-cursor -localtime -serial stdio -sd ~/.xboot/sdcard.img -kernel ${QEMU_DIR}/../../../output/xboot
