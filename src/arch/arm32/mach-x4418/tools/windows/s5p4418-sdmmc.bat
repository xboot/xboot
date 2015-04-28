@echo off
if {%1}=={} goto :USAGE
if {%2}=={} goto :USAGE
if not exist "%2" @Echo The file "%2" not exist & goto :USAGE

dd if=%2 of=%TEMP%/xbootpak.bin bs=512 conv=sync
ddwin if=%TEMP%/xbootpak.bin of=%1 bs=512 seek=1 --progress
@echo The image is fused successfully
goto :EOF

:USAGE
@echo Usage: exynos4412-irom-sd.bat device bootloader
@echo        device     = disk device name for SD card
@echo        bootloader = \path\to\xbootpak.bin
@echo e.g. : exynos4x12-irom-sd.bat \\?\Device\Harddisk1\Partition0 d:\xbootpak.bin
@echo.
@echo.
@echo list all of available device:
ddwin --list
goto :EOF
