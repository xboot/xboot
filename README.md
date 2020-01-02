![xboot-logo](https://github.com/xboot/xboot/raw/master/developments/logo/xboot-logo.png)

***

# XBOOT简介([English Version](README.md#xboot-introduction))

```
       _                   _                     
 _  _ | |___ _____ _____ _| |_                   
\ \/ /|  _  |  _  |  _  |_   _|  (C) 2007-2020   
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
* [XBOOT系统开发指南](https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-zh-CN.md)
* [XBOOT应用开发指南](https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-zh-CN.md)

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

* ## X86_64位linux系统下的sandbox

sandbox依赖与SDL2库，在编译前需要安装libsdl2-dev，以ubuntu系统为例:
```shell
sudo apt-get install libsdl2-dev
```
```shell
make clean
make CROSS_COMPILE="" PLATFORM=x64-sandbox
```

# 运行XBOOT([效果演示](https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-zh-CN.md))
各个平台运行方式不太一样，具体查看板级支持包里面的相关说明文件，Realview模拟器平台直接执行tools目录下的相关脚本，linux系统下的sandbox平台，直接双击生成的目标文件执行即可。

# 讨论组，大佬聚集，请踊跃加入
XBOOT官方QQ群：[658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000人)

# 资助鼓励

如果您认为我做的这些对您来说是有价值的, 并鼓励我进行更多的开源及免费开发. 那您可以资助我, 就算是一杯咖啡...
![donate-with-wechat](https://github.com/xboot/xboot/raw/master/documents/images/donate-with-wechat.png)
![donate-with-alipay](https://github.com/xboot/xboot/raw/master/documents/images/donate-with-alipay.png)

***
# Xboot Introduction
It very tedious that we need careful read soc datasheet when operate GPIO on soc.We always repeat working when changed the soc.its almost almost impossible to We want develop a UI that support all kinds of magic motion.Stacks of protocols are like heavenly books ,it Reading is very hard , How do we program? Virtual machine technology is very popular and it function is powerful,but it very difficult to transplant it.so we build the Xboot.it can help us deal with these problems.
XBOOT is not only a powerful, portable, and highly reusable, embedded system bootloader,but also on a piece of SOC system application software execution engine, without complex operating system, electricity directly executed on the APP。"Once written, running everywhere." It not just a slogan,but also the only reason for the existence of XBOOT. What's on the XBOOT?

- Support file systems
- Support lua virtual machine
- Support many protocol stacks
- Support graphics library, and vector font
- Supports a modern GUI, and animations
- Multi-platform support
- Bus drivers, UART, I2C, SPI and so on
- Device drivers, GPIO, PWM, IRQ, CLK, LED, BUZZER, VIBRATOR, WATCHDOG, RNG, FRAMEBUFFER, RTC, etc.
- Support application using lua, which include high-level API, can operate a variety of hardware abstract interface directly
- Application software platform has nothing to do, write once, run everywhere

# System And Application Development Guide
* [Xboot System Development Guide](https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md)
* [Xboot Application Development Guide](https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md)

# Download Development Tools
* [Download Eclipse Integrated Development Environment (Integrated cross toolchains)](http://pan.baidu.com/s/1i3ImG0d)
* [Download Cross Toolchains](http://pan.baidu.com/s/1dDtssIt)

# Compile The Source Code
Makefile have two variables need to pass, one is cross toolchain, the other is a specific hardware platform

| variable      | Description                              |
| ------------- | ---------------------------------------- |
| CROSS_COMPILE | The specical cross toolchain             |
| PLATFORM      | The hardware platform, have two parts, `arch` and `mach` |

* ## Realview Platform，qemu-system-arm Emulator

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-realview
```

* ## Linux Sandbox On X86_64

The sandbox depends on the SDL2 library, you need to install libsdl2-dev before compile, as an example at the ubuntu system:

```shell
sudo apt-get install libsdl2-dev
```
```shell
make clean
make CROSS_COMPILE="" PLATFORM=x64-sandbox
```

# Running Xboot([ShowCase](https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md))
Each platform is not same, Please see the relevant documentation in the BSP directory, To running realview platform using some scripts in the tools, The sandbox platform just double-click the generated target file

# Discussion Group, Many Big Brother, Please Join Us
XBOOT Official Tencent QQ Group: [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000 people)

# Donation

If you find my work useful and you want to encourage the development of more free resources, you can do it by donating… 

[![donate-with-paypal](https://github.com/xboot/xboot/raw/master/documents/images/donate-with-paypal.png)](https://www.paypal.me/openxboot/10)