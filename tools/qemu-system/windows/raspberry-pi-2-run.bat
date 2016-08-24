@echo off

:RunQemu
qemu-system-arm.exe -M raspi2 -m 512M -name "Raspberry Pi 2" -show-cursor -localtime -serial vc -kernel ..\..\..\output\xboot
