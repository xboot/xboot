# SAMSUNG EXYNOS4412

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-exynos4412
```

## Burn to SDCARD
```shell
sudo dd if=xboot.bin of=/dev/sdb bs=512 seek=1 conv=sync
```

## Running
Config OM pin to boot from sdcard mode, insert sdcard, and then power on. The uart3 will showing some debug messages.

