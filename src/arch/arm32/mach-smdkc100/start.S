/*
 * arch/arm/mach-smdkc100/start.S
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
	.long   0x410fb760
/* xboot check sum */
	.byte   0, 0, 0, 0

/*
 * the actual reset code
 */
reset:
	/* disable watchdog */
	ldr	r0, =0xea200000
	mov	r1, #0
	str	r1, [r0]

	/* set the cpu to supervisor mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

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
	ldr	r0, =0xe0108200
	ldr	r1, [r0]
	ldr	r2, =(1 << 31)
	orr	r1, r1, r2
	str	r1, [r0]

	/* disable all interrupts (vic0, vic1 and vic2) */
	ldr	r0, =0xe4000000
	ldr	r1, =0xe4100000
	ldr	r2, =0xe4200000
	mvn	r3, #0x0
	str	r3, [r0, #0x14]
	str	r3, [r1, #0x14]
	str	r3, [r2, #0x14]

	/* set all interrupts as IRQ */
	mov	r3, #0x0
	str	r3, [r0, #0x0c]
	str	r3, [r1, #0x0c]
	str	r3, [r2, #0x0c]

	/* pending interrupt clear */
	mov	r3, #0x0
	str	r3, [r0, #0xf00]
	str	r3, [r1, #0xf00]
	str	r3, [r2, #0xf00]

	/* init system clock */
	bl system_clock_init

	/* initialize memory control */
	bl	mem_ctrl_init

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

	ldr	r1, =CLK_DIV1_VAL
	str	r1, [r0, #0x304]

	ldr	r1, =CLK_DIV2_VAL
	str	r1, [r0, #0x308]

	ldr	r1, =CLK_DIV3_VAL
	str	r1, [r0, #0x30c]

	ldr	r1, =CLK_DIV4_VAL
	str	r1, [r0, #0x310]

	ldr	r1, =APLL_VAL
	str	r1, [r0, #0x100]
	ldr	r1, =MPLL_VAL
	str	r1, [r0, #0x104]
	ldr	r1, =EPLL_VAL
	str	r1, [r0, #0x108]
	ldr	r1, =HPLL_VAL
	str	r1, [r0, #0x10c]

	ldr	r1, =CLK_SRC0_VAL
	str	r1, [r0, #0x200]

	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b

	mov	pc, lr

/*
 * memory controller initial.
 */
mem_ctrl_init:
	ldr	r0, =0xe6000000				/* apb dmc base */

	ldr	r1, =0x003b3b00				/* phycontrol0 dll parameter setting */
	str	r1, [r0, #0x18]

	ldr	r1,	=0x00000004				/* phycontrol1 dll parameter setting */
	str	r1, [r0, #0x1c]

	ldr	r1, =0x00000000				/* Phycontrol2 dll parameter setting */
	str	r1, [r0, #0x20]

	ldr	r1, =0x003b3b02				/* dll on */
	str	r1, [r0, #0x18]

	ldr	r1, =0x003b3b03				/* dll start */
	str	r1, [r0, #0x18]

	ldr	r1, =0x6a3b3b03				/* force value locking */
	str	r1, [r0, #0x18]

	ldr	r1, =0x6a3b3b01				/* dll off */
	str	r1, [r0, #0x18]

	ldr	r1, =0xff001010				/* auto refresh off */
	str	r1, [r0, #0x00]

	ldr	r1, =0xff212100				/* dll off */
	str	r1, [r0, #0x04]

	ldr	r1, =0x20f01322
	str	r1, [r0, #0x08]

	ldr	r1, =0x40f80312
	str	r1, [r0, #0x0c]

	ldr	r1,	=0x20000000
	str	r1,	[r0, #0x14]

	ldr	r1, =0x0000004e
	str	r1, [r0, #0x30]

	ldr	r1, =0x0c233287				/* timing row 133MHz */
	str	r1, [r0, #0x34]

	ldr	r1, =0x12130005
	str	r1, [r0, #0x38]

	ldr	r1, =0x0e120122				/* timing power*/
	str	r1, [r0, #0x3c]

	ldr	r1, =0x07000000				/* chip0 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01000000				/* chip0 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05000000				/* chip0 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00000032				/* chip0 mrs */
	str	r1, [r0, #0x10]

	ldr	r1, =0x07100000				/* chip1 deselect */
	str	r1, [r0, #0x10]

	ldr	r1, =0x01100000				/* chip1 pall */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x05100000				/* chip1 refa */
	str	r1, [r0, #0x10]

	ldr	r1, =0x00100032				/* chip1 mrs */
	str	r1, [r0, #0x10]

	ldr	r1, =0xff001030				/* concontrol auto refresh on */
	str	r1, [r0, #0x00]

	ldr	r1, =0x00100002				/* powerdown config */
	str	r1, [r0, #0x28]

	ldr	r1, =0xff212100				/* memory control */
	str	r1, [r0, #0x04]

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
