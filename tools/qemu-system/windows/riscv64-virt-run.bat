@echo off
@set SDL_STDIO_REDIRECT=0

:RunQemu
@qemu-system-riscv64.exe -M virt -m 512M -smp 4 -name "RiscV64 Virtual Machine" -show-cursor -rtc base=localtime -serial stdio -kernel ..\..\..\output\xboot
