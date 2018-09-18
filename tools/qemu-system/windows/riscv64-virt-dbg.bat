@echo off
@set SDL_STDIO_REDIRECT=0

:RunQemu
@qemu-system-riscv64.exe -M virt -m 512M -name "RiscV64 Virtual Machine" -S -gdb tcp::10000,ipv4 -show-cursor -rtc base=localtime -serial vc -kernel ..\..\..\output\xboot
