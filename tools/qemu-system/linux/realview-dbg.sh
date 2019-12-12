#!/bin/sh
#

# The qemu's root directory.
export QEMU_DIR=$(cd `dirname $0` ; pwd)

if [ ! -e ~/.xboot/sdcard.img ]; then
	mkdir -p ~/.xboot;
	unzip ${QEMU_DIR}/sdcard.zip -d ~/.xboot > /dev/null;
fi

# Run qemu
exec qemu-system-arm -M realview-pbx-a9 -m 256M -smp 4 -name "ARM RealView Platform Baseboard Explore for Cortex-A9" -S -gdb tcp::10000,ipv4 -show-cursor -rtc base=localtime -serial stdio -sd ~/.xboot/sdcard.img -net nic,macaddr=88:88:88:11:22:33,model=lan9118 -net user -kernel ${QEMU_DIR}/../../../output/xboot
