# 全志F1C200S <!-- {docsify-ignore} -->

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-eabi- PLATFORM=arm32-f1c200s
```

## 烧写到RAM中并运行
```shell
sudo xfel ddr; sudo xfel write 0x80000000 xboot.bin; sudo xfel exec 0x80000000;
```

## 烧写普通镜像到SPI Flash
```shell
sudo xfel spinor write 0 xboot.bin
```

## 烧写压缩镜像到SPI Flash
```shell
sudo xfel spinor write 0 xboot.bin.z
```

## 下载xfel工具源码
```shell
git clone https://github.com/xboot/xfel.git
```

## 编译安装xfel工具
```shell
make
sudo make install
```

