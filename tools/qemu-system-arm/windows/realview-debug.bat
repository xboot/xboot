@echo off

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M realview-pb-a8 -name "ARM RealView Platform Baseboard for Cortex-A8" -S -gdb tcp::1234,ipv4 -show-cursor -localtime -serial vc -sd "%USERPROFILE%\.xboot\sdcard.img" -kernel ..\..\..\output\xboot.elf
