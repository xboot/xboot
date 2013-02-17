#!/bin/sh
#

# the qemu-system-arm's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

if [ ! -e ~/.xboot/sdcard.img ]; then
	mkdir -p ~/.xboot;
	unzip ${QEMU_DIR}/sdcard.zip -d ~/.xboot > /dev/null;
fi

# run qemu-system-arm
exec qemu-system-arm -M realview-eb -name "ARM RealView Emulation Baseboard" -show-cursor -localtime -serial stdio -sd ~/.xboot/sdcard.img -kernel ${QEMU_DIR}/../../../output/xboot.elf
