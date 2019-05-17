# 全志V3S，中科世为(SuperView Z11s)

***

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-z11s
```

## 烧写到RAM中并运行
```shell
sunxi-fel spl xboot.bin; sunxi-fel -p write 0x40000000 xboot.bin; sunxi-fel exec 0x40000000;
```

## 烧写普通镜像到SPI Flash
```shell
sunxi-fel -p spiflash-write 0 xboot.bin
```

## 关于sunxi-fel工具

sunxi-fel工具默认并没有集成spi flash写入功能，这里需要自行下载源码并编译，以支持spi flash烧写功能

## 下载sunxi-fel源码
```shell
git clone https://github.com/Icenowy/sunxi-tools.git
git checkout -b spi-rebase origin/spi-rebase
```

## 编译安装sunxi-fel
```shell
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
