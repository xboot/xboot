# 全志V3S，荔枝派(lichee-pi zero)

***

# 编译XBOOT,目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-licheepi
```

# 烧写到RAM中并运行
```shell
sunxi-fel spl xboot.bin; sunxi-fel -p write 0x40000000 xboot.bin; sunxi-fel exec 0x40000000;
```

# 烧写到SPI Flash
```shell
sunxi-fel -p spiflash-write 0 xboot.bin
```

# 关于sunxi-fel工具

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