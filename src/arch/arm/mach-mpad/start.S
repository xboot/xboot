/*
 * start.S
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

.equ	USR_MODE,		0x10	/* normal user mode */
.equ 	FIQ_MODE,		0x11	/* fiq fast interrupts mode */
.equ 	IRQ_MODE,		0x12	/* irq standard interrupts mode */
.equ 	SVC_MODE,		0x13	/* supervisor interrupts mode */
.equ 	ABT_MODE,		0x17	/* abort processing memory faults mode */
.equ 	UDF_MODE,		0x1b	/* undefined instructions mode */
.equ 	MODE_MASK,		0x1f	/* system running in priviledged operating mode */
.equ 	NO_INT,			0xc0
.equ    CPSR_IRQ_EN,	0x80
.equ    CPSR_IRQ_MASK,	0x40	/* disable interrupt mode (irq) */
.equ    CPSR_FIQ_MASK,	0x80	/* disable fast interrupt mode (fiq) */

/*
 * exception vector table
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
	.word 0x4000
	.word 0x0
	.word 0x0
	.word 0x0

	.global	_start
_start:

/* 0x00: reset */
	b	reset

/* 0x04: undefined instruction exception */
	ldr	pc, _undefined_instruction

/* 0x08: software interrupt exception */
	ldr	pc, _software_interrupt

/* 0x0c: prefetch abort */
	ldr	pc, _prefetch_abort

/* 0x10: data access memory abort */
	ldr	pc, _data_abort

/* 0x14: not used */
	ldr	pc, _not_used

/* 0x18: interrupt request exception */
	ldr	pc, _irq

/* 0x1c: fast interrupt request exception */
	ldr	pc, _fiq


_undefined_instruction:
	.long undefined_instruction
_software_interrupt:
	.long software_interrupt
_prefetch_abort:
	.long prefetch_abort
_data_abort:
	.long data_abort
_not_used:
	.long not_used
_irq:
	.long irq
_fiq:
	.long fiq

/*
 * xboot magics
 */
/* magic number so we can verify that we only put */
	.long	0x11223344, 0x11223344, 0x11223344, 0x11223344
/* where the xboot was linked */
	.long	_start
/* platform, cpu or machine id */
	.long   0xdeadbeef
/* xboot check sum */
	.byte   0, 0, 0, 0

/*
 * the actual reset code
 */
reset:
	/* disable watchdog */
	ldr	r0, =0xe2700000
	mov	r1, #0
	str	r1, [r0]

	/* set the cpu to supervisor mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

	/* disable l2 cache */
	mrc p15, 0, r0, c1, c0, 1
	bic r0, r0, #(1<<1)
	mcr p15, 0, r0, c1, c0, 1

	/* set l2 cache aux ctrl cycle */
	mrc p15, 1, r0, c9, c0, 2
	bic r0, r0, #(0x1<<29)
	bic r0, r0, #(0x1<<21)
	bic r0, r0, #(0x7<<6)
	bic r0, r0, #(0x7<<0)
	mcr p15, 1, r0, c9, c0, 2

	/* enable l2 cache */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, r0, #(1<<1)
	mcr p15, 0, r0, c1, c0, 1

	/* invalidate l1 i/d */
	mov	r0, #0						/*  set up for mcr */
	mcr	p15, 0, r0, c8, c7, 0		/* invalidate tlbs */
	mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */

	/* disable mmu stuff and caches */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000			/* clear bits 13 (--v-) */
	bic	r0, r0, #0x00000007			/* clear bits 2:0 (-cam) */
	orr	r0, r0, #0x00000002			/* set bit 1 (--a-) align */
	orr	r0, r0, #0x00000800			/* set bit 12 (z---) btb */
	mcr	p15, 0, r0, c1, c0, 0

	/* io retention release */
	ldr	r0, =0xe010e000
	ldr	r1, [r0]
	ldr	r2, =((1 << 31) | (1 << 29) | (1 << 28))
	orr	r1, r1, r2
	str	r1, [r0]

	/* ps_hold pin (gph0_0) set to high */
	ldr	r0, =0xe010e81c
	ldr	r1, [r0]
	orr	r1, r1, #0x300
	orr	r1, r1, #0x1
	str	r1, [r0]

	/* init system clock */
	bl	system_clock_init

	/* initialize memory controller */
	bl	mem_ctrl_init

	/* initialize nand flash controller */
	bl	nand_init

	/* setting trustzone protection controller */
	bl	tzpc_init

	/* wakeup or not */
	ldr	r0, =(0xe0100000 + 0xa000)
	ldr	r1, [r0]
	bic	r1, r1, #0xfff6ffff
	cmp	r1, #0x10000
	beq	wakeup_reset
	cmp	r1, #0x80000
	beq	wakeup_reset

	/* copyself to ram using irom */
	adr	r0, _start
	ldr r1, =_start
	cmp	r0, r1
	beq	have_copyed
	bl	irom_copyself
have_copyed:
	nop

	/* initialize stacks */
	bl	init_stacks

	/* copy shadow of data section */
copy_shadow_data:
	ldr	r0, _data_shadow_start
	ldr	r1, _data_start
	ldr	r2, _data_shadow_end
	bl	mem_copy

	/* clear bss section */
clear_bss:
	ldr	r0, _bss_start
	ldr	r1, _bss_end
	mov r2, #0x00000000
	bl	mem_clear

	/* jump to ram */
	ldr	r1, =on_the_ram
	mov	pc, r1
on_the_ram:
	/* jump to xboot_main fuction */
	mov r0, #1;
	mov r1, #0;
	bl	xboot_main		/* call xboot's main function */
	b	on_the_ram

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

/*
 * initialize nand flash controller
 */
nand_init:
	ldr	r0, =0xE020000				/* GPIO base register */
	
	ldr	r1, [r0, #0x02E0]			/* MP0_1CON */
	bic	r1, r1, #(0xf<<8)
	orr	r1, r1, #(0x3<<8)
	str	r1, [r0, #0x02E0]

	ldr	r1, [r0, #0x02E8]			/* MP0_1PUD */
	bic	r1, r1, #(0x3<<4)
	str	r1, [r0, #0x02E8]

	ldr	r1, [r0, #0x0320]			/* MP0_3CON */
	bic	r1, r1, #0xFFFFFF
	ldr	r2, =0x22222222
	orr	r1, r1, r2
	str	r1, [r0, #0x0320]

	ldr	r1, [r0, #0x0328]			/* MP0_3PUD */
	ldr	r2, =0x3fff
	bic	r1, r1, r2
	str	r1, [r0, #0x0328]

	ldr	r0, =0xB0E00000				/* Nand flash controller register */

	ldr	r1, [r0, #0x0000]			/* NFCONF */
	ldr	r2, =0x777F
	bic	r1, r1, r2
	ldr	r2, =(7<<12)|(7<<8)|(7<<4)|(0<<3)|(1<<2)|(1<<1)|(0<<0)
	orr	r1, r1, r2
	str	r1, [r0, #0x0000]

	ldr	r1, [r0, #0x0004]			/* NFCONT */
	ldr	r2, =0x707C7
	bic	r1, r1, r2
	ldr	r2, =(0<<18)|(0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0x3<<1)|(1<<0)
	orr	r1, r1, r2
	str	r1, [r0, #0x0004]

	ldr	r1, [r0, #0x00000]			/* NFCONF */
	orr	r1, r1, #0x70
	orr	r1, r1, #0x7700
	str	r1, [r0, #0x0000]

	ldr	r1, [r0, #0x0004]			/* NFCONT */
	orr	r1, r1, #0x03
	str	r1, [r0, #0x0004]

	mov	pc, lr

/*
 * setting trustzone protection controller
 */
tzpc_init:
	ldr	r0, =0xF1500000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]

	ldr 	r0, =0xFAD00000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]

	ldr	r0, =0xE0600000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]
	str	r1, [r0, #0x828]

	ldr	r0, =0xE1C00000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]

	mov	pc, lr

/*
 * initialize stacks
 */
init_stacks:
	mrs	r0, cpsr
	bic	r0, r0, #MODE_MASK | NO_INT
	orr	r1, r0, #UDF_MODE
	msr	cpsr_cxsf, r1
	ldr	sp, _stack_und_end

	bic	r0, r0, #MODE_MASK | NO_INT
	orr	r1, r0, #ABT_MODE
	msr	cpsr_cxsf, r1
	ldr	sp, _stack_abt_end

	bic	r0, r0, #MODE_MASK | NO_INT
	orr	r1, r0, #IRQ_MODE
	msr	cpsr_cxsf, r1
	ldr	sp, _stack_irq_end

	bic	r0, r0, #MODE_MASK | NO_INT
	orr	r1, r0, #FIQ_MODE
	msr	cpsr_cxsf, r1
	ldr	sp, _stack_fiq_end

	bic	r0, r0, #MODE_MASK | NO_INT
	orr	r1, r0, #SVC_MODE
	msr	cpsr_cxsf, r1
	ldr	sp, _stack_srv_end
	mov	pc, lr

/*
 * memory copy
 */
mem_copy:
	sub	r2, r2, #32
	cmp	r0, r2
	ble	3f
1:	ldmia r0!, {r3-r10}
	stmia r1!, {r3-r10}
	cmp	r0, r2
	ble	1b
3:	add	r2, r2, #32
2:	ldr	r3, [r0], #4
	str	r3, [r1], #4
	cmp	r0, r2
	blt	2b
	mov	pc, lr

/*
 * memory clear zero
 */
mem_clear:
	sub	r1, r1, #32
	cmp	r0, r1
	ble	cp
	mov r3, #0
	mov r4, #0
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	mov r9, #0
	mov r10, #0
1:	stmia r0!, {r3-r10}
	cmp	r0, r1
	ble	1b
cp:	add	r1, r1, #32
2:	str	r2, [r0], #4
	cmp	r0, r1
	blt	2b
	mov	pc, lr

/*
 * wakeup reset
 */
wakeup_reset:
	/*
	 * load return address and jump to kernel
	 */
	ldr	r0, =(0xe010f000 + 0x00)
	ldr	r1, [r0]
	mov	pc, r1
	nop
	nop

	b	.

/*
 * exception handlers
 */
undefined_instruction:
	b	.

software_interrupt:
	b	.

prefetch_abort:
	b	.

data_abort:
	b	.

not_used:
	b	.

	.global	irq
irq:
	/* get irq's sp */
	ldr	sp, _stack_irq_end

	/* save user regs */
	sub	sp, sp, #72
	stmia sp, {r0 - r12}			/* calling r0-r12 */
	add r8, sp, #60
	stmdb r8, {sp, lr}^				/* calling sp, lr */
	str lr, [r8, #0]				/* save calling pc */
	mrs r6, spsr
	str r6, [r8, #4]				/* save cpsr */
	str r0, [r8, #8]				/* save old_r0 */
	mov	r0, sp

	/* do irqs routlines */
	bl 	do_irqs

	/* restore user regs */
	ldmia sp, {r0 - lr}^			/* calling r0 - lr */
	mov	r0, r0
	ldr	lr, [sp, #60]				/* get pc */
	add	sp, sp, #72
	subs pc, lr, #4					/* return & move spsr_svc into cpsr */

fiq:
	b	.

/*
 * the location of stacks
 */
 	.align 4
_stack_fiq_end:
	.long	__stack_fiq_end
_stack_irq_end:
	.long	__stack_irq_end
_stack_abt_end:
	.long	__stack_abt_end
_stack_und_end:
	.long	__stack_und_end
_stack_srv_end:
	.long	__stack_srv_end

/*
 * the location of section
 */
 	.align 4
_text_start:
	.long	__text_start
_text_end:
	.long	__text_end
_romdisk_start:
	.long	__romdisk_start
_romdisk_end:
	.long	__romdisk_end
_data_shadow_start:
	.long	 __data_shadow_start
_data_shadow_end:
	.long	 __data_shadow_end
_data_start:
	.long	__data_start
_data_end:
	.long	__data_end
_bss_start:
	.long	__bss_start
_bss_end:
	.long	__bss_end

.end
