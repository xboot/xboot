@echo off
if {%1}=={} goto :USAGE
if {%2}=={} goto :USAGE
if not exist "%2" @Echo The file "%2" not exist & goto :USAGE

dd if=%2 of=%TEMP%/xboot.bin bs=512 conv=sync
ddwin if=%TEMP%/xboot.bin of=%1 bs=512 seek=1 --progress
@echo The image is fused successfully
goto :EOF

:USAGE
@echo Usage: s5pv210-irom-sd.bat device bootloader
@echo        device     = disk device name for SD card
@echo        bootloader = \path\to\xboot.bin
@echo e.g. : s5pv210-irom-sd.bat \\?\Device\Harddisk1\Partition0 d:\xboot.bin
@echo.
@echo.
@echo list all of available device:
ddwin --list
goto :EOF
