# SAMSUNG EXYNOS4412 <!-- {docsify-ignore} -->

## 编译源码
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-exynos4412
```

## 烧写到SD卡
```shell
sudo dd if=xboot.bin of=/dev/sdb bs=512 seek=1 conv=sync
```

## 运行
配置SD卡启动模式，插入SD卡，上电，串口3显示Debug信息。
