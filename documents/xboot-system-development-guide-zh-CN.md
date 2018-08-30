# XBOOT系统开发指南

# 简介
操作一个GPIO，需要仔细对照芯片手册，好繁琐；每换一个主控芯片，所有工作从头来过；想开发个现代点支持各种动效的UI，发现几乎是不可能的事情；各种协议栈有如天书一样，阅读都困难，何谈编写；虚拟机技术很流行，功能很强大，想自己移植个，可是困难重重；还是放开自己吧，让XBOOT来替你解决这些问题。XBOOT不仅仅是一款功能强大、可移植性强、代码复用率高的嵌入式系统bootloader，而且还是一款SOC片上系统应用软件执行引擎，无需复杂的操作系统，APP上电直接执行。一次编写，到处运行，不仅仅是个口号，而且还是XBOOT存在的唯一原因。

一些基本特性，简单列举如下：

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

# 相关网址
* XBOOT官方首页
  https://xboot.github.io/

* XBOOT源码(完全开源)，寄存在github，请直接`fork`一份代码，如需申请合并，直接`pull request`即可
  https://github.com/xboot/xboot

* XBOOT运行效果展示页面，首次接触的用户，建议观看，对XBOOT有个直观的感受
  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-en-US.md

* XBOOT系统开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-en-US.md

* XBOOT应用开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-en-US.md

* XBOOT官方QQ群，大佬聚集，请踊跃加入
  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000人)

# 集成开发环境

工欲善其事，必先利其器。为提高开发效率，XBOOT提供了一套完整的集成开发环境，选择了开源界大名鼎鼎的Eclipse。

Eclipse集成开发环境是一个开源的IDE平台，其以强大的可扩展性而著称，传统的IDE公司都已转向Eclipse平台，在此平台上开发自家的插件，然后包装销售，典型的有ARM公司推出的MDK开发平台。

XBOOT官方提供的Eclipse IDE，其包含JRE运行环境，各种交叉工具链，各种插件，Windows版还提供了诸如make、tar、mkdir、rm等必要的命令行工具。

Eclipse支持跨平台特性，能够在Windows、Linux、Mac等操作系统上运行。

## 安装开发环境
首先按如下链接，下载相关软件，根据文件名，注意分辨自己所需的开发环境，推荐在Linux下开发，Windows下编译速度较慢，严重影响开发效率。

* 集成开发环境(已集成交叉工具链)下载链接:
   http://pan.baidu.com/s/1i3ImG0d

* 交叉工具链(命令行编译环境)下载链接：
  http://pan.baidu.com/s/1dDtssIt

此软件为绿色版，下载后直接解压，路径中不要带有空格符号，在Windows平台运行eclipse.bat脚本启动Eclipse集成开发环境；在Linux平台运行eclipse.sh脚本启动集成开发环境。

> 不可直接运行eclipse可执行文件，必须通过脚本启动Eclipse，否则，某些关键命令行工具无法运行。脚本在启动集成开发环境时，添加了搜索路径，以便让集成开发环境能够正确调用相关命令行工具。

## 启动开发环境

打开Eclipse集成开发环境，通过上述相关脚本来启动。这里以Windows开发平台为例，Linux平台与此类似，不再赘述。首先，打开软件后会弹出工作空间选择对话框，这里选择`D:\workspace`，可按照个人喜好，选择其他路径，注意避免中文及空格。

![eclipse-workspace-launcher](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-workspace-launcher.png)

点击`OK`按钮，第一次启动会出现欢迎界面，可直接关闭进入主界面。

![eclipse-welcome](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-welcome.png)

![eclipse-empty-screen](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-empty-screen.png)

## 导入源码

选择`File` `Import` ，导入git工程到当前工作空间。

![eclipse-file-new-import](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-file-new-import.png)

![eclipse-project-from-git](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-project-from-git.png)

点击`Next` ，出现git仓库选择界面，这里可以选择存在的本地git仓库，或者网络git仓库，这里选择`Clone URI`。

![eclipse-git-clone-uri](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-git-clone-uri.png)

点击`Next`，出现填充仓库URI链接界面，这里填写`https://github.com/xboot/xboot.git`，其他内容会根据所填的URI自动完成。

![eclipse-import-xboot-git](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-git.png)

点击`Next`，出现远程仓库分支选择界面，勾选主分支`master`，可以全部勾选。

![eclipse-import-xboot-select-branch](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-select-branch.png)

点击`Next`，选择目录及初始化分支，这里路径选择工作空间，然后初始化分支选择`master`主分支。

![eclipse-import-xboot-select-directory](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-select-directory.png)

点击`Next`，从远程仓库克隆源码，这个过程比较久，请耐心等待。

![eclipse-import-xboot-receiving-objects](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-receiving-objects.png)

克隆完成后，弹出如下界面：

![eclipse-import-xboot-receiving-complete](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-receiving-complete.png)

![eclipse-import-xboot-finish](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-import-xboot-finish.png)

点击`Finish` ，完成xboot工程的导入步骤，如下图所示：

![eclipse-xboot-project-explorer](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-xboot-project-explorer.png)

## 编译源码

首先，Makefile中有两个变量在编译时需要传递，一个是交叉工具链，另一个是具体的硬件平台

| 变量            | 说明                          |
| ------------- | --------------------------- |
| CROSS_COMPILE | 指定交叉工具链                     |
| PLATFORM      | 指定硬件平台，由两部分组成，`arch`及`mach` |

在编译前，需要修改默认的Makefile，以传递正确的`CROSS_COMPILE` 及`PLATFORM`，这里以`Realview`模拟器平台为例，修改后的Makefile，如下图所示：

![eclipse-modify-makefile](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-modify-makefile.png)

点击菜单`Project` `Build all` 或者直接按快捷键`CTRL+B`编译整个源码。Windows平台编译速度较慢，请耐心等待。

![eclipse-build-all](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-build-all.png)

观察`Console`窗口，直至编译完成。

![eclipse-make-finish](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-make-finish.png)

此时，在`output`目录下会生成`xboot.bin`等相关目标文件。

| 文件名       | 描述                      |
| --------- | ----------------------- |
| xboot     | 带有符号的ELF目标文件，模拟器直接加载此文件 |
| xboot.bin | 不带任何符号信息的二进制文件，一般作为烧写镜像 |
| xboot.map | 各个符号详细链接信息              |

![eclipse-make-xboot-output](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-make-xboot-output.png)

## 运行

启动模拟器，双击`tools\qemu-system\windows`目录下的`realview-run.bat`脚本即可。

![eclipse-realview-run-bat](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-run-bat.png)

模拟器运行速度较慢，如需体验真实性能，请在真机上实验，实际效果如下图所示：

![eclipse-realview-show-logo](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-show-logo.png)

![eclipse-realview-play-geme-2048](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-play-geme-2048.png)

> 如果是Linux系统，在执行`realview-run.sh`脚本前，需确认当前系统是否已安装qemu-system-arm，如没有安装，以ubuntu为例，请执行如下指令：
```shell
sudo apt-get install qemu-system-arm
```

## 调试

在调试前，我们需要先修改`src\arch\arm32\mach-realview\xboot.mk`默认的编译参数，默认`-O3`优化选项，编译时，会优化掉很多有用的符号信息，不利于调试，这里改成`-O0`参数，禁止编译器做任何优化，在修改参数后，需要清除一下，再重新完整编译，具体修改如下图所示：

![eclipse-realview-o3-o0](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-o3-o0.png)

确认开发环境里是否已安装`GDB Hardware Debuging`插件，这个插件在`CDT`插件包里，如果没有安装，可以通过`Help` `Install new software`菜单来安装，如下图所示：

![eclipse-install-gdb-hardware-debugging](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-install-gdb-hardware-debugging.png)

配置调试器，点击`Run` `Debug Configurations` ，然后双击`GDB Hardware Debugging`，按下图配置：

![eclipse-gdb-debug-config-main](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-config-main.png)

![eclipse-gdb-debug-config-debugger](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-config-debugger.png)

设置断点，可以直接双击源码左侧，这里我们设置`xboot_main`函数的断点。

![eclipse-gdb-debug-break-main](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-break-main.png)

启动`GDB Server`，双击`tools\qemu-system\windows`目录下的`realview-dbg.bat`脚本即可。

![eclipse-realview-dbg-bat](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-dbg-bat.png)

运行GDB调试器，观察`Console`窗口输出

![eclipse-gdb-debug-launch](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-launch.png)

正常连接后，会出现如下调试界面，断点在`_start`处。

![eclipse-realview-gdb-debug-start](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-gdb-debug-start.png)

点击如下工具栏，可以运行，停止，单步，步过，跳出等基本操作。

![eclipse-realview-gdb-debug-start](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-tools.png)

点击运行，单步等命令按钮，就可以调试了，如下图所示：

![eclipse-gdb-debug-main](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-gdb-debug-main.png)

## 编译其他平台

Makefile中有两个变量在编译时需要传递，一个是交叉工具链，另一个是具体的硬件平台。

| 变量            | 说明                          |
| ------------- | --------------------------- |
| CROSS_COMPILE | 指定交叉工具链                     |
| PLATFORM      | 指定硬件平台，由两部分组成，`arch`及`mach` |

* ### Realview平台，qemu-system-arm模拟器

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-realview
```

* ### 全志V3S，荔枝派(lichee-pi zero)

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-v3s
```

* ### Raspberry PI 2代

```shell
make clean
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-raspi2
```

* ### Raspberry PI 3代

```shell
make clean
make CROSS_COMPILE=/path/to/aarch64-none-elf- PLATFORM=arm64-raspi3
```

* ### X86_64位linux系统下的sandbox

sandbox依赖与SDL2库，在编译前需要安装libsdl2-dev，以ubuntu系统为例:
```shell
sudo apt-get install libsdl2-dev
```
```shell
make clean
make CROSS_COMPILE="" PLATFORM=x64-sandbox
```

# 源码目录结构

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

# 系统移植
为了能够在一个全新的平台运行XBOOT，必须进行系统移植，也就是需要开发针对这个平台的BSP支持包，XBOOT的BSP支持包全部放置在`arch`目录下，其包括各种驱动，头文件，根文件系统，各种工具及文档，有效降低了模块间的耦合度。具体的目录组织形式请参阅`源码目录结构`章节。本章节以全志`V3S`平台为例，重点讲述如何从零开始支持一款全新的平台。首先列出最终的移植文件，先有个整体印象，并方便讲述。

```
mach-v3s
├── include
│   ├── v3s
│   │   ├── reg-ccu.h
│   │   ├── reg-de.h
│   │   ├── reg-dram.h
│   │   └── reg-tcon.h
│   ├── configs.h
│   ├── v3s-gpio.h
│   ├── v3s-irq.h
│   └── v3s-reset.h
├── driver
│   ├── ce-v3s-timer.c
│   ├── clk-v3s-pll.c
│   ├── cs-v3s-timer.c
│   ├── fb-v3s.c
│   ├── gpio-v3s.c
│   ├── irq-gic400.c
│   ├── irq-v3s-gpio.c
│   ├── key-v3s-lradc.c
│   ├── pwm-v3s.c
│   ├── reset-v3s.c
│   ├── spi-v3s.c
│   ├── ts-ns2009.c
│   ├── uart-16550.c
│   └── wdog-v3s.c
├── start.S
├── sys-uart.c
├── sys-clock.c
├── sys-dram.c
├── sys-spi-flash.c
├── sys-copyself.c
├── licheepi-zero.c
├── arm32-gdbstub.c
├── exception.c
├── README-en-US.md
├── README-zh-CN.md
├── romdisk
│   └── boot
│       └── licheepi-zero.json
├── tools
│   ├── linux
│   │   └── mksunxi
│   └── windows
│       └── mksunxi.exe
├── xboot.ld
└── xboot.mk
```

## 创建目录
首先我们需要根据平台选取板级名称，可以采用产品名或者芯片名。选用原则是，如果该BSP需要支持多个板子，那么就用芯片名，否则直接采用产品名。我们这里选用芯片名作为板级名称`mach-v3s`。建好根目录后，我们需要创建其他几个子目录，包括`include` `driver` `romdisk` `tools`。

| 目录      | 描述        |
| :------ | --------- |
| include | 板级头文件目录   |
| driver  | 板级驱动目录    |
| romdisk | 板级根文件系统目录 |
| tools   | 板级工具目录    |

## 创建xboot.mk
xboot.mk文件主要是为了修改主Makefile的默认编译参数，比如添加全局宏定义，指定编译器优化参数，在生成目标文件后，对目标文件进行加头处理以让芯片BROM能够正确识别并引导。`V3S`CPU核心为`Cortex-A7`，根据芯片填写具体的编译参数：

```
#
# Machine makefile
#

DEFINES		+= -D__ARM32_ARCH__=7 -D__CORTEX_A7__ -D__ARM32_NEON__

ASFLAGS		:= -g -ggdb -Wall -O3
CFLAGS		:= -g -ggdb -Wall -O3
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostdlib
MCFLAGS		:= -march=armv7-a -mtune=cortex-a7 -mfpu=vfpv4 -mfloat-abi=hard -marm -mno-thumb-interwork

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=
SRCDIRS		:=

ifeq ($(strip $(HOSTOS)), linux)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/linux/mksunxi
endif
ifeq ($(strip $(HOSTOS)), windows)
MKSUNXI		:= arch/$(ARCH)/$(MACH)/tools/windows/mksunxi
endif

xend:
	@echo Make header information for brom booting
	@$(MKSUNXI) $(X_NAME).bin
```
可以注意到在文件末尾，有个`xend`依赖规则，这个是对最终生成的目标文件做加头处理，这里用到了`mksunxi`这个工具，该工具是根据`V3S` BROM引导校验要求制作而成。现代的大部分SOC处理器在引导启动时，其内部的固化ROM都会对镜像进行一定的校验认证工作，而且大部分SOC也不会提供具体的技术细节。所以在移植前，需要研究引导启动并制作出相应的做头工具。这部分通常需要阅读大量的开放源码，并且由一些零星的信息，总结出中间技术细节，有可能还需要用上逆向工程技术，这是移植的难点，同时也是一个平台是否能够移植成功的基础。

## 创建xboot.ld
xboot.ld文件是链接器生成最终目标文件时使用的链接脚本，它控制着整个目标代码的生成，比如入口点如何指定，代码段，数据段，堆，栈，内嵌根文件系统在哪里，如何保证系统自举代码链接到前32K空间等等。这里面涉及很多技术细节，自行编写难度较大，建议直接拷一份其他平台的链接脚本，简单做些修改就可以满足要求了。

```
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

STACK_UND_SIZE = 0x40000;
STACK_ABT_SIZE = 0x40000;
STACK_IRQ_SIZE = 0x40000;
STACK_FIQ_SIZE = 0x40000;
STACK_SRV_SIZE = 0x100000;

MEMORY
{
	ram  : org = 0x40000000, len = 16M
	dma  : org = 0x41000000, len = 16M
	heap : org = 0x42000000, len = 32M
}

SECTIONS
{
	.text :
	{
		PROVIDE(__image_start = .);
		PROVIDE(__text_start = .);
		.obj/arch/arm32/mach-v3s/start.o (.text)
		.obj/arch/arm32/lib/memcpy.o (.text)
		.obj/arch/arm32/lib/memset.o (.text)
		.obj/arch/arm32/mach-v3s/sys-uart.o (.text)
		.obj/arch/arm32/mach-v3s/sys-clock.o (.text)
		.obj/arch/arm32/mach-v3s/sys-dram.o (.text)
		.obj/arch/arm32/mach-v3s/sys-spi-flash.o (.text)
		.obj/arch/arm32/mach-v3s/sys-copyself.o (.text)
		*(.text*)
		*(.init.text)
		*(.exit.text)
		*(.glue*)
		*(.note.gnu.build-id)
		PROVIDE(__text_end = .);
	} > ram

	.initcall ALIGN(8) :
	{
		PROVIDE(__initcall_start = .);
		KEEP(*(.initcall_0.text))
		KEEP(*(.initcall_1.text))
		KEEP(*(.initcall_2.text))
		KEEP(*(.initcall_3.text))
		KEEP(*(.initcall_4.text))
		KEEP(*(.initcall_5.text))
		KEEP(*(.initcall_6.text))
		KEEP(*(.initcall_7.text))
		KEEP(*(.initcall_8.text))
		KEEP(*(.initcall_9.text))
		PROVIDE(__initcall_end = .);
	} > ram

	.exitcall ALIGN(8) :
	{
		PROVIDE(__exitcall_start = .);
		KEEP(*(.exitcall_9.text))
		KEEP(*(.exitcall_8.text))
		KEEP(*(.exitcall_7.text))
		KEEP(*(.exitcall_6.text))
		KEEP(*(.exitcall_5.text))
		KEEP(*(.exitcall_4.text))
		KEEP(*(.exitcall_3.text))
		KEEP(*(.exitcall_2.text))
		KEEP(*(.exitcall_1.text))
		KEEP(*(.exitcall_0.text))
		PROVIDE(__exitcall_end = .);
	} > ram

	.ksymtab ALIGN(8) :
	{
		PROVIDE(__ksymtab_start = .);
		KEEP(*(.ksymtab.text))
		PROVIDE(__ksymtab_end = .);
	} > ram

	.romdisk ALIGN(8) :
	{
		PROVIDE(__romdisk_start = .);
		KEEP(*(.romdisk))
		PROVIDE(__romdisk_end = .);
	} > ram

	.rodata ALIGN(8) :
	{
		PROVIDE(__rodata_start = .);
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		PROVIDE(__rodata_end = .);
	} > ram

	.data_shadow ALIGN(8) :
	{
		PROVIDE(__data_shadow_start = .);
		PROVIDE(__data_shadow_end = (. + SIZEOF(.data)));
		PROVIDE(__image_end = __data_shadow_end);
	} > ram

	.data : AT(ADDR(.data_shadow))
	{
		PROVIDE(__data_start = .);	
		*(.data*)
		. = ALIGN(8);
  		PROVIDE(__data_end = .);
	} > ram

	.ARM.exidx ALIGN(8) :
	{
		PROVIDE (__exidx_start = .);
		*(.ARM.exidx*)
		PROVIDE (__exidx_end = .);
	} > ram

	.ARM.extab ALIGN(8) :
	{
		PROVIDE (__extab_start = .);
		*(.ARM.extab*)
		PROVIDE (__extab_end = .);
	} > ram

	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(8);
		PROVIDE(__bss_end = .);
	} > ram

	.stack ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__stack_start = .);
		PROVIDE(__stack_und_start = .);
		. += STACK_UND_SIZE;
		PROVIDE(__stack_und_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_abt_start = .);
		. += STACK_ABT_SIZE;
		PROVIDE(__stack_abt_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_irq_start = .);
		. += STACK_IRQ_SIZE;
		PROVIDE(__stack_irq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_fiq_start = .);
		. += STACK_FIQ_SIZE;
		PROVIDE(__stack_fiq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_srv_start = .);
		. += STACK_SRV_SIZE;
		PROVIDE(__stack_srv_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_end = .);
	} > ram

	.dma ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__dma_start = ORIGIN(dma));
		PROVIDE(__dma_end = ORIGIN(dma) + LENGTH(dma));
	} > dma

	.heap ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__heap_start = ORIGIN(heap));
		PROVIDE(__heap_end = ORIGIN(heap) + LENGTH(heap));
	} > heap

	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_info 0 : { *(.debug_info) }
	.debug_line 0 : { *(.debug_line) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	.debug_aranges 0 : { *(.debug_aranges) }
}
```

## 创建configs.h
这个是板级配置文件，用于修改系统默认配置。一般情况下，没有特殊需求，基本上不需修改默认定义，我们这里建立一个空文件。
```
#ifndef __OVERRIDE_CONFIGS_H__
#define __OVERRIDE_CONFIGS_H__

#endif /* __OVERRIDE_CONFIGS_H__ */
```

## 创建start.S
start.S文件是程序的入口文件，一般由汇编代码编写而成。包括初始化向量表、栈指针，初始化系统时钟、DDR控制器、还有实现自举引导、为C语言准备环境、最终跳转到RAM中并执行`xboot_main`函数等等。这部分代码的编写难度较大，需要较深的编程功底。可以参考系统里的其它实现模仿而来，分步骤编写并测试代码。

```
.macro save_regs
	sub sp, sp, #68
	stmia sp, {r0-r12}
	ldr r0, [sp, #72]
	str r0, [sp, #60]
	ldr r0, [sp, #76]
	mov r3, r0
	orr r0, r0, #0xc0
	msr cpsr_c, r0
	mov r1, sp
	mov r2, lr
	cps #0x13
	str r1, [sp, #52]
	str r2, [sp, #56]
	str r3, [sp, #64]
	mov r0, sp
.endm

.macro restore_regs
	ldr r1, [sp, #52]
	ldr r2, [sp, #56]
	ldr r0, [sp, #64]
	orr r0, r0, #0xc0
	msr cpsr_c, r0
	mov sp, r1
	mov lr, r2
	cps #0x13
	ldr r0, [sp, #60]
	str r0, [sp, #72]
	ldmia sp, {r0-r12}
	add sp, #68
.endm

/*
 * Exception vector table
 */
.text
	.arm

	.global	_start
_start:
	/* Boot head information for BROM */
	.long 0xea000016
	.byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'
	.long 0, 0x8000
	.byte 'S', 'P', 'L', 2
	.long 0, 0
	.long 0, 0, 0, 0, 0, 0, 0, 0	/* 0x20 - dram size, 0x28 - boot type */
	.long 0, 0, 0, 0, 0, 0, 0, 0	/* 0x40 - boot params */

_vector:
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

/*
 * The actual reset code
 */
reset:
	/* Save boot params to 0x00000040 */
	ldr r0, =0x00000040
	str sp, [r0, #0]
	str lr, [r0, #4]
	mrs lr, cpsr
	str lr, [r0, #8]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #12]
	mrc p15, 0, lr, c12, c0, 0
	str lr, [r0, #16]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #20]

	/* Enter svc mode cleanly and mask interrupts */
	mrs r0, cpsr
	eor r0, r0, #0x1a
	tst r0, #0x1f
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	bne 1f
	orr r0, r0, #0x100
	adr lr, 2f
	msr spsr_cxsf, r0
	.word 0xe12ef30e /* msr elr_hyp, lr */
	.word 0xe160006e /* eret */
1:	msr cpsr_c, r0
2:	nop

	/* Set vector base address register */
	ldr r0, =_vector
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0

	/* Enable SMP mode for dcache, by setting bit 6 of auxiliary ctl reg */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, r0, #(1 << 6)
	mcr p15, 0, r0, c1, c0, 1

	/* Enable neon/vfp unit */
	mrc p15, 0, r0, c1, c0, 2
	orr r0, r0, #(0xf << 20)
	mcr p15, 0, r0, c1, c0, 2
	isb
	mov r0, #0x40000000
	vmsr fpexc, r0

	/* Initial system uart, clock and ddr */
	bl sys_uart_init
	bl sys_clock_init
	bl sys_dram_init

	/* Copyself to link address */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq 1f
	bl sys_copyself
1:	nop

	/* Initialize stacks */
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x1b
	msr cpsr_cxsf, r1
	ldr sp, _stack_und_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x17
	msr cpsr_cxsf, r1
	ldr sp, _stack_abt_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x12
	msr cpsr_cxsf, r1
	ldr sp, _stack_irq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x11
	msr cpsr_cxsf, r1
	ldr sp, _stack_fiq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x13
	msr cpsr_cxsf, r1
	ldr sp, _stack_srv_end

	/* Copy data section */
	ldr r0, _data_start
	ldr r1, _data_shadow_start
	ldr r2, _data_shadow_end
	sub r2, r2, r1
	bl memcpy

	/* Clear bss section */
	ldr r0, _bss_start
	ldr r2, _bss_end
	sub r2, r2, r0
	mov r1, #0
	bl memset

	/* Call _main */
	ldr r1, =_main
	mov pc, r1
_main:
	mov r0, #1;
	mov r1, #0;
	bl xboot_main
	b _main

	.global return_to_fel
return_to_fel:
	ldr r0, =0x00000040
	ldr sp, [r0, #0]
	ldr lr, [r0, #4]
	ldr r1, [r0, #20]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #16]
	mcr p15, 0, r1, c12, c0, 0
	ldr r1, [r0, #12]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #8]
	msr cpsr, r1
	bx lr

/*
 * Exception handlers
 */
	.align 5
undefined_instruction:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_undefined_instruction
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
software_interrupt:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_software_interrupt
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
prefetch_abort:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_prefetch_abort
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
data_abort:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_data_abort
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
not_used:
	b .

	.align 5
irq:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_irq
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

	.align 5
fiq:
	sub lr, lr, #4
	srsdb sp!, #0x13
	cps #0x13
	cpsid if
	push {lr}
	save_regs
	bl arm32_do_fiq
	restore_regs
	pop {lr}
	cpsie if
	rfeia sp!

/*
 * The location of section
 */
 	.align 4
_image_start:
	.long __image_start
_image_end:
	.long __image_end
_data_shadow_start:
	.long __data_shadow_start
_data_shadow_end:
	.long __data_shadow_end
_data_start:
	.long __data_start
_data_end:
	.long __data_end
_bss_start:
	.long __bss_start
_bss_end:
	.long __bss_end
_stack_und_end:
	.long __stack_und_end
_stack_abt_end:
	.long __stack_abt_end
_stack_irq_end:
	.long __stack_irq_end
_stack_fiq_end:
	.long __stack_fiq_end
_stack_srv_end:
	.long __stack_srv_end
```

## 创建licheepi-zero.c
每个板级支持包的实现，必须至少注册一个`machine`，`machine`是具体硬件平台的描述，同时也提供了必要的顶层接口，包括关机、重启、序列号、生成密钥等，结构体定义如下：

```
struct machine_t {
	struct kobj_t * kobj;
	struct list_head list;

	const char * name;
	const char * desc;
	const struct mmap_t * map;

	int (*detect)(struct machine_t * mach);
	void (*memmap)(struct machine_t * mach);
	void (*shutdown)(struct machine_t * mach);
	void (*reboot)(struct machine_t * mach);
	void (*sleep)(struct machine_t * mach);
	void (*cleanup)(struct machine_t * mach);
	void (*logger)(struct machine_t * mach, const char * buf, int count);
	const char * (*uniqueid)(struct machine_t * mach);
	int (*keygen)(struct machine_t * mach, const char * msg, void * key);
};
```

这里有个比较关键的方法，就是`detect`方法，这是machine的检测函数，如果返回为真，代表检测到该平台并依据对应的设备树文件生成设备，如果返回为假，则检测不通过，继续下一个`machine`的检测，直到检测到为止。
```
#include <xboot.h>
#include <mmu.h>

static const struct mmap_t mach_map[] = {
	{"ram",  0x40000000, 0x40000000, SZ_16M, MAP_TYPE_CB},
	{"dma",  0x41000000, 0x41000000, SZ_16M, MAP_TYPE_NCNB},
	{"heap", 0x42000000, 0x42000000, SZ_32M, MAP_TYPE_CB},
	{ 0 },
};

static u32_t sram_read_id(virtual_addr_t virt)
{
	u32_t id;

	write32(virt, read32(virt) | (1 << 15));
	id = read32(virt) >> 16;
	write32(virt, read32(virt) & ~(1 << 15));
	return id;
}

static int mach_detect(struct machine_t * mach)
{
	u32_t id = sram_read_id(phys_to_virt(0x01c00024));

	if(id == 0x1681)
		return 1;
	return 0;
}

static void mach_memmap(struct machine_t * mach)
{
	mmu_setup(mach->map);
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x01c28000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[32 + 3 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(0x01c23800);
	u32_t sid0, sid1, sid2, sid3;

	sid0 = read32(virt + 0 * 4);
	sid1 = read32(virt + 1 * 4);
	sid2 = read32(virt + 2 * 4);
	sid3 = read32(virt + 3 * 4);
	snprintf(uniqueid, sizeof(uniqueid), "%08x:%08x:%08x:%08x",sid0, sid1, sid2, sid3);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t licheepi_zero = {
	.name 		= "licheepi-zero",
	.desc 		= "Lichee Pi Zero Based On Allwinner V3S SOC",
	.map		= mach_map,
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void licheepi_zero_machine_init(void)
{
	register_machine(&licheepi_zero);
}

static __exit void licheepi_zero_machine_exit(void)
{
	unregister_machine(&licheepi_zero);
}

machine_initcall(licheepi_zero_machine_init);
machine_exitcall(licheepi_zero_machine_exit);
```

## 创建licheepi-zero.json
一个`machine`对应一个`json`设备树，在注册一个`machine`的同时，也需要提供相应的`json`设备树。当然系统里也可以提供多个`machine`及多个`json`设备树。系统在启动时，会自动遍历所有`machine`，如果某个`machine`检测通过，则会依据名称加载对应的设备树文件。利用这套机制，可以实现一个镜像支持若干平台，前提是这些平台提供了硬件版本号，且软件能够正确获取并加以区分。

```
{
	"clk-fixed@0": { "name": "osc24m", "rate": 24000000 },
	"clk-fixed@1": { "name": "osc32k", "rate": 32768 },
	"clk-fixed@2": { "name": "iosc", "rate": 16000000 },

	"clk-v3s-pll@0": { "parent": "osc24m", "name": "pll-cpu", "channel": 0 },
	"clk-v3s-pll@1": { "parent": "osc24m", "name": "pll-audio", "channel": 1 },
	"clk-v3s-pll@2": { "parent": "osc24m", "name": "pll-video", "channel": 2 },
	"clk-v3s-pll@3": { "parent": "osc24m", "name": "pll-ve", "channel": 3 },
	"clk-v3s-pll@4": { "parent": "osc24m", "name": "pll-ddr0", "channel": 4 },
	"clk-v3s-pll@5": { "parent": "osc24m", "name": "pll-periph0", "channel": 5 },
	"clk-v3s-pll@6": { "parent": "osc24m", "name": "pll-isp", "channel": 6 },
	"clk-v3s-pll@7": { "parent": "osc24m", "name": "pll-periph1", "channel": 7 },
	"clk-v3s-pll@8": { "parent": "osc24m", "name": "pll-ddr1", "channel": 8 },

	"clk-fixed-factor@0": { "parent": "osc24m", "name": "osc24m-750", "mult": 1, "div": 750 },
	"clk-fixed-factor@1": { "parent": "pll-periph0", "name": "pll-periph0-2", "mult": 1, "div": 2 },
	"clk-fixed-factor@2": { "parent": "iosc", "name": "losc", "mult": 1, "div": 512 },

	"clk-mux@0x01c20050": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 },
			{ "name": "pll-cpu", "value": 2 }
		],
		"name": "cpu", "shift": 16, "width": 2
	},
	"clk-divider@0x01c20050": { "parent": "cpu", "name": "axi", "shift": 0, "width": 2, "divider-one-based": true },
	"clk-divider@0x01c20054": { "parent": "pll-periph0", "name": "ahb1-pre-div", "shift": 6, "width": 2, "divider-one-based": true },
	"clk-mux@0x01c20054": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 },
			{ "name": "axi", "value": 2 },
			{ "name": "ahb1-pre-div", "value": 3 }
		],
		"name": "mux-ahb1", "shift": 12, "width": 2
	},
	"clk-ratio@0x01c20054": { "parent": "mux-ahb1", "name": "ahb1", "shift": 4, "width": 2 },
	"clk-ratio@0x01c20054": { "parent": "ahb1", "name": "apb1", "shift": 8, "width": 2 },
	"clk-mux@0x01c20058": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 },
			{ "name": "pll-periph0", "value": 2 }
		],
		"name": "mux-apb2", "shift": 24, "width": 2
	},
	"clk-ratio@0x01c20058": { "parent": "mux-apb2", "name": "ratio-apb2", "shift": 16, "width": 2 },
	"clk-divider@0x01c20058": { "parent": "ratio-apb2", "name": "apb2", "shift": 0, "width": 4, "divider-one-based": true },
	"clk-mux@0x01c2005c": {
		"parent": [
			{ "name": "ahb1", "value": 0 },
			{ "name": "pll-periph0-2", "value": 1 }
		],
		"name": "ahb2", "shift": 0, "width": 2
	},

	"clk-gate@0x01c2006c": {"parent": "apb2", "name": "gate-bus-uart0", "shift": 16, "invert": false },
	"clk-gate@0x01c2006c": {"parent": "apb2", "name": "gate-bus-uart1", "shift": 17, "invert": false },
	"clk-gate@0x01c2006c": {"parent": "apb2", "name": "gate-bus-uart2", "shift": 18, "invert": false },
	"clk-link": { "parent": "gate-bus-uart0", "name": "link-uart0" },
	"clk-link": { "parent": "gate-bus-uart1", "name": "link-uart1" },
	"clk-link": { "parent": "gate-bus-uart2", "name": "link-uart2" },

	"clk-gate@0x01c2006c": {"parent": "apb2", "name": "gate-bus-i2c0", "shift": 0, "invert": false },
	"clk-gate@0x01c2006c": {"parent": "apb2", "name": "gate-bus-i2c1", "shift": 1, "invert": false },
	"clk-link": { "parent": "gate-bus-i2c0", "name": "link-i2c0" },
	"clk-link": { "parent": "gate-bus-i2c1", "name": "link-i2c1" },

	"clk-gate@0x01c200cc": {"parent": "ahb1", "name": "gate-bus-usbphy0", "shift": 8, "invert": false },
	"clk-gate@0x01c20060": {"parent": "gate-bus-usbphy0", "name": "gate-bus-usb-otg-device", "shift": 24, "invert": false },
	"clk-gate@0x01c20060": {"parent": "gate-bus-usbphy0", "name": "gate-bus-usb-otg-ehci0", "shift": 26, "invert": false },
	"clk-gate@0x01c20060": {"parent": "gate-bus-usbphy0", "name": "gate-bus-usb-otg-ohci0", "shift": 29, "invert": false },
	"clk-gate@0x01c200cc": {"parent": "gate-bus-usb-otg-ohci0", "name": "gate-usb-otg-ohci0", "shift": 16, "invert": false },
	"clk-link": { "parent": "gate-bus-usb-otg-device", "name": "link-usb-otg-device" },
	"clk-link": { "parent": "gate-bus-usb-otg-ehci0", "name": "link-usb-otg-ehci0" },
	"clk-link": { "parent": "gate-usb-otg-ohci0", "name": "link-usb-otg-ohci0" },

	"clk-gate@0x01c20060": {"parent": "ahb1", "name": "gate-bus-hstimer", "shift": 19, "invert": false },
	"clk-link": { "parent": "gate-bus-hstimer", "name": "link-hstimer" },

	"clk-gate@0x01c20070": {"parent": "ahb2", "name": "gate-bus-ephy", "shift": 0, "invert": false },
	"clk-gate@0x01c20060": {"parent": "gate-bus-ephy", "name": "gate-bus-emac", "shift": 17, "invert": false },
	"clk-link": { "parent": "gate-bus-emac", "name": "link-emac" },

	"clk-mux@0x01c200a0": {
		"parent": [
			{ "name": "osc24m", "value": 0 },
			{ "name": "pll-periph0", "value": 1 },
			{ "name": "pll-periph1", "value": 2 }
		],
		"name": "mux-spi0", "shift": 24, "width": 2,
		"default": { "parent": "pll-periph0" }
	},
	"clk-ratio@0x01c200a0": { "parent": "mux-spi0", "name": "ratio-spi0", "shift": 16, "width": 2 },
	"clk-divider@0x01c200a0": { "parent": "ratio-spi0", "name": "div-spi0", "shift": 0, "width": 4, "divider-one-based": true, "default": { "rate": 100000000 } },
	"clk-gate@0x01c200a0": {"parent": "div-spi0", "name": "gate-spi0", "shift": 31, "invert": false },
	"clk-gate@0x01c20060": {"parent": "gate-spi0", "name": "gate-bus-spi0", "shift": 20, "invert": false },
	"clk-link": { "parent": "gate-bus-spi0", "name": "link-spi0" },

	"clk-mux@0x01c20c10": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 }
		],
		"name": "mux-timer0", "shift": 2, "width": 2
	},
	"clk-ratio@0x01c20c10": { "parent": "mux-timer0", "name": "ratio-timer0", "shift": 4, "width": 3 },
	"clk-link": { "parent": "ratio-timer0", "name": "link-timer0" },

	"clk-mux@0x01c20c20": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 }
		],
		"name": "mux-timer1", "shift": 2, "width": 2
	},
	"clk-ratio@0x01c20c20": { "parent": "mux-timer1", "name": "ratio-timer1", "shift": 4, "width": 3 },
	"clk-link": { "parent": "ratio-timer1", "name": "link-timer1" },

	"clk-mux@0x01c20c30": {
		"parent": [
			{ "name": "losc", "value": 0 },
			{ "name": "osc24m", "value": 1 }
		],
		"name": "mux-timer2", "shift": 2, "width": 2
	},
	"clk-ratio@0x01c20c30": { "parent": "mux-timer2", "name": "ratio-timer2", "shift": 4, "width": 3 },
	"clk-link": { "parent": "ratio-timer2", "name": "link-timer2" },

	"clk-link": { "parent": "osc24m", "name": "link-pwm" },
	"clk-link": { "parent": "osc24m-750", "name": "link-wdt" },

	"clk-mux@0x01c20104": {
		"parent": [
			{ "name": "pll-video", "value": 0 },
			{ "name": "pll-periph0", "value": 1 }
		],
		"name": "mux-de", "shift": 24, "width": 3,
		"default": { "parent": "pll-video" }
	},
	"clk-divider@0x01c20104": { "parent": "mux-de", "name": "div-de", "shift": 0, "width": 4, "divider-one-based": true, "default": { "rate": 396000000 } },
	"clk-gate@0x01c20104": {"parent": "div-de", "name": "gate-de", "shift": 31, "invert": false },
	"clk-gate@0x01c20064": {"parent": "gate-de", "name": "gate-bus-de", "shift": 12, "invert": false },
	"clk-link": { "parent": "gate-bus-de", "name": "link-de" },

	"clk-mux@0x01c20118": {
		"parent": [
			{ "name": "pll-video", "value": 0 },
			{ "name": "pll-periph0", "value": 1 }
		],
		"name": "mux-tcon", "shift": 24, "width": 3,
		"default": { "parent": "pll-video" }
	},
	"clk-divider@0x01c20118": { "parent": "mux-tcon", "name": "div-tcon", "shift": 0, "width": 4, "divider-one-based": true, "default": { "rate": 396000000 } },
	"clk-gate@0x01c20118": {"parent": "div-tcon", "name": "gate-tcon", "shift": 31, "invert": false },
	"clk-gate@0x01c20064": {"parent": "gate-tcon", "name": "gate-bus-tcon", "shift": 4, "invert": false },
	"clk-link": { "parent": "gate-bus-tcon", "name": "link-tcon" },

	"reset-v3s@0x01c202c0": {
		"reset-base": 0,
		"reset-count": 32
	},

	"reset-v3s@0x01c202c4": {
		"reset-base": 32,
		"reset-count": 32
	},

	"reset-v3s@0x01c202c8": {
		"reset-base": 64,
		"reset-count": 32
	},

	"reset-v3s@0x01c202d0": {
		"reset-base": 96,
		"reset-count": 32
	},

	"reset-v3s@0x01c202d8": {
		"reset-base": 128,
		"reset-count": 32
	},

	"irq-gic400@0x01c80000": {
		"interrupt-base": 32,
		"interrupt-count": 128
	},

	"irq-v3s-gpio@0x01c20a20": {
		"interrupt-base": 160,
		"interrupt-count": 10,
		"interrupt-parent": 47
	},

	"irq-v3s-gpio@0x01c20a40": {
		"interrupt-base": 192,
		"interrupt-count": 6,
		"interrupt-parent": 49
	},

	"gpio-v3s@0x01c20824": {
		"gpio-base": 32,
		"gpio-count": 10,
		"interrupt-offset": 160
	},

	"gpio-v3s@0x01c20848": {
		"gpio-base": 64,
		"gpio-count": 4,
		"interrupt-offset": -1
	},

	"gpio-v3s@0x01c20890": {
		"gpio-base": 128,
		"gpio-count": 25,
		"interrupt-offset": -1
	},

	"gpio-v3s@0x01c208b4": {
		"gpio-base": 160,
		"gpio-count": 7,
		"interrupt-offset": -1
	},

	"gpio-v3s@0x01c208d8": {
		"gpio-base": 192,
		"gpio-count": 6,
		"interrupt-offset": 192
	},

	"pwm-v3s@0x01c21400": {
		"clock-name": "link-pwm",
		"channel": 0,
		"pwm-gpio": 36,
		"pwm-gpio-config": 2
	},

	"pwm-v3s@0x01c21400": {
		"clock-name": "link-pwm",
		"channel": 1,
		"pwm-gpio": 37,
		"pwm-gpio-config": 2
	},

	"ce-v3s-timer@0x01c20c00": {
		"clock-name": "link-timer0",
		"interrupt": 50
	},

	"cs-v3s-timer@0x01c20c00": {
		"clock-name": "link-timer1"
	},

	"uart-16550@0x01c28000": {
		"clock-name": "link-uart0",
		"reset": 144,
		"txd-gpio": 40,
		"txd-gpio-config": 3,
		"rxd-gpio": 41,
		"rxd-gpio-config": 3,
		"baud-rates": 115200,
		"data-bits": 8,
		"parity-bits": 0,
		"stop-bits": 1
	},

	"uart-16550@0x01c28400": {
		"clock-name": "link-uart1",
		"reset": 145,
		"txd-gpio": 149,
		"txd-gpio-config": 4,
		"rxd-gpio": 150,
		"rxd-gpio-config": 4,
		"baud-rates": 115200,
		"data-bits": 8,
		"parity-bits": 0,
		"stop-bits": 1
	},

	"uart-16550@0x01c28800": {
		"clock-name": "link-uart2",
		"reset": 146,
		"txd-gpio": 32,
		"txd-gpio-config": 2,
		"rxd-gpio": 33,
		"rxd-gpio-config": 2,
		"baud-rates": 115200,
		"data-bits": 8,
		"parity-bits": 0,
		"stop-bits": 1
	},

	"i2c-gpio@0": {
		"sda-gpio": 39,
	 	"sda-gpio-config": 1,
		"scl-gpio": 38,
		"scl-gpio-config": 1,
		"sda-open-drain": false,
		"scl-open-drain": false,
		"scl-output-only": false,
		"delay-us": 5
	},

	"spi-v3s@0x01c68000": {
		"clock-name": "link-spi0",
		"reset": 20,
		"sclk-gpio": 65,
		"sclk-gpio-config": 3,
		"mosi-gpio": 67,
		"mosi-gpio-config": 3,
		"miso-gpio": 64,
		"miso-gpio-config": 3,
		"cs-gpio": 66,
		"cs-gpio-config": 3
	},

	"spi-flash@0": {
		"spi-bus": "spi-v3s.0",
		"chip-select": 0,
		"mode": 0,
		"speed": 50000000
	},

	"wdog-v3s@0x01c20ca0": {
		"clock-name": "link-wdt"
	},

	"key-v3s-lradc@0x01c22800": {
		"interrupt": 62,
		"reference-voltage": 3000000,
		"keys": [
			{ "min-voltage": 100000, "max-voltage": 300000, "key-code": 6 },
			{ "min-voltage": 300000, "max-voltage": 500000, "key-code": 7 },
			{ "min-voltage": 500000, "max-voltage": 700000, "key-code": 10 },
			{ "min-voltage": 700000, "max-voltage": 900000, "key-code": 11 }
		]
	},
	
	"ts-ns2009@0": {
		"i2c-bus": "i2c-gpio.0",
		"slave-address": 72,
		"median-filter-length": 5,
		"mean-filter-length": 5,
		"calibration": [14052, 21, -2411064, -67, 8461, -1219628, 65536],
		"poll-interval-ms": 10
	},

	"led-gpio@0": {
		"gpio": 194,
		"gpio-config": 1,
		"active-low": true,
		"default-brightness": 0
	},

	"led-gpio@1": {
		"gpio": 192,
		"gpio-config": 1,
		"active-low": true,
		"default-brightness": 0
	},

	"led-gpio@2": {
		"gpio": 193,
		"gpio-config": 1,
		"active-low": true,
		"default-brightness": 0
	},

	"ledtrigger-heartbeat@0": {
		"led-name": "led-gpio.0",
		"period-ms": 1260
	},

	"led-pwm-bl@0": {
		"pwm-name": "pwm-v3s.0",
		"pwm-period-ns": 1000000,
		"pwm-polarity": true,
		"pwm-percent-from": 6,
		"pwm-percent-to": 100,
		"default-brightness": 0
	},

	"buzzer-pwm@0": {
		"pwm-name": "pwm-v3s.1",
		"pwm-polarity": true
	},

	"fb-v3s@0x01000000": {
		"clock-name-de": "link-de",
		"clock-name-tcon": "link-tcon",
		"reset-de": 44,
		"reset-tcon": 36,
		"width": 800,
		"height": 480,
		"physical-width": 216,
		"physical-height": 135,
		"bits-per-pixel": 18,
		"bytes-per-pixel": 4,
		"clock-frequency": 33000000,
		"hfront-porch": 40,
		"hback-porch": 87,
		"hsync-len": 1,
		"vfront-porch": 13,
		"vback-porch": 31,
		"vsync-len": 1,
		"hsync-active": false,
		"vsync-active": false,
		"den-active": false,
		"clk-active": false,
		"backlight": "led-pwm-bl.0"
	},

	"console-uart@0": {
		"uart-bus": "uart-16550.0"
	}
}
```

## mksunxi工具
`V3S`引导头为96字节，其中开始4字节为相对跳转指令并偏移96字节，然后就是8字节魔法数字，从第12字节开始的4个字节为CheckSum，这个字段就是最终需要填充的字段。紧接着是SPL启动代码长度，这里最大32K，也就是说BROM会首先拷贝32K代码到IRAM中并运行以实现自举。96字节完整定义如下：

```
/* Boot head information for BROM */
.long 0xea000016
.byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'
.long 0, 0x8000
.byte 'S', 'P', 'L', 2
.long 0, 0
.long 0, 0, 0, 0, 0, 0, 0, 0	/* 0x20 - dram size, 0x28 - boot type */
.long 0, 0, 0, 0, 0, 0, 0, 0	/* 0x40 - boot params */
```

mksunxi工具就是根据上面的规则填充CheckSum字段，该算法为累加和，小端格式存储，初值为0x5F0A6C39，实现代码如下：
```
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#if 0
static inline uint32_t __swab32(uint32_t x)
{
	return ( (x<<24) | (x>>24) | \
		((x & (uint32_t)0x0000ff00UL)<<8) | \
		((x & (uint32_t)0x00ff0000UL)>>8) );
}
#define cpu_to_le32(x)	(__swab32((uint32_t)(x)))
#define le32_to_cpu(x)	(__swab32((uint32_t)(x)))
#else
#define cpu_to_le32(x)	(x)
#define le32_to_cpu(x)	(x)
#endif

struct boot_head_t {
	uint32_t instruction;
	uint8_t magic[8];
	uint32_t checksum;
	uint32_t length;
	uint8_t spl_signature[4];
	uint32_t fel_script_address;
	uint32_t fel_uenv_length;
	uint32_t dt_name_offset;
	uint32_t reserved1;
	uint32_t boot_media;
	uint32_t string_pool[13];
};

int main (int argc, char *argv[])
{
	struct boot_head_t * h;
	FILE * fp;
	char * buffer;
	int buflen, filelen;
	uint32_t * p;
	uint32_t sum;
	int i, l, loop;
	
	if(argc != 2)
	{
		printf("Usage: mksunxi <bootloader>\n");
		return -1;
	}

	fp = fopen(argv[1], "r+b");
	if(fp == NULL)
	{
		printf("Open bootloader error\n");
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	if(filelen <= sizeof(struct boot_head_t))
	{
		fclose(fp);
		printf("The size of bootloader too small\n");
		return -1;
	}

	buflen = (filelen + 0x2000) & ~(0x2000 - 1);
	buffer = malloc(buflen);
	memset(buffer, 0, buflen);
	if(fread(buffer, 1, filelen, fp) != filelen)
	{
		printf("Can't read bootloader\n");
		free(buffer);
		fclose(fp);
		return -1;
	}

	h = (struct boot_head_t *)buffer;
	p = (uint32_t *)h;
	l = le32_to_cpu(h->length);
	h->checksum = cpu_to_le32(0x5F0A6C39);
	loop = l >> 2;
	for(i = 0, sum = 0; i < loop; i++)
		sum += le32_to_cpu(p[i]);
	h->checksum = cpu_to_le32(sum);
	
	fseek(fp, 0L, SEEK_SET);
	if(fwrite(buffer, 1, buflen, fp) != buflen)
	{
		printf("Write bootloader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}

	fclose(fp);
	printf("The bootloader head has been fixed\n");
	return 0;
}
```

## 编写驱动程序
移植进行到这一步，基本系统已经能够运行起来了，但是系统里还没有任何驱动。按照驱动实现的优先级及难易程度，推荐如下顺序：

| 驱动       | 文件名             |
| -------- | --------------- |
| 时钟驱动     | clk-v3s-pll.c   |
| RESET驱动  | reset-v3s.c     |
| GPIO驱动   | gpio-v3s.c      |
| 串口驱动     | uart-16550.c    |
| 系统中断驱动   | irq-gic400.c    |
| GPIO中断驱动 | irq-v3s-gpio.c  |
| 时钟源驱动    | cs-v3s-timer.c  |
| 时钟事件驱动   | ce-v3s-timer.c  |
| PWM驱动    | pwm-v3s.c       |
| SPI驱动    | spi-v3s.c       |
| ADC按键驱动  | key-v3s-lradc.c |
| 看门狗驱动    | wdog-v3s.c      |
| 显示屏驱动    | fb-v3s.c        |

这里仅列出部分`V3S`驱动，还有相当一大部分驱动是仅通过设备树就可以添加的。关于如何编写驱动，以及如何用设备树来描述设备，请参阅下一个章节`驱动开发`。

# 驱动开发
驱动开发是系统开发中的一个重要环节，也是XBOOT的两大核心关注点(驱动及应用)之一，在正式进入驱动开发之前，这里先介绍几个概念，了解了这些背景知识后，将有助于理解本章内容。驱动与设备是两个不同的概念，平时交流中，经常会混用两个概念，大家也不会觉得有违和感，那是因为大家将他们看成了一个整体，在理解具体要表达的内容上没有太大歧义。但是，在实际开发中，我们要严格区分这两个概念。驱动可类比为函数，是个执行机构，负责具体的执行动作；设备可类比为变量，用于描述自身属性。两者之间相互依存，缺一不可。系统中用哈希表分别管理了所有的驱动以及所有的设备，并为同种类型的设备提供了统一的操作接口。有了驱动及设备的概念后，为了进一步简化问题，这里引入了设备树的概念，设备树概念的引入，大大降低了添加设备的难度，很多时候，注册个新设备，仅仅需要填个设备属性描述就可以了。关于设备树的详细技术实现，参考后面章节的`设备树技术源码解析`。

## ADC驱动

ADC缩写`Analog to Digital Converter`，即模数转换器 ，能够将连续变化的模拟信号转换为离散的数字信号。真实世界中的模拟信号，例如温度、压力、声音或者图像等，都需要通过ADC变换后，才能够处理及存储。

### ADC驱动结构体定义

ADC驱动结构体，包含如下定义：

- 驱动名字
- 参考电压
- 转换精度
- 转换通道数
- 转换方法
- 私有数据

```c
struct adc_t
{
	char * name;
	int vreference;
	int resolution;
	int nchannel;

	u32_t (*read)(struct adc_t * adc, int channel);
	void * priv;
};
```

### ADC设备搜索、注册及卸载

```c
struct adc_t * search_adc(const char * name);
bool_t register_adc(struct device_t ** device, struct adc_t * adc);
bool_t unregister_adc(struct adc_t * adc);
```

### ADC设备C操作API

```c
u32_t adc_read_raw(struct adc_t * adc, int channel);
int adc_read_voltage(struct adc_t * adc, int channel);
```

## 音频驱动

## 电池电量驱动

## 块设备驱动

## 蜂鸣器驱动

## 时钟驱动

## 时钟事件驱动

## 时钟源驱动

## 超级终端驱动

## DAC驱动

## DMA驱动

## 显示屏驱动

## 重力传感器驱动

## GPIO驱动

## 湿度传感器驱动

## I2C总线驱动

## 输入设备驱动

## 中断控制器驱动

## 激光震镜驱动

## LED驱动

## 光线传感器驱动

## 马达驱动

## 非易失性存储器驱动

## PWM驱动

## 电压调节器驱动

## 复位驱动

## 随机数发生器驱动

## 实时时钟驱动

## SD卡驱动

## 伺服舵机驱动

## SPI总线驱动

## 步进电机驱动

## 温度传感器驱动

## 串口总线驱动

## 振动马达驱动

## 看门狗驱动

# 应用接口

# 源码解析
## 核心组件kobj技术详解
kobj顾名思意，即内核object，其层次表现为一颗倒置的树，用来管理各种设备，驱动、运行时状态等。在文件系统中，通过sysfs接口绑定kobj节点，来实现对该节点的读写操作。sysfs接口有两种最基本的用途：

- 组件外部操作接口，应用可以通过此接口操作设备。

- 调试驱动模块，或查看运行时状态。

### 引言
> 实现kobj可以对任何形态的组件进行高度抽象，是各种组件的基石，系统中提供了一组API，以方便管理kobj。理解了kobj，对后面理解xboot中的各种核心组件，有相当大的帮助，建议细心阅读。

### kobj节点类型
kobj有两种基本类型，一个是目录，其可以挂接子节点，另一个是文件，代表一颗树的末端，不可拥有子节点，但提供了读或者写该节点的操作，声明如下:
```c
enum kobj_type_t {
	KOBJ_TYPE_DIR,
	KOBJ_TYPE_REG,
};
```

### kobj结构体定义
kobj结构体拥有名字，节点类型，父节点指针，当前节点链表，子节点链表头，节点锁，读写接口函数及一个私有数据指针，详细定义如下:
```c
struct kobj_t
{
	/* kobj name */
	char * name;
	
	/* kobj type DIR or REG */
	enum kobj_type_t type;
	
	/* kobj's parent */
	struct kobj_t * parent;
	
	/* kobj's entry */
	struct list_head entry;
	
	/* kobj's children */
	struct list_head children;
	
	/* kobj lock */
	spinlock_t lock;
	
	/* kobj read */
	ssize_t (*read)(struct kobj_t * kobj, void * buf, size_t size);
	
	/* kobj write */
	ssize_t (*write)(struct kobj_t * kobj, void * buf, size_t size);
	
	/* private data */
	void * priv;
};
```

### 分配kobj节点
其为一内部函数，主要是分配一段内存，然后利用传递的参数进行初始化，并返回分配的kobj节点。
```c
static struct kobj_t * __kobj_alloc(const char * name, enum kobj_type_t type, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!name)
		return NULL;
	
	kobj = malloc(sizeof(struct kobj_t));
	if(!kobj)
		return NULL;
	
	kobj->name = strdup(name);
	kobj->type = type;
	kobj->parent = kobj;
	init_list_head(&kobj->entry);
	init_list_head(&kobj->children);
	spin_lock_init(&kobj->lock);
	kobj->read = read;
	kobj->write = write;
	kobj->priv = priv;
	
	return kobj;
}
```

### 分配目录或文件节点
其为内部函数__kobj_alloc的二次封装，快速分配指定类型的节点。分配一个目录节点实现如下：
```c
struct kobj_t * kobj_alloc_directory(const char * name)
{
	return __kobj_alloc(name, KOBJ_TYPE_DIR, NULL, NULL, NULL);
}
```

分配一个文件节点实现如下：
```c
struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	return __kobj_alloc(name, KOBJ_TYPE_REG, read, write, priv);
}
```

### 释放kobj节点
将动态分配的内存进行回收，实现如下：
```c
bool_t kobj_free(struct kobj_t * kobj)
{
	if(!kobj)
		return FALSE;
	
	free(kobj->name);
	free(kobj);
	return TRUE;
}
```

### 搜索kobj节点
从一个父节点中搜索一个名为name的子节点。
```c
struct kobj_t * kobj_search(struct kobj_t * parent, const char * name)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return NULL;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;
	
	if(!name)
		return NULL;
	
	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	
	return NULL;
}
```

### 搜索kobj节点，不存在则创建
与上一个函数类似，不同之处在于未搜索到名为name的子节点时，自动创建一个子节点，该节点为目录类型。
```c
struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return NULL;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;
	
	if(!name)
		return NULL;
	
	kobj = kobj_search(parent, name);
	if(!kobj)
	{
		kobj = kobj_alloc_directory(name);
		if(!kobj)
			return NULL;
	
		if(!kobj_add(parent, kobj))
		{
			kobj_free(kobj);
			return NULL;
		}
	}
	else if(kobj->type != KOBJ_TYPE_DIR)
	{
		return NULL;
	}
	
	return kobj;
}
```

### 添加kobj节点
将一个节点，可以是目录节点或文件节点，添加至父目录节点，成功返回真，否则返回假。
```c
bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj)
{
	if(!parent)
		return FALSE;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;
	
	if(!kobj)
		return FALSE;
	
	if(kobj_search(parent, kobj->name))
		return FALSE;
	
	spin_lock_irq(&parent->lock);
	spin_lock_irq(&kobj->lock);
	
	kobj->parent = parent;
	list_add_tail(&kobj->entry, &parent->children);
	
	spin_unlock_irq(&kobj->lock);
	spin_unlock_irq(&parent->lock);
	
	return TRUE;
}
```

### 删除kobj节点
从一个父目录节点中删除一个子节点，为“添加kobj节点”的逆过程。同样，成功返回真，否则返回假。
```c
bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return FALSE;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;
	
	if(!kobj)
		return FALSE;
	
	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(pos == kobj)
		{
			spin_lock_irq(&parent->lock);
			spin_lock_irq(&kobj->lock);
	
			pos->parent = pos;
			list_del(&(pos->entry));
	
			spin_unlock_irq(&kobj->lock);
			spin_unlock_irq(&parent->lock);
	
			return TRUE;
		}
	}
	
	return FALSE;
}
```

### 添加目录节点或文件节点
此接口实现的目的是为了快速添加子节点，其根据所传递的参数，自动创建节点并添加到父节点上，添加子目录节点：
```c
bool_t kobj_add_directory(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;
	
	if(!name)
		return FALSE;
	
	if(kobj_search(parent, name))
		return FALSE;
	
	kobj = kobj_alloc_directory(name);
	if(!kobj)
		return FALSE;
	
	if(!kobj_add(parent, kobj))
		kobj_free(kobj);
	
	return TRUE;
}
```

添加子文件节点：
```c
bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;
	
	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;
	
	if(!name)
		return FALSE;
	
	if(kobj_search(parent, name))
		return FALSE;
	
	kobj = kobj_alloc_regular(name, read, write, priv);
	if(!kobj)
		return FALSE;
	
	if(!kobj_add(parent, kobj))
		kobj_free(kobj);
	
	return TRUE;
}
```

### 递归删除节点
此接口会将自身及其所以子节点以递归方式删除：
```c
bool_t kobj_remove_self(struct kobj_t * kobj)
{
	struct kobj_t * parent;
	struct kobj_t * pos, * n;
	bool_t ret;
	
	if(!kobj)
		return FALSE;
	
	if(kobj->type == KOBJ_TYPE_DIR)
	{
		list_for_each_entry_safe(pos, n, &(kobj->children), entry)
		{
			kobj_remove_self(pos);
		}
	}
	
	parent = kobj->parent;
	if(parent && (parent != kobj))
	{
		ret = kobj_remove(parent, kobj);
		if(ret)
			kobj_free(kobj);
		return ret;
	}
	
	kobj_free(kobj);
	return TRUE;
}
```

### 根节点
在获取根节点时，如果不存在，则会自动创建一个名为`kobj`的根节点，该节点为全局静态变量，同时也是一个`sysfs`的顶层目录节点，在`mount`文件系统时会挂载到`sys`目录。
```c
struct kobj_t * kobj_get_root(void)
{
	if(!__kobj_root)
		__kobj_root = kobj_alloc_directory("kobj");
	return __kobj_root;
}
```

### 总结
对于如何使用kobj,请参见驱动模块里的实现，每个驱动模块都含有一个kobj对象，在注册设备时，会依据设备的名称及类型自动创建一组kobj对象。如何通过文件系统访问kobj，则参考虚拟文件系统中的sysfs文件系统的实现。综上所述，kobj API归纳如下：
```c
struct kobj_t * kobj_get_root(void);
struct kobj_t * kobj_search(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_alloc_directory(const char * name);
struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_free(struct kobj_t * kobj);
bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_add_directory(struct kobj_t * parent, const char * name);
bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_remove_self(struct kobj_t * kobj);
```

## 设备树技术源码解析

驱动与设备的关系，可以类比为函数与变量的关系，函数负责具体的执行动作，变量负责描述自身属性，两者之间是互相依存，缺一不可，通常情况下，都是一对一的关系，即一个驱动对应一个设备，但更多的时候是一对多的关系，即一个驱动对应好几个设备。比如，一颗主控芯片，集成了四个串行端口，这个时候，你不可能写四份驱动然后分别注册设备，这样显然很冗余。通常都是只写一份驱动，然后用变量描述之间的差异，分别执行四次，达到注册四个设备的目的。实际操作中，直接用变量来描述设备的属性，也显得灵活性不够。每添加一个设备，不仅仅面临重新编译的问题，而且代码可读性还比较差。在这个时候，设备树应运而生，我们只需将所有的设备用文本的方式描述好，然后直接解析这个文件，依据文件的内容直接生成相应的设备。

### 引言

设备树本质就是一个配置文件，描述了所有设备的属性。JSON作为一种轻量级的数据交换格式，简洁的层次结构使得其成为理想的配置文件，易于人们编写和阅读，同时也易于机器生成和解析。选择JSON作为设备树的存储格式再合适不过了。

### JSON语法

#### 两种结构

- “名称/值”对的集合（A collection of name/value pairs）。不同的语言中，它被理解为*对象（object）*，纪录（record），结构（struct），字典（dictionary），哈希表（hash table），有键列表（keyed list），或者关联数组 （associative array）。
- 值的有序列表（An ordered list of values）。在大部分语言中，它被理解为数组（array）。

这些都是常见的数据结构。事实上大部分现代计算机语言都以某种形式支持它们。这使得一种数据格式在同样基于这些结构的编程语言之间交换成为可能。

#### 五种形式

对象是一个无序的“‘名称/值’对”集合。一个对象以“{”（左括号）开始，“}”（右括号）结束。每个“名称”后跟一个“:”（冒号）；“‘名称/值’ 对”之间使用“,”（逗号）分隔。

![json-object](https://github.com/xboot/xboot/raw/master/documents/images/json-object.gif)

数组是值（value）的有序集合。一个数组以“[”（左中括号）开始，“]”（右中括号）结束。值之间使用“,”（逗号）分隔。

![json-array](https://github.com/xboot/xboot/raw/master/documents/images/json-array.gif)

值（*value*）可以是双引号括起来的字符串（*string*）、数值(number)、`true`、`false`、 `null`、对象（object）或者数组（array）。这些结构可以嵌套。

![json-value](https://github.com/xboot/xboot/raw/master/documents/images/json-value.gif)

字符串（*string*）是由双引号包围的任意数量Unicode字符的集合，使用反斜线转义。一个字符（character）即一个单独的字符串（character string）。

字符串（*string*）与C或者Java的字符串非常相似。

![json-string](https://github.com/xboot/xboot/raw/master/documents/images/json-string.gif)

数值（*number*）也与C或者Java的数值非常相似。除去未曾使用的八进制与十六进制格式。除去一些编码细节。

![json-number](https://github.com/xboot/xboot/raw/master/documents/images/json-number.gif)

### 设备节点dtnode结构体定义

XBOOT规定了设备树中描述的每一个设备节点都是一个json对象，对象里面可以包含各种形式的键值对。每个设备节点包含如下关键信息：

- 设备节点名字
- 设备自动分配起始索引或者设备物理地址
- 具体的json对象

​```c
struct dtnode_t {
	const char * name;
	physical_addr_t addr;
	json_value * value;
};
```

### 获取设备节点信息

为了快速获取设备节点信息，这里提供了一组快捷操作函数，直接返回设备节点结构体相关信息。下面列举了两个设备节点描述，一个LED灯，没有设备物理地址，仅有设备分配起始索引，另一个PL011串行端口控制器，提供了设备物理地址。设备名称与索引或物理地址之间以`@`分隔，并表现为`object`对象，如下所示：

```json
	"led-gpio@0": {
		"gpio": 0,
		"active-low": true,
		"default-brightness": 0
	},

	"uart-pl011@0x10009000": {
		"clock-name": "uclk",
		"txd-gpio": -1,
		"txd-gpio-config": -1,
		"rxd-gpio": -1,
		"rxd-gpio-config": -1,
		"baud-rates": 115200,
		"data-bits": 8,
		"parity-bits": 0,
		"stop-bits": 1
	}
```

#### 获取设备节点名称
即获取`@`左侧部分，这个节点名称，就是所对应的驱动名称，在依据设备树添加添备时，会自动匹配同名驱动。写在设备树前面的的设备节点先匹配，写在后面的设备节点后匹配，这里提供了一套优先级机制，解决设备间互相依赖的问题。如果一个设备依赖于另外一个设备，那么此设备的设备节点，必须写在依赖设备的后面，如果顺序颠倒，在注册设备时，会因找不到依赖设备，而出现注册失败的现象。一般的，写在最前面的，都是比较底层的驱动设备，比如`clk`、`irq`、`gpio`等，写在后面的，都是比较高等级的设备，比如`framebuffer`等。

```c
const char * dt_read_name(struct dtnode_t * n)
{
	return n ? n->name : NULL;
}
```

#### 获取设备自动分配起始索引
即获取`@`右侧部分，数值类型。起始索引ID，主要用于同一个驱动在注册多个设备时，可以手动指定设备尾缀，以`.0`、`.1`、`.2`等形式存在，在注册设备时，如果该设备尾缀已经被占用，则自动加一，直到找到空闲尾缀为止。如果设备节点没有提供`@`右侧部分，自动从`.0`开始。

```c
int dt_read_id(struct dtnode_t * n)
{
	return n ? (int)n->addr : 0;
}
```

#### 获取设备物理地址
这个函数具体的实现跟上面的获取自动分配起始索引几乎一模一样，唯一的差异，就是返回值的类型。设备节点存在两种形态，一种带有设备物理地址的，还有一种没有的，比如上面的`PL011`与`LED`。这两种形态在描述设备时不加以区分，仅在注册设备时，驱动才显式的调用对应的方法，以获取设备信息。

```c
physical_addr_t dt_read_address(struct dtnode_t * n)
{
	return n ? n->addr : 0;
}
```

### 访问设备节点对象

设备节点对象包含各种形式的键值对，包括布尔逻辑、整形、浮点、字符串、对象，数组。每个具体的实现函数，都提供了默认值参数，如果找不到该键值对，就返回传递的默认值参数。

#### 获取布尔逻辑

```c
int dt_read_bool(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_boolean))
					return v->u.boolean ? 1 : 0;
			}
		}
	}
	return def;
}
```

#### 获取整型数据

```c
int dt_read_int(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (int)v->u.integer;
			}
		}
	}
	return def;
}
```

#### 获取长整型数据

```c
long long dt_read_long(struct dtnode_t * n, const char * name, long long def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (long long)v->u.integer;
			}
		}
	}
	return def;
}
```

#### 获取浮点型数据

```c
double dt_read_double(struct dtnode_t * n, const char * name, double def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_double))
					return (double)v->u.dbl;
			}
		}
	}
	return def;
}
```

#### 获取字符串

```c
char * dt_read_string(struct dtnode_t * n, const char * name, char * def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_string))
					return (char *)v->u.string.ptr;
			}
		}
	}
	return def;
}
```

#### 获取json对象

```c
struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name, struct dtnode_t * o)
{
	json_value * v;
	int i;

	if(o && n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_object))
				{
					o->name = name;
					o->addr = 0;
					o->value = v;
					return o;
				}
			}
		}
	}
	return NULL;
}
```

#### 获取数组长度

```c
int dt_read_array_length(struct dtnode_t * n, const char * name)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
					return v->u.array.length;
			}
		}
	}
	return 0;
}
```

#### 获取数组中的布尔逻辑

```c
int dt_read_array_bool(struct dtnode_t * n, const char * name, int idx, int def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_boolean))
							return e->u.boolean ? 1 : 0;
					}
				}
			}
		}
	}
	return def;
}
```

#### 获取数组中的整型数据

```c
int dt_read_array_int(struct dtnode_t * n, const char * name, int idx, int def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (int)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}
```

#### 获取数组中的长整型数据

```c
long long dt_read_array_long(struct dtnode_t * n, const char * name, int idx, long long def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (long long)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}
```

#### 获取数组中的浮点型数据

```c
double dt_read_array_double(struct dtnode_t * n, const char * name, int idx, double def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_double))
							return (double)e->u.dbl;
					}
				}
			}
		}
	}
	return def;
}

```

#### 获取数组中的字符串

```c
char * dt_read_array_string(struct dtnode_t * n, const char * name, int idx, char * def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_string))
							return (char *)e->u.string.ptr;
					}
				}
			}
		}
	}
	return def;
}
```

#### 获取数组中的json对象

```c
struct dtnode_t * dt_read_array_object(struct dtnode_t * n, const char * name, int idx, struct dtnode_t * o)
{
	json_value * v, * e;
	int i;

	if(o && n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_object))
						{
							o->name = 0;
							o->addr = 0;
							o->value = e;
							return o;
						}
					}
				}
			}
		}
	}
	return NULL;
}
```

### 总结
一颗设备树就是一个以`.json`为后缀的文件，一个系统里可以提供多个设备树，以文件名相区分，每个文件名对应一个`machine`。在提供多个设备树后，同时也注册了多个`machine`，系统在启动时，会自动遍历所有`machine`，如果某个`machine`检测通过，则会依据名称加载对应的设备树文件。利用这套机制，可以实现一个镜像支持若干平台，前提是这些平台提供了硬件版本号，且软件能够正确获取并加以区分。
设备树操作API，如何使用请参考各种驱动，归纳如下：

```c
const char * dt_read_name(struct dtnode_t * n);
int dt_read_id(struct dtnode_t * n);
physical_addr_t dt_read_address(struct dtnode_t * n);
int dt_read_bool(struct dtnode_t * n, const char * name, int def);
int dt_read_int(struct dtnode_t * n, const char * name, int def);
long long dt_read_long(struct dtnode_t * n, const char * name, long long def);
double dt_read_double(struct dtnode_t * n, const char * name, double def);
char * dt_read_string(struct dtnode_t * n, const char * name, char * def)
struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name, struct dtnode_t * o);
int dt_read_array_length(struct dtnode_t * n, const char * name);
int dt_read_array_bool(struct dtnode_t * n, const char * name, int idx, int def);
int dt_read_array_int(struct dtnode_t * n, const char * name, int idx, int def);
long long dt_read_array_long(struct dtnode_t * n, const char * name, int idx, long long def);
double dt_read_array_double(struct dtnode_t * n, const char * name, int idx, double def);
char * dt_read_array_string(struct dtnode_t * n, const char * name, int idx, char * def);
struct dtnode_t * dt_read_array_object(struct dtnode_t * n, const char * name, int idx, struct dtnode_t * o);
```
