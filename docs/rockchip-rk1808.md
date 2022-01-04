# 瑞芯微RK1808 <!-- {docsify-ignore} -->

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/aarch64-none-elf- PLATFORM=arm64-rk1808
```

## 进入maskrom模式，烧写镜像到emmc
```shell
sudo rkdeveloptool db xbootpak.bin;
sleep 3;
sudo rkdeveloptool ul xbootpak.bin;
sudo rkdeveloptool rd;
```

## 关于rkdeveloptool工具

rkdeveloptool工具是瑞芯微提供的开源版maskrom烧录工具，支持全系列芯片

## 下载rkdeveloptool源码
```shell
git clone https://github.com/rockchip-linux/rkdeveloptool.git
```

## 编译安装rkdeveloptool
```shell
autoreconf -i
./configure
make
sudo make install
```
