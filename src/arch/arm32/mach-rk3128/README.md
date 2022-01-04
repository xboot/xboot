# Rockchip RK3128

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rk3128
```

## Enter maskrom mode and using xrock for burning image
```shell
sudo xrock maskrom rk3128_ddr_300MHz_v2.12.bin rk3128_usbplug_v2.63.bin;
sleep 3;
sudo xrock flash write 64 xbootpak.bin;
sudo xrock reset;
```

## Download xrock source code
```shell
git clone https://github.com/xboot/xrock.git
```

## Make and install xrock
```shell
make
sudo make install
```

