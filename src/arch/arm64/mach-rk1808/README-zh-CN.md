# 瑞芯微RK1808

***

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
