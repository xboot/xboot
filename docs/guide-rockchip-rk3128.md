# 瑞芯微RK3128 <!-- {docsify-ignore} -->

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rk3128
```

## 进入maskrom模式，利用xrock烧写镜像
```shell
sudo xrock maskrom rk3128_ddr_300MHz_v2.12.bin rk3128_usbplug_v2.63.bin;
sleep 3;
sudo xrock flash write 64 xbootpak.bin;
sudo xrock reset;
```

## 下载xrock工具源码
```shell
git clone https://github.com/xboot/xrock.git
```

## 编译安装xrock工具
```shell
make
sudo make install
```

