# 全志T113-S3 <!-- {docsify-ignore} -->

## 编译源码，生成的目标文件在output目录下
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-t113s3
```

## 烧写到RAM中并运行
```shell
sudo xfel ddr t113-s3; sudo xfel write 0x40000000 xboot.bin; sudo xfel exec 0x40000000;
```

## 烧写普通镜像到SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin
```

## 烧写压缩镜像到SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin.z
```

## 烧写普通镜像到SPI Nand Flash
```shell
sudo xfel spinand splwrite 2048 1048576 xboot.bin
```

## 烧写压缩镜像到SPI Nand Flash
```shell
sudo xfel spinand splwrite 2048 1048576 xboot.bin.z
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
