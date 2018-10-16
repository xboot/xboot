@echo off
@set SDL_STDIO_REDIRECT=0

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M realview-pb-a8 -m 256M -name "ARM RealView Platform Baseboard for Cortex-A8" -show-cursor -rtc base=localtime -serial stdio -sd "%USERPROFILE%\.xboot\sdcard.img" -net nic,macaddr=88:88:88:11:22:33,model=lan9118 -net user -kernel ..\..\..\output\xboot
