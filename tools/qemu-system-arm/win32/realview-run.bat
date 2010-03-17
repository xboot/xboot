@echo off

if exist sdcard.img goto RunQemu
    unzip.exe sdcard.zip

:RunQemu
qemu-system-arm.exe -M realview -name "realview" -localtime -serial vc -sd sdcard.img -kernel ..\..\..\output\xboot.elf
