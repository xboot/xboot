# Rockchip RV1106

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-rv1106
```

## Enter maskrom mode and using xrock for burning spi nor flash (offset = 0x00010000, sector = 128)
```shell
sudo xrock maskrom rv1106_ddr_924MHz_v1.09.bin rv1106_usbplug_v1.06.bin --rc4-off;
sleep 3;
sudo xrock flash write 128 xbootpak.bin;
sudo xrock reset;
```

## Enter maskrom mode and using xrock for burning spi nand flash (offset = 0x00040000, sector = 512)
```shell
sudo xrock maskrom rv1106_ddr_924MHz_v1.09.bin rv1106_usbplug_v1.06.bin --rc4-off;
sleep 3;
sudo xrock flash write 512 xbootpak.bin;
sudo xrock reset;
```

## Enter maskrom mode and using xrock for burning emmc (offset = 0x00008000, sector = 64)
```shell
sudo xrock maskrom rv1106_ddr_924MHz_v1.09.bin rv1106_usbplug_v1.06.bin --rc4-off;
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
