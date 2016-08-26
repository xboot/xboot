@echo off
@set SDL_STDIO_REDIRECT=0

:RunQemu
qemu-system-arm.exe -M raspi2 -m 512M -name "Raspberry Pi 2" -show-cursor -localtime -serial stdio -kernel ..\..\..\output\xboot
