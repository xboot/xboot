@echo off
@set SDL_STDIO_REDIRECT=0

:RunQemu
qemu-system-arm.exe -M raspi2 -m 512M -global bcm2835-fb.pixo=0 -name "Raspberry Pi 2" -show-cursor -rtc base=localtime -serial stdio -kernel ..\..\..\output\xboot
