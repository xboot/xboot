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

  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case.md

* XBOOT系统开发指南

  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide.md

* XBOOT应用开发指南

  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide.md

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

在调试前，我们需要先修改`src\arch\arm32\mach-realview\xboot.mk`默认的编译参数，默认`O3`优化选项，编译时，会优化掉很多有用的符号信息，不利于调试，这里改成`O0`参数，禁止编译器做任何优化，在修改参数后，需要清除一下，再重新完整编译，具体修改如下图所示：

![eclipse-realview-o3-o0](https://github.com/xboot/xboot/raw/master/documents/images/eclipse-realview-o3-o0.png)

确认开发环境里是否已安装`GDB Hardware Debuging`插件，这个插件在`CDT`插件包里，如果没有安装，可以通过`Help` `Install new software`菜单来安装，如下图 所示：

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
make CROSS_COMPILE=/path/to/arm-linux-gnueabihf- PLATFORM=arm32-licheepi
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
│   │   │   ├── mach-h3
│   │   │   ├── mach-licheepi
│   │   │   ├── mach-n3ds
│   │   │   ├── mach-raspi2
│   │   │   ├── mach-realview
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
│   │   ├── fb
│   │   ├── gmeter
│   │   ├── gpio
│   │   ├── hygrometer
│   │   ├── i2c
│   │   ├── input
│   │   ├── interrupt
│   │   ├── laserscan
│   │   ├── led
│   │   ├── light
│   │   ├── pwm
│   │   ├── regulator
│   │   ├── reset
│   │   ├── rng
│   │   ├── rtc
│   │   ├── sd
│   │   ├── spi
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
| mach-h3       | 全志H3芯片，Nano PI                          |
| mach-licheepi | 全志V3S，Lichee PI Zero                    |
| mach-n3ds     | 任天堂3DS，包括后出的New 2DSXLL                  |
| mach-raspi2   | Raspberry PI 2代                         |
| mach-realview | Qemu Realview模拟器                        |
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

| 名称          | 描述       |
| ----------- | -------- |
| adc         | ADC驱动    |
| audio       | 音频驱动     |
| battery     | 电池电量驱动   |
| block       | 块设备驱动    |
| buzzer      | 蜂鸣器驱动    |
| clk         | 时钟驱动     |
| clockevent  | 时钟事件驱动   |
| clocksource | 时钟源驱动    |
| console     | 超级终端驱动   |
| dac         | DAC驱动    |
| dma         | DMA驱动    |
| fb          | 显示屏驱动    |
| gmeter      | 重力传感器驱动  |
| gpio        | GPIO驱动   |
| hygrometer  | 湿度传感器驱动  |
| i2c         | I2C总线驱动  |
| input       | 输入设备驱动   |
| interrupt   | 中断控制器驱动  |
| laserscan   | 激光振镜驱动   |
| led         | LED驱动    |
| light       | 光线传感器驱动  |
| pwm         | PWM驱动    |
| regulator   | 电压调节器驱动  |
| reset       | 复位驱动     |
| rng         | 随机数发生器驱动 |
| rtc         | 实时时钟驱动   |
| sd          | SD卡驱动    |
| spi         | SPI总线驱动  |
| thermometer | 温度传感器驱动  |
| uart        | 串口总线驱动   |
| vibrator    | 振动马达驱动   |
| watchdog    | 看门狗驱动    |

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
扩展文件系统，为上层应用软件提供统一的访问接口。

### lib
这个目录存放自行实现的各种底层库，包括`libc`、`libm`、`libx`等相关库函数。

### romdisk
这是编译进系统的romdisk目录，也就是xboot的根文件系统，存储为cpio格式，并在链接目标文件时一起打包。在系统运行时，会自动挂载到根目录作根文件系统。在板级支持包下，存在一个同名的目录，两者之间是overlay的关系，即板级支持包里的romdisk目录会覆盖默认系统源码目录下的romdisk。板级相关的文件，请放到板级支持包里的romdisk目录。

| 目录          | 描述          |
| ----------- | ----------- |
| application | 系统应用软件目录    |
| boot        | 设备树存放目录     |
| framework   | 核心组件        |
| private     | 私有目录        |
| storage     | 外部存储目录      |
| sys         | sys文件系统挂载目录 |

## tools
提供各种预编译好的二进制工具，包括QEMU模拟器，各种运行脚本等。

# 驱动开发

## ADC驱动

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

## PWM驱动

## 电压调节器驱动

## 复位驱动

## 随机数发生器驱动

## 实时时钟驱动

## SD卡驱动

## SPI总线驱动

## 温度传感器驱动

## 串口总线驱动

## 振动马达驱动

## 看门狗驱动

# 系统移植

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
系统启动时，会自动创建一个根节点，名为"kobj",该节点为一个全局静态变量，是顶层目录节点，在mount文件系统时会挂载到sysfs接口。
```c
void do_init_kobj(void)
{
	__kobj_root = kobj_alloc_directory("kobj");
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
