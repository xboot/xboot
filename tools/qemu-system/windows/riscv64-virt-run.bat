@echo off
@set SDL_STDIO_REDIRECT=0

:RunQemu
@qemu-system-riscv64.exe -M virt -m 512M -name "RiscV64 Virt Machine" -show-cursor -rtc base=localtime -serial vc -kernel ..\..\..\output\xboot