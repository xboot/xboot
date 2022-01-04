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

