/*
 * arch/arm/mach-mid560/start.S
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
	ldr	r0, =0x7e000000
	orr	r0, r0, #0x4000
	mov	r1, #0
	str	r1, [r0]

	/* set the cpu to supervisor mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

	/* flush v4 I/D caches */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 tlb */

	/* disable mmu stuff and caches */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300		/* clear bits 13, 9:8 (--V- --RS) */
	bic	r0, r0, #0x00000087		/* clear bits 7, 2:0 (B--- -CAM) */
	orr	r0, r0, #0x00000002		/* set bit 2 (A) align */
	orr	r0, r0, #0x00001000		/* set bit 12 (I) i-cache */
	mcr	p15, 0, r0, c1, c0, 0

	/* peri port setup */
	ldr	r0, =0x70000000
	orr	r0, r0, #0x13
	mcr	p15, 0, r0, c15, c2, 4	/* 256MB (0x70000000 - 0x7fffffff) */

	/* external interrupt pending clear */
	ldr	r0, =(0x7f008000+0x924)
	ldr	r1, [r0]
	str	r1, [r0]

	/* disable all interrupts (VIC0 and VIC1) */
	ldr	r0, =0x71200000
	ldr	r1, =0x71300000
	mvn	r3, #0x0
	str	r3, [r0, #0x14]
	str	r3, [r1, #0x14]

	/* set all interrupts as IRQ */
	mov	r3, #0x0
	str	r3, [r0, #0x0c]
	str	r3, [r1, #0x0c]

	/* pending interrupt clear */
	mov	r3, #0x0
	str	r3, [r0, #0xf00]
	str	r3, [r1, #0xf00]

	/* init system clock */
	bl system_clock_init

	/* initialize memory control */
	bl	mem_ctrl_init

	/* wakeup or not */
	ldr	r0, =(0x7e00f000 + 0x904)
	ldr	r1, [r0]
	bic	r1, r1, #0xfffffff7
	cmp	r1, #0x8
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
	ldr	r0, =0x7e00f000
	ldr	r1, [r0, #0x900]
	mov	r2, #0x40
	orr	r1, r1, r2
	str	r1, [r0, #0x900]

	nop
	nop
	nop
	nop
	nop

	ldr	r2, =0x80
	orr	r1, r1, r2
	str	r1, [r0, #0x900]

check_syncack:
	ldr	r1, [r0, #0x900]
	ldr	r2, =0xf00
	and	r1, r1, r2
	cmp	r1, #0xf00
	bne	check_syncack

	mov	r1, #0xff00
	orr	r1, r1, #0xff
	str	r1, [r0, #0x00]
	str	r1, [r0, #0x04]
	str	r1, [r0, #0x08]

	ldr   	r1, [r0, #0x28]
	bic	r1, r1, #0x70000
	orr	r1, r1, #0x30000
	str	r1, [r0, #0x28]

	ldr   	r1, [r0, #0x20]
	bic	r1, r1, #0x30000
	bic	r1, r1, #0xff00
	bic	r1, r1, #0xff
	ldr	r2, =CLK_DIV_VAL
	orr	r1, r1, r2
	str	r1, [r0, #0x20]

	ldr	r1, =APLL_VAL
	str	r1, [r0, #0x0C]
	ldr	r1, =MPLL_VAL
	str	r1, [r0, #0x10]

	ldr	r1, =0x80200203
	str	r1, [r0, #0x14]
	ldr	r1, =0x0
	str	r1, [r0, #0x18]
	ldr	r1, [r0, #0x1C]

	ldr	r2, =0x2007
	orr	r1, r1, r2
	str	r1, [r0, #0x1C]

	/* wait at least 200us to stablize all clock */
	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b

	/* synchronization for vic port */
	ldr	r1, [r0, #0x900]
	orr	r1, r1, #0x20
	str	r1, [r0, #0x900]

	mov	pc, lr

/*
 * memory controller initial.
 */
mem_ctrl_init:
	ldr	r0, =0x7e00f120
	mov	r1, #0xd
	str	r1, [r0]

	ldr	r0, =0x7e001000

	ldr	r1, =0x04
	str	r1, [r0, #0x04]

	ldr	r1, =DMC_DDR_REFRESH_PRD
	str	r1, [r0, #0x10]

	ldr	r1, =DMC_DDR_CAS_LATENCY
	str	r1, [r0, #0x14]

	ldr	r1, =DMC_DDR_t_DQSS
	str	r1, [r0, #0x18]

	ldr	r1, =DMC_DDR_t_MRD
	str	r1, [r0, #0x1C]

	ldr	r1, =DMC_DDR_t_RAS
	str	r1, [r0, #0x20]

	ldr	r1, =DMC_DDR_t_RC
	str	r1, [r0, #0x24]

	ldr	r1, =DMC_DDR_t_RCD
	ldr	r2, =DMC_DDR_schedule_RCD
	orr	r1, r1, r2
	str	r1, [r0, #0x28]

	ldr	r1, =DMC_DDR_t_RFC
	ldr	r2, =DMC_DDR_schedule_RFC
	orr	r1, r1, r2
	str	r1, [r0, #0x2C]

	ldr	r1, =DMC_DDR_t_RP
	ldr	r2, =DMC_DDR_schedule_RP
	orr	r1, r1, r2
	str	r1, [r0, #0x30]

	ldr	r1, =DMC_DDR_t_RRD
	str	r1, [r0, #0x34]

	ldr	r1, =DMC_DDR_t_WR
	str	r1, [r0, #0x38]

	ldr	r1, =DMC_DDR_t_WTR
	str	r1, [r0, #0x3C]

	ldr	r1, =DMC_DDR_t_XP
	str	r1, [r0, #0x40]

	ldr	r1, =DMC_DDR_t_XSR
	str	r1, [r0, #0x44]

	ldr	r1, =DMC_DDR_t_ESR
	str	r1, [r0, #0x48]

	ldr	r1, =DMC1_MEM_CFG
	str	r1, [r0, #0x0C]

	ldr	r1, =DMC1_MEM_CFG2
	str	r1, [r0, #0x4C]

	ldr	r1, =DMC1_CHIP0_CFG
	str	r1, [r0, #0x200]

	ldr	r1, =DMC_DDR_32_CFG
	str	r1, [r0, #0x304]

	/* dmc0 ddr chip 0 configuration direct command reg */
	ldr	r1, =DMC_NOP0
	str	r1, [r0, #0x08]

	/* precharge all */
	ldr	r1, =DMC_PA0
	str	r1, [r0, #0x08]

	/* auto refresh 2 time */
	ldr	r1, =DMC_AR0
	str	r1, [r0, #0x08]
	str	r1, [r0, #0x08]

	/* mrs */
	ldr	r1, =DMC_mDDR_EMR0
	str	r1, [r0, #0x08]

	/* mode reg */
	ldr	r1, =DMC_mDDR_MR0
	str	r1, [r0, #0x08]

	/* enable dmc1 */
	mov	r1, #0x0
	str	r1, [r0, #0x04]

check_dmc1_ready:
	ldr	r1, [r0, #0x00]
	mov	r2, #0x3
	and	r1, r1, r2
	cmp	r1, #0x1
	bne	check_dmc1_ready
	nop
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
	 * clear wakeup status register
	 */
	ldr	r0, =(0x7e00f000 + 0x908)
	ldr	r1, [r0]
	str	r1, [r0]

	/*
	 * load return address and jump to kernel
	 */
	ldr	r0, =(0x7e00f000 + 0xa00)
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
