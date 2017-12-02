/*
 * start.S
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

.include "syscfg.inc"

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

/*
 * bl1 header infomation for irom
 *
 * 0x0 - bl1 size
 * 0x4 - reserved (should be 0)
 * 0x8 - check sum
 * 0xc - reserved (should be 0)
 */
	.word 0x2000
	.word 0x0
	.word 0x0
	.word 0x0

	.global	_start
_start:
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
	/* Set cpu to svc32 mode and disable interrupt */
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	msr cpsr, r0

	/* Enable neon/vfp unit */
	mrc p15, #0, r1, c1, c0, #2
	orr r1, r1, #(0xf << 20)
	mcr p15, #0, r1, c1, c0, #2
	mov r1, #0
	mcr p15, #0, r1, c7, c5, #4
	mov r0, #0x40000000
	fmxr fpexc, r0

	/* Disable watchdog */
	ldr r0, =0xe2700000
	mov r1, #0
	str r1, [r0]

	/* Release retention IO */
	ldr r0, =0xe010e000
	ldr r1, [r0]
	ldr r2, =((1 << 31) | (1 << 29) | (1 << 28))
	orr r1, r1, r2
	str r1, [r0]

	/* Wakeup or not */
	ldr r0, =0xe010a000
	ldr r1, [r0]
	bic r1, r1, #0xfff6ffff
	cmp r1, #0x10000
	beq _wakeup
	cmp r1, #0x80000
	beq _wakeup

	/* init system clock */
	bl system_clock_init
	/* initialize memory controller */
	bl mem_ctrl_init

	/* Initial system tzpc, uart, clock, and ddr */
	bl sys_tzpc_init
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

/*
 * initialize core clock and bus clock.
 */
system_clock_init:
	ldr	r0, =0xe0100000

	mov	r1, #0xe00
	orr	r1, r1, #0x10
	str	r1, [r0, #0x00]
	str	r1, [r0, #0x04]
	str	r1, [r0, #0x08]
	str	r1, [r0, #0x0c]

	ldr r1, =CLK_DIV0_VAL
	str	r1, [r0, #0x300]

	ldr r1, =CLK_DIV1_VAL
	str	r1, [r0, #0x304]

	ldr r1, =CLK_DIV2_VAL
	str	r1, [r0, #0x308]

	ldr	r1, =APLL_VAL
	str	r1, [r0, #0x0100]

	ldr	r1, =MPLL_VAL
	str	r1, [r0, #0x0108]

	ldr	r1, =EPLL_VAL
	str	r1, [r0, #0x0110]

	ldr	r1, =VPLL_VAL
	str	r1, [r0, #0x0120]

	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b

	mov	pc, lr

/*
 * memory controller initial.
 */
mem_ctrl_init:
	ldr	r0, =0xf1e00000
	ldr	r1, =0x0
	str	r1, [r0, #0x0]

	/*
	 * dmc0 drive strength (setting 2x)
	 */
	ldr	r0, =0xe0200000

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x3cc]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x3ec]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x40c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x42c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x44c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x46c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x48c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x4ac]

	ldr	r1, =0x00002aaa
	str	r1, [r0, #0x4cc]

	/*
	 * dmc1 drive strength (setting 2x)
	 */
	ldr	r0, =0xe0200000

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x4ec]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x50c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x52c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x54c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x56C]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x58c]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x5ac]

	ldr	r1, =0x0000aaaa
	str	r1, [r0, #0x5cc]

	ldr	r1, =0x00002aaa
	str	r1, [r0, #0x5ec]

	/*
	 * dmc0 initialization at single type
	 */
	ldr	r0, =0xf0000000

	ldr	r1, =0x00101000				/* phycontrol0 dll parameter setting */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000086				/* phycontrol1 dll parameter setting */
	str	r1, [r0, #0x1c]

	ldr	r1, =0x00101002				/* phycontrol0 dll on */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00101003				/* phycontrol0 dll start */
	str	r1, [r0, #0x18]

dmc0_lock:
	ldr	r1, [r0, #0x40]				/* load phystatus register value */
	and	r2, r1, #0x7
	cmp	r2, #0x7					/* loop until dll is locked */
	bne	dmc0_lock

	and	r1, #0x3fc0
	mov	r2, r1, LSL #18
	orr	r2, r2, #0x100000
	orr	r2 ,r2, #0x1000

	orr	r1, r2, #0x3				/* force value locking */
	str	r1, [r0, #0x18]

	/* setting ddr2 */
	ldr	r1, =0x0FFF2010				/* concontrol auto refresh off */
	str	r1, [r0, #0x00]

	ldr	r1, =0x00212400				/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	ldr	r1, =DMC0_MEMCONFIG_0		/* memconfig0 256MB config, 8 banks, mapping method[12:15] 0:linear, 1:linterleaved, 2:mixed */
	str	r1, [r0, #0x08]

	ldr	r1, =DMC0_MEMCONFIG_1		/* memconfig1 */
	str	r1, [r0, #0x0c]

	ldr	r1, =0xFF000000				/* prech config */
	str	r1, [r0, #0x14]

	ldr	r1, =DMC0_TIMINGA_REF		/* timing aref */
	str	r1, [r0, #0x30]

	ldr	r1, =DMC0_TIMING_ROW		/* timingRow */
	str	r1, [r0, #0x34]

	ldr	r1, =DMC0_TIMING_DATA		/* timing data CL=3 */
	str	r1, [r0, #0x38]

	ldr	r1, =DMC0_TIMING_PWR		/* timing power */
	str	r1, [r0, #0x3c]

	ldr	r1, =0x07000000				/* chip0 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000				/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00020000				/* chip0 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00030000				/* chip0 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400				/* chip0 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000542				/* chip0 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000				/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000442				/* chip0 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010780				/* chip0 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400				/* chip0 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x07100000				/* chip1 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000				/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00120000				/* chip1 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00130000				/* chip1 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400				/* chip1 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100542				/* chip1 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000				/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100442				/* chip1 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110780				/* chip1 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400				/* chip1 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x0FF02030				/* concontrol auto refresh on */
	str	r1, [r0, #0x00]

	ldr	r1, =0xFFFF00FF				/* pwrdn config */
	str	r1, [r0, #0x28]

	ldr	r1, =0x00202400				/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	/*
	 * dmc1 initialization at single type
	 */
	ldr	r0, =0xf1400000

	ldr	r1, =0x00101000				/* phycontrol0 dll parameter setting */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000086				/* phycontrol1 dll parameter setting */
	str	r1, [r0, #0x1c]

	ldr	r1, =0x00101002				/* phycontrol0 dll on */
	str	r1, [r0, #0x18]

	ldr	r1, =0x00101003				/* phycontrol0 dll start */
	str	r1, [r0, #0x18]

dmc1_lock:
	ldr	r1, [r0, #0x40]				/* load phystatus register value */
	and	r2, r1, #0x7
	cmp	r2, #0x7					/* loop until dll is locked */
	bne	dmc1_lock

	and	r1, #0x3fc0
	mov	r2, r1, LSL #18
	orr	r2, r2, #0x100000
	orr	r2 ,r2, #0x1000

	orr	r1, r2, #0x3				/* force value locking */
	str	r1, [r0, #0x18]

	/* setting ddr2 */
	ldr	r1, =0x0FFF2010				/* concontrol auto refresh off */
	str	r1, [r0, #0x00]

	ldr	r1, =0x00212400				/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	ldr	r1, =DMC1_MEMCONFIG_0		/* memconfig0 256MB config, 8 banks, mapping method[12:15] 0:linear, 1:linterleaved, 2:mixed */
	str	r1, [r0, #0x08]

	ldr	r1, =DMC1_MEMCONFIG_1		/* memconfig1 */
	str	r1, [r0, #0x0c]

	ldr	r1, =0xFF000000				/* prech config */
	str	r1, [r0, #0x14]

	ldr	r1, =DMC1_TIMINGA_REF		/* timing aref */
	str	r1, [r0, #0x30]

	ldr	r1, =DMC1_TIMING_ROW		/* timingRow */
	str	r1, [r0, #0x34]

	ldr	r1, =DMC1_TIMING_DATA		/* timing data CL=3 */
	str	r1, [r0, #0x38]

	ldr	r1, =DMC1_TIMING_PWR		/* timing power */
	str	r1, [r0, #0x3c]

	ldr	r1, =0x07000000				/* chip0 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000				/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00020000				/* chip0 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00030000				/* chip0 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400				/* chip0 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000542				/* chip0 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000				/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000442				/* chip0 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010780				/* chip0 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00010400				/* chip0 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x07100000				/* chip1 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000				/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00120000				/* chip1 emrs2 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00130000				/* chip1 emrs3 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400				/* chip1 emrs1 (mem dll on, dqs disable) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100542				/* chip1 mrs (mem dll reset) cl=4, bl=4 */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000				/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100442				/* chip1 mrs (mem dll unreset) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110780				/* chip1 emrs1 (ocd default) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00110400				/* chip1 emrs1 (ocd exit) */
	str	r1, [r0, #0x10]

	ldr	r1, =0x0FF02030				/* concontrol auto refresh on */
	str	r1, [r0, #0x00]

	ldr	r1, =0xFFFF00FF				/* pwrdn config */
	str	r1, [r0, #0x28]

	ldr	r1, =0x00202400				/* memcontrol bl=4, 2 chip, ddr2 type, dynamic self refresh, force precharge, dynamic power down off */
	str	r1, [r0, #0x04]

	mov	pc, lr

_wakeup:
	ldr r0, =(0xe010f000 + 0x00)
	ldr r1, [r0]
	mov pc, r1
	b _wakeup

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
	.global irq
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
