@echo off

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M realview-eb -name "ARM RealView Emulation Baseboard" -show-cursor -localtime -serial vc -sd "%USERPROFILE%\.xboot\sdcard.img" -kernel ..\..\..\output\xboot.elf
