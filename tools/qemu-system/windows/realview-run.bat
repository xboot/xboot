@echo off
@set SDL_STDIO_REDIRECT=0

if exist "%USERPROFILE%\.xboot\sdcard.img" goto RunQemu
    mkdir "%USERPROFILE%\.xboot"
    unzip.exe sdcard.zip -d "%USERPROFILE%\.xboot"

:RunQemu
qemu-system-arm.exe -M realview-pbx-a9 -m 256M -smp 4 -name "ARM RealView Platform Baseboard Explore for Cortex-A9" -rtc base=localtime -serial stdio -sd "%USERPROFILE%\.xboot\sdcard.img" -net nic,netdev=net0,macaddr=88:88:88:11:22:33,model=lan9118 -netdev user,id=net0,hostfwd=tcp::8080-:80 -kernel ..\..\..\output\xboot
