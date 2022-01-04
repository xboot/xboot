# Allwinner V3S, Lichee PI Zero

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-v3s
```

## Program to RAM and execute
```shell
sudo xfel ddr v3s; sudo xfel write 0x40000000 xboot.bin; sudo xfel exec 0x40000000;
```

## Burn normal image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin
```

## Burn compress image to SPI Nor Flash
```shell
sudo xfel spinor write 0 xboot.bin.z
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

