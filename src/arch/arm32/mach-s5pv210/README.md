# SAMSUNG S5PV210

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-s5pv210
```

## Burn to SDCARD
```shell
sudo dd if=xboot.bin of=/dev/sdb bs=512 seek=1 conv=sync
```

## Running
Config OM pin to boot from sdcard mode, Insert sdcard then power on, The uart2 will showing some debug message.

