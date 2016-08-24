@echo off

:RunQemu
qemu-system-arm.exe -M raspi2 -m 512M -name "Raspberry Pi 2" -S -gdb tcp::10000,ipv4 -show-cursor -localtime -serial vc -kernel ..\..\..\output\xboot
