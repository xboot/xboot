# [XBOOT](http://xboot.github.io)
操作一个GPIO，需要仔细对照芯片手册，好繁琐；每换一个主控芯片，所有工作从头来过；想开发个现代点支持各种动效的UI，发现几乎是不可能的事情；各种协议栈有如天书一样，阅读都困难，何谈编写；虚拟机技术很流行，功能强大，想自己移植个，可是...；还是放开自己吧，让XBOOT来替你解决这些问题。xboot不仅仅是一款功能强大、可移植性强、代码复用率高的嵌入式系统bootloader，而且还是一款SOC片上系统应用软件执行引擎，无需复杂的操作系统，APP上电直接执行。一次编写，到处运行，不仅仅是个口号，而是还是xboot之所以存在的唯一原因。
一些基本特性，简单列举如下：
  - 支持文件系统
  - 支持lua虚拟机
  - 支持各种协议栈
  - 支持矢量图形库，矢量字体
  - 支持各种现代gui控件，以及动效
  - 多平台支持
  - 各种总线驱动，UART,I2C,SPI等等
  - 各种设备驱动，GPIO,PWM,IRQ,CLK,CLOCKSOURCE,CLOCKEVENT,LED,BUZZER,VIBRATOR,WATCHDOG,RNG,BLOCK,FRAMEBUFFER,RTC等等
  - 支持用lua编写应用软件，包含高等级API,可直接操作各种硬件抽象接口
  - 应用软件平台无关，一次编写，到处运行

## Eclipse集成开发环境
> http://pan.baidu.com/s/1i3ImG0d

## 交叉工具链
> http://pan.baidu.com/s/1dDtssIt

## 编译xboot
	"CROSS_COMPILE"变量，指定交叉工具链
	"PLATFORM"变量，指定硬件平台
### qemu-system-arm中的realview模拟器
```
	make clean
	make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-realview
```
## 运行xboot
    各种平台运行方式不太一样，具体查看各种板级支持包里面的说明文件，对于x64平台上的sandbox双击即可，realview模拟器可以执行tools目录下的脚本即可。
