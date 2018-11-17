@echo off
@set SDL_STDIO_REDIRECT=0

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M vexpress-a9 -smp 1 -m 256M -name "ARM Versatile Express for Cortex-A9" -show-cursor -rtc base=localtime -serial stdio -sd "%USERPROFILE%\.xboot\sdcard.img" -kernel ..\..\..\output\xboot
