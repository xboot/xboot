These files must be copied along with the generated image onto a SD card with FAT file system

wget -q -O bootcode.bin https://github.com/raspberrypi/firmware/blob/master/boot/bootcode.bin?raw=true
wget -q -O fixup.dat https://github.com/raspberrypi/firmware/blob/master/boot/fixup.dat?raw=true
wget -q -O start.elf https://github.com/raspberrypi/firmware/blob/master/boot/start.elf?raw=true
