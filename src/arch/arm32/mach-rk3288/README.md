# Rockchip RK3288

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rk3288
```

## Enter maskrom mode and using xrock for burning image
```shell
sudo xrock maskrom rk3288_ddr_400MHz_v1.09.bin rk3288_usbplug_v2.58.bin;
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

