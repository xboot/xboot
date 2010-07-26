@echo off

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M realview -name "ARM RealView Emulation Baseboard" -localtime -serial vc -sd "%USERPROFILE%\.xboot\sdcard.img" -kernel ..\..\..\output\xboot.elf
