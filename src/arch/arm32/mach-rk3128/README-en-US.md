# Rockchip RK3128

***

## Compile source code and generate the target files at the  output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rk3128
```

## Enter maskrom mode and using xrock for burning image
```shell
sudo xrock maskrom rk3128_ddr_300MHz_v2.12.bin rk3128_usbplug_v2.63.bin;
sleep 3;
sudo xrock flash write 64 xbootpak.bin;
sudo xrock reset;
```

## Download xrock source code
```shell
git clone https://github.com/xboot/xrock.git
```

## Make and install xrock
```shell
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
