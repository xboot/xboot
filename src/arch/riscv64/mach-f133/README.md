# Allwinner F133

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/riscv64-unknown-elf- PLATFORM=riscv64-f133
```

## Program to RAM and execute
```shell
sudo xfel ddr ddr2; sudo xfel write 0x40000000 xboot.bin; sudo xfel exec 0x40000000;
```

## Burn normal image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin
```

## Burn compress image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin.z
```

## Burn normal image to SPI Nand Flash
```shell
sudo xfel spinand splwrite 2048 1048576 xboot.bin
```

## Burn compress image to SPI Nand Flash
```shell
sudo xfel spinand splwrite 2048 1048576 xboot.bin.z
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

