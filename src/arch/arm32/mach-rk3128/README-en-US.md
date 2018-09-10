# Rockchip RK3128

***

## Compile source code and generate the target files at the  output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-rk3128
```

## Enter maskrom mode and burn image to emmc
```shell
sudo rkdeveloptool db xbootpak.bin;
sleep 3;
sudo rkdeveloptool ul xbootpak.bin;
sudo rkdeveloptool rd;
```

## About rkdeveloptool tool

The default rkdeveloptool which is open sourced and used for burning images in the maskrom mode, and support all chips by rockchip

## Download rkdeveloptool source code
```shell
git clone https://github.com/rockchip-linux/rkdeveloptool.git
```

## Make and install rkdeveloptool
```shell
autoreconf -i
./configure
make
sudo make install
```

# Navigation Pages

* Xboot official home page

  https://xboot.github.io/

* XBOOT source code(open source completely),registered in `github`,please `star` and `fork` a copy directly,if you want to merge it, just sent `pull request`

  https://github.com/xboot/xboot

* XBOOT show case pages，In XBOOT show case page,you can have an intuitive feel.

  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md

* XBOOT system development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md

* XBOOT application development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md

* XBOOT official tencent qq group,many big brother, welcome to join us.

  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000 peoples)

* You can isuse us if you have any question.
