# 源码目录结构 <!-- {docsify-ignore} -->

```
xboot
├── developments
├── documents
├── examples
├── src
│   ├── arch
│   │   ├── arm32
│   │   │   ├── include
│   │   │   ├── lib
│   │   │   ├── mach-apple
│   │   │   ├── mach-f1c100s
│   │   │   ├── mach-h3
│   │   │   ├── mach-n3ds
│   │   │   ├── mach-raspi2
│   │   │   ├── mach-realview
│   │   │   ├── mach-s5pv210
│   │   │   ├── mach-v3s
│   │   │   ├── mach-x3128
│   │   │   ├── mach-x3288
│   │   │   └── mach-x4418
│   │   ├── arm64
│   │   │   ├── include
│   │   │   ├── lib
│   │   │   ├── mach-raspi3
│   │   │   ├── mach-virt
│   │   │   ├── mach-x3399
│   │   │   └── mach-x6818
│   │   └── x64
│   │       ├── include
│   │       ├── lib
│   │       └── mach-sandbox
│   ├── driver
│   │   ├── adc
│   │   ├── audio
│   │   ├── battery
│   │   ├── block
│   │   ├── buzzer
│   │   ├── clk
│   │   ├── clockevent
│   │   ├── clocksource
│   │   ├── console
│   │   ├── dac
│   │   ├── dma
│   │   ├── framebuffer
│   │   ├── gmeter
│   │   ├── gpio
│   │   ├── hygrometer
│   │   ├── i2c
│   │   ├── input
│   │   ├── interrupt
│   │   ├── laserscan
│   │   ├── led
│   │   ├── light
│   │   ├── motor
│   │   ├── nvmem
│   │   ├── pwm
│   │   ├── regulator
│   │   ├── reset
│   │   ├── rng
│   │   ├── rtc
│   │   ├── sd
│   │   ├── servo
│   │   ├── spi
│   │   ├── stepper
│   │   ├── thermometer
│   │   ├── uart
│   │   ├── vibrator
│   │   └── watchdog
│   ├── external
│   │   ├── cairo-1.14.10
│   │   ├── chipmunk-7.0.2
│   │   ├── freetype-2.8
│   │   ├── json-parser-1.1.0
│   │   ├── libfdt-1.4.2
│   │   ├── libpng-1.6.32
│   │   ├── lua-5.3.4
│   │   ├── lua-cjson-2.1.0
│   │   ├── pixman-0.34.0
│   │   └── zlib-1.2.11
│   ├── framework
│   ├── include
│   ├── init
│   ├── kernel
│   │   ├── command
│   │   ├── core
│   │   ├── fs
│   │   ├── shell
│   │   ├── time
│   │   └── xfs
│   ├── lib
│   │   ├── libc
│   │   ├── libm
│   │   └── libx
│   └── romdisk
│       ├── application
│       ├── boot
│       ├── framework
│       ├── private
│       ├── storage
│       └── sys
└── tools
```

## developments
该目录主要放置一些辅助性开发资料，比如`S5P4418`、`RK3288`等引导启动制作工具源码，`Raspberry PI 2`机械尺寸图及扩展口相关定义，各种LOGO及多媒体资源等文件。

## documents
该目录主要放置XBOOT相关的各种开发文档，包括《XBOOT系统开发指南》、《XBOOT应用开发指南》、《XBOOT运行效果展示》等等。

## examples
这里主要存放各种参考例程，按主题内容，分目录存放。在编写应用软件时，应首先参考此目录里的相关实现，可加快开发进度，降低学习成本。

## src
XBOOT源码主目录，所有的实现代码都在此目录下，在此目录下，有一个主Makefile，这个Makefile管理所有源码的编译规则，能够递归搜索子目录，并自动建立依赖关系，也就是说您只需将某个`c`文件或者`S`文件放置在某个搜索路径下，则会自动编译并链接，无需对Makefile进行修改。该Makefile提供了一组默认编译参数，但默认编译参数无法适应所有平台，故该Makefile会主动加载板级支持包里面的`xboot.mk`，以提供一种修改默认编译参数的机制。

### arch
体系结构主目录，每个具体的体系实现里提供了一个`include`及`lib`目录，用于放置该体系通用代码，比如汇编版的`memcpy` `memset`等加速函数，就放置在该`lib`目录，共享的头文件放置在`include`目录下。该目录现已支持如下体系结构：

| 名称    | 描述         |
| ----- | ---------- |
| arm32 | ARM 32位指令集 |
| arm64 | ARM 64位指令集 |
| x64   | X86 64位指令集 |

#### arm32
该目录下包含了很多以`mach-`开头的目录，这些都是同一体系下的不同板级支持包。aarch32体系包括`arm7` `arm9` `cortex-a8` `cortex-a9` `cortex-a15` `cortex-a17` `cortex-a5` `cortex-a7`等，已支持的平台列举如下：

| 目录            | 描述                                      |
| ------------- | --------------------------------------- |
| mach-apple    | 苹果A4处理器，包括Iphone4，Ipod touch 4g，Ipad 1g |
| mach-f1c100s  | 全志F1C100S                               |
| mach-h3       | 全志H3芯片，Nano PI                          |
| mach-n3ds     | 任天堂3DS，包括后出的New 2DSXLL                  |
| mach-raspi2   | Raspberry PI 2代                         |
| mach-realview | Qemu Realview模拟器                        |
| mach-s5pv210  | 三星S5PV210                               |
| mach-v3s      | 全志V3S芯片，Lichee PI Zero                  |
| mach-x3128    | 瑞芯微RK3128                               |
| mach-x3288    | 瑞芯微RK3288                               |
| mach-x4418    | 三星S5P4418                               |

#### arm64
该目录主要支持aarch64体系，包括`cortex-a53` `cortex-a72`等，目录组织方式同`arm32`体系，已支持的平台列举如下：

| 目录          | 描述              |
| ----------- | --------------- |
| mach-raspi3 | Raspberry PI 3代 |
| mach-virt   | Qemu Virt模拟器    |
| mach-x3399  | 瑞芯微RK3399       |
| mach-x6818  | 三星S5P6818       |

#### x64
该目录，就是传统的PC平台，`X86_64`指令集，目录组织方式同`arm32`体系，这里仅实现了沙盒，运行在PC操作系统之上。

### driver
驱动框架主目录，各种类型的驱动框架都放置在此，已实现以下设备驱动模型：

| 名称          | 描述        |
| ----------- | --------- |
| adc         | ADC驱动     |
| audio       | 音频驱动      |
| battery     | 电池电量驱动    |
| block       | 块设备驱动     |
| buzzer      | 蜂鸣器驱动     |
| clk         | 时钟驱动      |
| clockevent  | 时钟事件驱动    |
| clocksource | 时钟源驱动     |
| console     | 超级终端驱动    |
| dac         | DAC驱动     |
| dma         | DMA驱动     |
| framebuffer | 显示屏驱动     |
| gmeter      | 重力传感器驱动   |
| gpio        | GPIO驱动    |
| hygrometer  | 湿度传感器驱动   |
| i2c         | I2C总线驱动   |
| input       | 输入设备驱动    |
| interrupt   | 中断控制器驱动   |
| laserscan   | 激光振镜驱动    |
| led         | LED驱动     |
| light       | 光线传感器驱动   |
| motor       | 马达驱动      |
| nvmem       | 非易失性存储器驱动 |
| pwm         | PWM驱动     |
| regulator   | 电压调节器驱动   |
| reset       | 复位驱动      |
| rng         | 随机数发生器驱动  |
| rtc         | 实时时钟驱动    |
| sd          | SD卡驱动     |
| servo       | 伺服舵机驱动    |
| spi         | SPI总线驱动   |
| stepper     | 步进电机驱动    |
| thermometer | 温度传感器驱动   |
| uart        | 串口总线驱动    |
| vibrator    | 振动马达驱动    |
| watchdog    | 看门狗驱动     |

### external
这个目录，主要放置各种第三方软件包，如下表所示:

| 目录                | 描述          |
| ----------------- | ----------- |
| cairo-1.14.10     | 2D矢量图形库     |
| chipmunk-7.0.2    | 2D物理引擎      |
| freetype-2.8      | 矢量字体渲染引擎    |
| json-parser-1.1.0 | json文件解析器   |
| libfdt-1.4.2      | dt设备树解析库    |
| libpng-1.6.32     | png图片处理库    |
| lua-5.3.4         | lua虚拟机      |
| lua-cjson-2.1.0   | json lua接口库 |
| pixman-0.34.0     | 底层图形运算库     |
| zlib-1.2.11       | zip压缩库      |

### framework
中间件实现层，主要放置与虚拟机相关的实现代码，是驱动与应用软件间的桥梁，虚拟机的入口文件为`vm.c`

### include
头文件目录，所有的`.h`文件都放置在此。

### init
此目录下放置主入口函数以及版本信息等，主入口函数在做一些必要的初始化工作后，就进入shell命令行，等待用户键入命令，或者直接启动某个应用软件。

### kernel
内核目录，提供各种核心组件实现，是整个系统的基石。

#### command
实现种命令行工具，在shell命令行下，可键入相关指令来运行。

#### core
这个目录是整个系统的核心，提供诸如设备管理、驱动管理、设备树、事件管理、异步消息、性能监测等等。

#### fs
文件系统支持目录，实现虚拟文件系统，可挂载各种格式，包括`sysfs`。

#### shell
命令行解释器实现，提供命令行交互环境。

#### time
定时器相关实现，用红黑树管理，有别于传统的定时器方案，纳秒级精度。

#### xfs
扩展文件系统，为上层应用软件提供沙盒机制并统一的访问接口。

### lib
这个目录存放自行实现的各种底层库，包括`libc`、`libm`、`libx`等相关库函数。

### romdisk
这是编译进系统的romdisk目录，也就是xboot的根文件系统，存储为cpio格式，并在链接目标文件时一起打包。在系统运行时，会自动挂载到根目录作根文件系统。在板级支持包下，存在一个同名的目录，两者之间是overlay的关系，即板级支持包里的romdisk目录会覆盖默认系统源码目录下的romdisk。板级相关的文件，请放到板级支持包里的romdisk目录。

| 目录          | 描述          |
| :---------- | ----------- |
| application | 系统应用软件目录    |
| boot        | 设备树存放目录     |
| framework   | 核心组件        |
| private     | 私有目录        |
| storage     | 外部存储目录      |
| sys         | sys文件系统挂载目录 |

## tools
提供各种预编译好的二进制工具，包括QEMU模拟器，各种运行脚本等。

