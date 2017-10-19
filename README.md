![xboot-logo]
***
# [XBOOT][xboot-homepage]简介
```
       _                   _                     
 _  _ | |___ _____ _____ _| |_                   
\ \/ /|  _  |  _  |  _  |_   _|  (C) 2007-2017   
 )  ( | |_| | |_| | |_| | | |____JIANJUN.JIANG__ 
/_/\_\|_____|_____|_____| |_____________________|
```
操作一个GPIO，需要仔细对照芯片手册，好繁琐；每换一个主控芯片，所有工作从头来过；想开发个现代点支持各种动效的UI，发现几乎是不可能的事情；各种协议栈有如天书一样，阅读都困难，何谈编写；虚拟机技术很流行，功能很强大，想自己移植个，可是困难重重；还是放开自己吧，让XBOOT来替你解决这些问题。XBOOT不仅仅是一款功能强大、可移植性强、代码复用率高的嵌入式系统bootloader，而且还是一款SOC片上系统应用软件执行引擎，无需复杂的操作系统，APP上电直接执行。一次编写，到处运行，不仅仅是个口号，而且还是XBOOT存在的唯一原因。一些基本特性，简单列举如下：
* 支持文件系统
* 支持lua虚拟机
* 支持各种协议栈
* 支持矢量图形库，矢量字体
* 支持各种现代GUI控件，以及动效
* 多平台支持
* 各种总线驱动，UART，I2C，SPI等等
* 各种设备驱动，GPIO，PWM，IRQ，CLK，LED，BUZZER，VIBRATOR，WATCHDOG，RNG，FRAMEBUFFER，RTC等
* 支持用lua编写应用软件，包含高等级API,可直接操作各种硬件抽象接口
* 应用软件平台无关，一次编写，到处运行

![linux-sandbox-game-2048](https://github.com/xboot/xboot/raw/master/documents/images/linux-sandbox-game-2048.gif)

# 系统及应用开发指南
* [XBOOT系统开发指南](https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide.md)
* [XBOOT应用开发指南](https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide.md)

# 下载开发工具
* [Eclipse集成开发环境 (已集成交叉工具链)](http://pan.baidu.com/s/1i3ImG0d)
* [交叉工具链](http://pan.baidu.com/s/1dDtssIt)

# 编译源码
Makefile中有两个变量在编译时需要传递，一个是交叉工具链，另一个是具体的硬件平台

| 变量            | 说明                          |
| ------------- | --------------------------- |
| CROSS_COMPILE | 指定交叉工具链                     |
| PLATFORM      | 指定硬件平台，由两部分组成，`arch`及`mach` |

* ## Realview平台，qemu-system-arm模拟器

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-realview
```

* ## 全志V3S，荔枝派(lichee-pi zero)

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-v3s
```

* ## Raspberry PI 2代

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-raspi2
```

* ## Raspberry PI 3代

```shell
make clean
make CROSS_COMPILE=/path/to/aarch64-none-elf- PLATFORM=arm64-raspi3
```

* ## X86_64位linux系统下的sandbox

sandbox依赖与SDL2库，在编译前需要安装libsdl2-dev，以ubuntu系统为例:
```shell
sudo apt-get install libsdl2-dev
```
```shell
make clean
make CROSS_COMPILE="" PLATFORM=x64-sandbox
```

# 运行XBOOT([效果演示](https://github.com/xboot/xboot/blob/master/documents/xboot-show-case.md))
各个平台运行方式不太一样，具体查看板级支持包里面的相关说明文件，Realview模拟器平台直接执行tools目录下的相关脚本，linux系统下的sandbox平台，直接双击生成的目标文件执行即可。

# 讨论组，大佬聚集，请踊跃加入
XBOOT官方QQ群：[658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000人)

***
[xboot-homepage]:http://xboot.github.io "xboot's homepage"
[xboot-logo]:https://github.com/xboot/xboot/raw/master/developments/logo/xboot-logo.png "xboot's logo"
