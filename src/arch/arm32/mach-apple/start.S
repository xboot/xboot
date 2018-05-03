/*
 * start.S
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

.macro save_regs
	str lr, [sp, #-4]
	mrs lr, spsr_all
	str lr, [sp, #-8]
	str r1, [sp, #-12]
	str r0, [sp, #-16]
	mov r0, sp
	cps #0x13
	ldr r1, [r0, #-4]
	str r1, [sp, #-4]!
	ldr r1, [r0, #-8]
	str r1, [sp, #-(4*16)]
	ldr r1, [r0, #-12]
	ldr r0, [r0, #-16]
	stmdb sp, {r0-r14}^
	sub sp, sp, #(4*16)
	ldr r4, [sp]
	and r0, r4, #0x1f
	cmp r0, #0x10
	beq 10f
	cmp r0, #0x13
	beq 11f
	b .
11:	add r1, sp, #(4*17)
	str r1, [sp, #(4*14)]
	str lr, [sp, #(4*15)]
10:	add r1, sp, #(4*17)
	str r1, [sp, #-4]!
	mov r0, sp
.endm

.macro restore_regs
	mov r12, sp
	ldr sp, [r12], #4
	ldr r1, [r12], #4
	msr spsr_cxsf, r1
	and r0, r1, #0x1f
	cmp r0, #0x10
	beq 20f
	cmp r0, #0x13
	beq 21f
	b .
20:	ldr lr, [r12, #(4*15)]
	ldmia r12, {r0-r14}^
	movs pc, lr
21:	ldm r12, {r0-r15}^
	mov r0, r0
.endm

/*
 * Exception vector table
 */
.text
	.arm

	.global _start
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

	/* Set Vector Base Address Register */
	ldr r0, =_start
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0

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

	/* Copyself to link address */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq 1f
	ldr r0, _image_start
	adr r1, _start
	ldr r2, _image_end
	sub r2, r2, r0
	bl memcpy
1:	nop

	/* Copy data section */
	ldr r0, _data_start
	ldr r3, _image_start
	ldr r1, _data_shadow_start
	sub r1, r1, r3
	adr r3, _start
	add r1, r1 ,r3
	ldr r2, _data_shadow_start
	ldr r3, _data_shadow_end
	sub r2, r3, r2
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
	mov r0, #1
	mov r1, #0
	bl xboot_main
	b _main

/*
 * Exception handlers
 */
	.align 5
undefined_instruction:
	sub lr, lr, #4
	save_regs
	bl arm32_do_undefined_instruction
	restore_regs

	.align 5
software_interrupt:
	sub lr, lr, #4
	save_regs
	bl arm32_do_software_interrupt
	restore_regs

	.align 5
prefetch_abort:
	sub lr, lr, #4
	save_regs
	bl arm32_do_prefetch_abort
	restore_regs

	.align 5
data_abort:
	sub lr, lr, #8
	save_regs
	bl arm32_do_data_abort
	restore_regs

	.align 5
not_used:
	b .

	.align 5
	.global irq
irq:
	sub lr, lr, #4
	save_regs
	bl arm32_do_irq
	restore_regs

	.align 5
fiq:
	sub lr, lr, #4
	save_regs
	bl arm32_do_fiq
	restore_regs

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
