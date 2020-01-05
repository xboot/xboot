# SAMSUNG EXYNOS4412

***

## Compile source code
```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-exynos4412
```

## Burn to SDCARD
```shell
sudo dd if=xboot.bin of=/dev/sdb bs=512 seek=1 conv=sync
```

## Running
Config OM pin to boot from sdcard mode, insert sdcard, and then power on. The uart3 will showing some debug messages.

# Navigation Pages

* Xboot official home page

  https://xboot.github.io/

* XBOOT source code (completely open source), registered in `github`, please `star` and `fork` a copy directly, if you want to merge it, just sent a `pull request`

  https://github.com/xboot/xboot

* XBOOT show case pages. In XBOOT show case page you can have an intuitive feel.

  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md

* XBOOT system development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md

* XBOOT application development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md

* XBOOT official tencent qq group, many big brother, welcome to join us.

  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000 peoples)

* You can create an `issue` if you have any question.
