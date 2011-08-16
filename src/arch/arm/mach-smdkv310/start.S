/*
 * start.S
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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
	ldr	r0, =0x10060000
	mov	r1, #0
	str	r1, [r0]

	/* set the cpu to supervisor mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

	/* disable mmu stuff and caches */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000			/* clear bits 13 (--v-) */
	bic	r0, r0, #0x00000007			/* clear bits 2:0 (-cam) */
	orr	r0, r0, #0x00000002			/* set bit 1 (--a-) align */
	orr	r0, r0, #0x00000800			/* set bit 12 (z---) btb */
	mcr	p15, 0, r0, c1, c0, 0

	/* init system clock */
	bl system_clock_init

	/* initialize memory controller */
	bl	mem_ctrl_init

	/* setting trustzone protection controller */
	bl	tzpc_init

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
	push	{lr}
	ldr	r0, =0x10030000

	ldr	r1, =CLK_SRC_CPU_VAL
	ldr	r2, =0x14200
	str	r1, [r0, r2]

	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b

	ldr	r1, =CLK_DIV_CPU0_VAL
	ldr	r2, =0x14500
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_CPU1_VAL
	ldr	r2, =0x14504
	str	r1, [r0, r2]

	ldr	r1, =CLK_SRC_DMC_VAL
	ldr	r2, =0x10200
	str	r1, [r0, r2]

	mov	r1, #0x10000
2:	subs	r1, r1, #1
	bne	2b

	ldr	r1, =CLK_DIV_DMC0_VAL
	ldr	r2, =0x10500
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_DMC1_VAL
	ldr	r2, =0x10504
	str	r1, [r0, r2]

	ldr	r1, =CLK_SRC_TOP0_VAL
	ldr	r2, =0x0c210
	str	r1, [r0, r2]
	ldr	r1, =CLK_SRC_TOP1_VAL
	ldr	r2, =0x0c214
	str	r1, [r0, r2]

	mov	r1, #0x10000
3:	subs	r1, r1, #1
	bne	3b

	ldr	r1, =CLK_DIV_TOP_VAL
	ldr	r2, =0x0c510
	str	r1, [r0, r2]

	ldr	r1, =CLK_SRC_LEFTBUS_VAL
	ldr	r2, =0x04200
	str	r1, [r0, r2]

	mov	r1, #0x10000
4:	subs	r1, r1, #1
	bne	4b

	ldr	r1, =CLK_DIV_LEFRBUS_VAL
	ldr	r2, =0x04500
	str	r1, [r0, r2]

	ldr	r1, =CLK_SRC_RIGHTBUS_VAL
	ldr	r2, =0x08200
	str	r1, [r0, r2]

	mov	r1, #0x10000
5:	subs	r1, r1, #1
	bne	5b

	ldr	r1, =CLK_DIV_RIGHTBUS_VAL
	ldr	r2, =0x08500
	str	r1, [r0, r2]

	ldr	r1, =APLL_LOCK_VAL
	ldr	r2, =0x14000
	str	r1, [r0, r2]
	ldr	r1, =MPLL_LOCK_VAL
	ldr	r2, =0x14008
	str	r1, [r0, r2]
	ldr	r1, =EPLL_LOCK_VAL
	ldr	r2, =0x0c010
	str	r1, [r0, r2]
	ldr	r1, =VPLL_LOCK_VAL
	ldr	r2, =0x0c020
	str	r1, [r0, r2]

	ldr	r1, =APLL_CON1_VAL
	ldr	r2, =0x14104
	str	r1, [r0, r2]

	ldr	r1, =APLL_CON0_VAL
	ldr	r2, =0x14100
	str	r1, [r0, r2]

	ldr	r1, =MPLL_CON1_VAL
	ldr	r2, =0x1410c
	str	r1, [r0, r2]
	ldr	r1, =MPLL_CON0_VAL
	ldr	r2, =0x14108
	str	r1, [r0, r2]

	ldr	r1, =EPLL_CON1_VAL
	ldr	r2, =0x0c114
	str	r1, [r0, r2]
	ldr	r1, =EPLL_CON0_VAL
	ldr	r2, =0x0c110
	str	r1, [r0, r2]

	ldr	r1, =VPLL_CON1_VAL
	ldr	r2, =0x0c124
	str	r1, [r0, r2]
	ldr	r1, =VPLL_CON0_VAL
	ldr	r2, =0x0c120
	str	r1, [r0, r2]

	mov	r1, #0x30000
6:	subs	r1, r1, #1
	bne	6b

	pop	{pc}

/*
 * memory controller initial.
 */
mem_ctrl_init:
	/*
	 * Async bridge configuration at CPU_core:
	 * 1: half_sync
	 * 0: full_sync
	 */
	ldr r0, =0x10010350
	mov r1, #1
	str r1, [r0]

	ldr	r0, =0x10600000
.if (MIU_1BIT_12_INTERLEAVED == 1)
	ldr	r1, =0x0000000c
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_1BIT_7_INTERLEAVED == 1)
	ldr	r1, =0x00000007
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_21_12_INTERLEAVED == 1)
	ldr	r1, =0x2000150c
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_21_7_INTERLEAVED == 1)
	ldr	r1, =0x20001507
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_21_7_INTERLEAVED_DMC_1_2_DISABLE_BANK_INTERL == 1)
	ldr	r1, =0x20001507
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_21_7_INTERLEAVED_DMC_1_ENABLE_2_DISABLE_BANK_INTERL == 1)
	ldr	r1, =0x20001507
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_31_INTERLEAVED == 1)
	ldr	r1, =0x0000001d
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.elseif (MIU_2BIT_31_INTERLEAVED_DMC_1_2_DISABLE_BANK_INTERL == 1)
	ldr	r1, =0x0000001d
	str	r1, [r0, #0x400]
	ldr	r1, =0x40000000
	str	r1, [r0, #0x808]
	ldr	r1, =0xbfffffff
	str	r1, [r0, #0x810]
	ldr	r1, =0x00000001
	str	r1, [r0, #0x800]
.endif

	/* DREX0 */
	ldr	r0, =0x10400000

	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	ldr	r1, =0xE3855703
	str	r1, [r0, #0x44]

	mov	r2, #0x100000
1:	subs	r2, r2, #1
	bne	1b

	ldr	r1, =0xe000008e
	str	r1, [r0, #0x1C]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	ldr	r1, =0x71101008
	str	r1, [r0, #0x18]
	ldr	r1, =0x7110100A
	str	r1, [r0, #0x18]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]
	ldr	r1, =0x7110100B
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000000
	str	r1, [r0, #0x20]

	ldr	r1, =0x0fff301a
	str	r1, [r0, #0x00]
	ldr	r1, =0x00312640
	str	r1, [r0, #0x04]

	ldr	r1, =0x20e01323
	str	r1, [r0, #0x08]
	ldr	r1, =0x40e01323
	str	r1, [r0, #0x0C]

	ldr	r1, =0xff000000
	str	r1, [r0, #0x14]

	ldr	r1, =0x000000BB
	str	r1, [r0, #0x30]

.if (DRAM_CLK_330 == 1)
	ldr	r1, =0x3545548d
	str	r1, [r0, #0x34]
	ldr	r1, =0x45430506
	str	r1, [r0, #0x38]
	ldr	r1, =0x46000a3c
	str	r1, [r0, #0x3C]
.elseif (DRAM_CLK_400 == 1)
	ldr	r1, =0x4046654f
	str	r1, [r0, #0x34]
	ldr	r1, =0x46400506
	str	r1, [r0, #0x38]
	ldr	r1, =0x52000a3c
	str	r1, [r0, #0x3C]
.endif

	ldr	r1, =0x07000000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x00020000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00030000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00010002
	str	r1, [r0, #0x10]
	ldr	r1, =0x00000328
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x0a000000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x07100000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00120000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00130000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00110002
	str	r1, [r0, #0x10]
	ldr	r1, =0x00100328
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
6:	subs	r2, r2, #1
	bne	6b

	ldr	r1, =0x0a100000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
7:	subs	r2, r2, #1
	bne	7b

	ldr	r1, =0xe000008e
	str	r1, [r0, #0x1C]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	mov	r2, #0x100000
8:	subs	r2, r2, #1
	bne	8b

	ldr	r0, =0x10400000
	ldr	r1, =0x0FFF303A
	str	r1, [r0, #0x00]

	/* DREX1 */
	ldr	r0, =0x10410000

	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	ldr	r1, =0xE3855703
	str	r1, [r0, #0x44]

	mov	r2, #0x100000
1:	subs	r2, r2, #1
	bne	1b

	ldr	r1, =0xe000008e
	str	r1, [r0, #0x1C]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	ldr	r1, =0x71101008
	str	r1, [r0, #0x18]
	ldr	r1, =0x7110100A
	str	r1, [r0, #0x18]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]
	ldr	r1, =0x7110100B
	str	r1, [r0, #0x18]

	ldr	r1, =0x00000000
	str	r1, [r0, #0x20]

	ldr	r1, =0x0fff301a
	str	r1, [r0, #0x00]
	ldr	r1, =0x00312640
	str	r1, [r0, #0x04]

	ldr	r1, =0x20e01323
	str	r1, [r0, #0x08]
	ldr	r1, =0x40e01323
	str	r1, [r0, #0x0C]

	ldr	r1, =0xff000000
	str	r1, [r0, #0x14]

	ldr	r1, =0x000000BB
	str	r1, [r0, #0x30]

.if (DRAM_CLK_330 == 1)
	ldr	r1, =0x3545548d
	str	r1, [r0, #0x34]
	ldr	r1, =0x45430506
	str	r1, [r0, #0x38]
	ldr	r1, =0x46000a3c
	str	r1, [r0, #0x3C]
.elseif (DRAM_CLK_400 == 1)
	ldr	r1, =0x4046654f
	str	r1, [r0, #0x34]
	ldr	r1, =0x46400506
	str	r1, [r0, #0x38]
	ldr	r1, =0x52000a3c
	str	r1, [r0, #0x3C]
.endif

	ldr	r1, =0x07000000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x00020000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00030000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00010002
	str	r1, [r0, #0x10]
	ldr	r1, =0x00000328
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x0a000000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x07100000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00120000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00130000
	str	r1, [r0, #0x10]
	ldr	r1, =0x00110002
	str	r1, [r0, #0x10]
	ldr	r1, =0x00100328
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
6:	subs	r2, r2, #1
	bne	6b

	ldr	r1, =0x0a100000
	str	r1, [r0, #0x10]

	mov	r2, #0x100000
7:	subs	r2, r2, #1
	bne	7b

	ldr	r1, =0xe000008e
	str	r1, [r0, #0x1C]
	ldr	r1, =0xe0000086
	str	r1, [r0, #0x1C]

	mov	r2, #0x100000
8:	subs	r2, r2, #1
	bne	8b

	ldr	r0, =0x10410000
	ldr	r1, =0x0FFF303A
	str	r1, [r0, #0x00]

	mov	pc, lr

/*
 * setting trustzone protection controller
 */
tzpc_init:
	ldr	r0, =0x10110000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

	ldr	r0, =0x10120000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

	ldr	r0, =0x10130000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

	ldr	r0, =0x10140000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

	ldr	r0, =0x10150000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

	ldr	r0, =0x10160000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x0804]
	str	r1, [r0, #0x0810]
	str	r1, [r0, #0x081C]
	str	r1, [r0, #0x0828]

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
@	bl 	do_irqs

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
