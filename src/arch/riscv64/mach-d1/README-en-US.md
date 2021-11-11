# Allwinner D1(Nezha)

***

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/riscv64-unknown-elf- PLATFORM=riscv64-d1
```

## Program to RAM and execute
```shell
sudo xfel ddr ddr3; sudo xfel write 0x40000000 xboot.bin; sudo xfel exec 0x40000000;
```

## Burn normal image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin
```

## Burn compress image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin.z
```

## Download riscv toolchain
```shell
http://pan.baidu.com/s/1dDtssIt

```
```
https://occ.t-head.cn/
riscv64-linux-x86_64-20210512.tar.gz
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

# Navigation Pages

* Xboot official home page

  https://xboot.github.io/

* XBOOT source code(completely open source) is registered in `github`. Please `star` and `fork` a copy directly. If you want to merge it, just sent `pull request`.

  https://github.com/xboot/xboot

* XBOOT show case pages，In XBOOT show case page,you can have an intuitive feel.

  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md

* XBOOT system development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md

* XBOOT application development guide

  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md

* XBOOT official tencent qq group with many other participants. You are welcome to join us.

  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000 peoples)

* You can create an isuse for us us if you have any question.
