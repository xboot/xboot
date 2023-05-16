# Allwinner R128

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/riscv64-unknown-elf- PLATFORM=riscv64-r128
```

## Program to RAM and execute
```shell
xfel ddr r128-s2;
xfel write 0x08004000 lichee/rtos/build/r128s2_pro_m33/img/rt_system.bin;
xfel write 0x08200000 xboot.bin;
xfel exec 0x08004001;
```

## Download riscv toolchain
```shell
http://pan.baidu.com/s/1dDtssIt

```
```
https://occ.t-head.cn/
Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4-20211227.tar.gz
```

## Download xfel tool's source code
```shell
git clone https://github.com/xboot/xfel.git
```

## Make and install xfel tool
```shell
make
sudo make install
```

