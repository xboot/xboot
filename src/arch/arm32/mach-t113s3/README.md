# Allwinner T113-S3

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-t113s3
```

## Program to RAM and execute
```shell
sudo xfel ddr t113-s3; sudo xfel write 0x40000000 xboot.bin; sudo xfel exec 0x40000000;
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

## Download xfel tool's source code
```shell
git clone https://github.com/xboot/xfel.git
```

## Make and install xfel tool
```shell
make
sudo make install
```

