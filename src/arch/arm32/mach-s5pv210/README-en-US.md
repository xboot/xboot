# SAMSUNG S5PV210

***

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

# Navigation Pages

* Xboot official home page

  https://xboot.github.io/

* XBOOT source code(completely open source) is registered in `github`. Please `star` and `fork` a copy directly. If you want to merge it, just send a `pull request`.

  https://github.com/xboot/xboot

* XBOOT show case pages，In XBOOT show case page,you can have an intuitive feel.

  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md

* XBOOT system development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md

* XBOOT application development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md

* XBOOT official tencent qq group with many other participants. You are welcome to join us.

  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000 peoples)

* You can create an isuse for us if you have any question.
