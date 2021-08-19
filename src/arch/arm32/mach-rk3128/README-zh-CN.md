# 瑞芯微RK3128 

***

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rk3128
```

## 进入maskrom模式，利用xrock烧写镜像
```shell
sudo xrock maskrom rk3128_ddr_300MHz_v2.12.bin rk3128_usbplug_v2.63.bin
sudo xrock flash write 64 xbootidb.bin;
sudo xrock reset;
```

## 进入maskrom模式，利用rkdeveloptool烧写镜像
```shell
sudo rkdeveloptool db xbootpak.bin;
sleep 3;
sudo rkdeveloptool ul xbootpak.bin;
sudo rkdeveloptool rd;
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

# 相关网址

- XBOOT官方首页
  https://xboot.github.io/

- XBOOT源码(完全开源)，寄存在github，请直接`fork`一份代码，如需申请合并，直接`pull request`即可
  https://github.com/xboot/xboot

- XBOOT运行效果展示页面，首次接触的用户，建议观看，对XBOOT有个直观的感受
  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-zh-CN.md

- XBOOT系统开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-zh-CN.md

- XBOOT应用开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-zh-CN.md

- XBOOT官方QQ群，大佬聚集，请踊跃加入
  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000人)
