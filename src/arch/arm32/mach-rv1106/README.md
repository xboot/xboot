# Rockchip RV1106

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rv1106
```

## Enter maskrom mode and using xrock for burning image
```shell
sudo xrock maskrom rv1106_ddr_924MHz_v1.07.bin rv1106_usbplug_v1.03.bin --rc4-off;
sleep 3;
sudo xrock flash write 512 xbootpak.bin;
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

