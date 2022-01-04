# Rockchip RK3399

## Compile source code and generate the target files at the output directory
```shell
make clean
make CROSS_COMPILE=/path/to/aarch64-none-elf- PLATFORM=arm64-rk3399
```

## Enter maskrom mode and burn image to emmc
```shell
sudo rkdeveloptool db xbootpak.bin;
sleep 3;
sudo rkdeveloptool ul xbootpak.bin;
sudo rkdeveloptool rd;
```

## About rkdeveloptool tool

The default rkdeveloptool which is open sourced and used for burning images in the maskrom mode, and support all chips by rockchip

## Download rkdeveloptool source code
```shell
git clone https://github.com/rockchip-linux/rkdeveloptool.git
```

## Make and install rkdeveloptool
```shell
autoreconf -i
./configure
make
sudo make install
```

