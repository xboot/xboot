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

#include <xconfigs.h>
#include <linkage.h>

.macro ventry label
	.align 7
	b \label
.endm

.macro save_regs
	sub sp, sp, #48
	stp x28, x29, [sp, #-16]!
	stp x26, x27, [sp, #-16]!
	stp x24, x25, [sp, #-16]!
	stp x22, x23, [sp, #-16]!
	stp x20, x21, [sp, #-16]!
	stp x18, x19, [sp, #-16]!
	stp x16, x17, [sp, #-16]!
	stp x14, x15, [sp, #-16]!
	stp x12, x13, [sp, #-16]!
	stp x10, x11, [sp, #-16]!
	stp x8, x9, [sp, #-16]!
	stp x6, x7, [sp, #-16]!
	stp x4, x5, [sp, #-16]!
	stp x2, x3, [sp, #-16]!
	stp x0, x1, [sp, #-16]!
	add x1, sp, #288
	mrs x2, elr_el1
	mrs x3, spsr_el1
	stp x30, x1, [sp, #240]
	stp x2, x3, [sp, #256]
.endm

.macro restore_regs
	ldp x21, x22, [sp, #256]
	ldp x0, x1, [sp], #16
	ldp x2, x3, [sp], #16
	ldp x4, x5, [sp], #16
	ldp x6, x7, [sp], #16
	ldp x8, x9, [sp], #16
	msr elr_el1, x21
	msr spsr_el1, x22
	ldp x10, x11, [sp], #16
	ldp x12, x13, [sp], #16
	ldp x14, x15, [sp], #16
	ldp x16, x17, [sp], #16
	ldp x18, x19, [sp], #16
	ldp x20, x21, [sp], #16
	ldp x22, x23, [sp], #16
	ldp x24, x25, [sp], #16
	ldp x26, x27, [sp], #16
	ldp x28, x29, [sp], #16
	ldr x30, [sp], #48
.endm

/*
 * Invalid mode handlers
 */
.macro invalid_exception, el, reason
	save_regs
	mov x0, sp
	mov x1, #\reason
	mrs x2, esr_el1
	bl arm64_invalid_exception
	restore_regs
	eret
.endm

	.global _start
_start:
	/*
	 * Initial system with exception level
	 */
	adr x0, vectors
	mrs x1, CurrentEL
	cmp x1, 0xc
	b.eq 3f
	cmp	x1, 0x8
	b.eq 2f
	cmp x1, 0x4
	b.eq 1f
3:	msr vbar_el3, x0		/* Set el3 vbar */
	mrs x2, scr_el3
	bic x2, x2, #(0xf<<0)	/* Clear ea, fiq, irq and ns bits */
	orr x2, x2, #(0x1<<10)	/* Set el2 to 64bit */
	msr scr_el3, x2
	msr cptr_el3, xzr
	ldr x2, =0x01800000		/* Initialize cntfrq */
	msr	cntfrq_el0, x2
2:	msr vbar_el2, x0		/* Set el2 vbar */
	mrs x2, hcr_el2
	orr x2, x2, #(0x1<<31)	/* Set el1 to 64bit */
    msr hcr_el2, x2
	mov x2, #0x33ff			/* Enable fp/simd */
	msr cptr_el2, x2
1:	msr vbar_el1, x0		/* Set el1 vbar */
	mrs x2, cpacr_el1
	orr x2, x2, #(0x3<<20)	/* Enable fp/simd */
	msr cpacr_el1, x2

	/*
	 * Change exception level, to el1
	 */
	mrs x1, CurrentEL
	cmp x1, 0xc
	b.eq 31f
	cmp	x1, 0x8
	b.eq 21f
	cmp x1, 0x4
	b.eq 11f
31:	adr x2, 11f
	msr elr_el3, x2
	mov x2, #((0xf<<6)|(0x5<<0))
	msr spsr_el3, x2
	isb
	eret
21:	b 11f
11:	isb

	/* Initialize stacks */
	ldr x0, _stack_el1_end
	mov sp, x0
	ldr x0, _stack_el0_end
	msr sp_el0, x0

	/* Copyself to link address */
	adr x0, _start
	ldr x1, =_start
	cmp x0, x1
	beq 1f
	ldr x0, _image_start
	adr x1, _start
	ldr x2, _image_end
	sub x2, x2, x0
	bl memcpy
1:	nop

	/* Copy data section */
	ldr x0, _data_start
	ldr x3, _image_start
	ldr x1, _data_shadow_start
	sub x1, x1, x3
	adr x3, _start
	add x1, x1 ,x3
	ldr x2, _data_shadow_start
	ldr x3, _data_shadow_end
	sub x2, x3, x2
	bl memcpy

	/* Clear bss section */
	ldr x0, _bss_start
	ldr x2, _bss_end
	sub x2, x2, x0
	mov x1, #0
	bl memset

	/* Call _main */
	ldr x1, =_main
	br x1
_main:
	mov x0, #1;
	mov x1, #0;
	bl xboot_main
	b _main

/*
 * Exception vectors.
 */
	.align 11
	.globl vectors
vectors:
	/* Current el with sp_el0 : 0x000 - 0x200 */
	ventry cel_sync_sp_el0
	ventry cel_irq_sp_el0
	ventry cel_fiq_sp_el0
	ventry cel_error_sp_el0

	/* Current el with sp_elx : 0x200 - 0x400 */
	ventry cel_sync_sp_elx
	ventry cel_irq_sp_elx
	ventry cel_fiq_sp_elx
	ventry cel_error_sp_elx

	/* Lower el using aarch64 : 0x400 - 0x600 */
	ventry lel_sync_aarch64
	ventry lel_irq_aarch64
	ventry lel_fiq_aarch64
	ventry lel_error_aarch64

	/* Lower el using aarch32 : 0x600 - 0x800 */
	ventry lel_sync_aarch32
	ventry lel_irq_aarch32
	ventry lel_fiq_aarch32
	ventry lel_error_aarch32

cel_sync_sp_el0:
	invalid_exception 1, 0x00
cel_irq_sp_el0:
	invalid_exception 1, 0x01
cel_fiq_sp_el0:
	invalid_exception 1, 0x02
cel_error_sp_el0:
	invalid_exception 1, 0x03

cel_sync_sp_elx:
	save_regs
	mov x0, sp
	bl arm64_sync_exception
	restore_regs
	eret
cel_irq_sp_elx:
	save_regs
	msr daifclr, #8
	mov x0, sp
	bl arm64_irq_exception
	restore_regs
	eret
cel_fiq_sp_elx:
	invalid_exception 1, 0x12
cel_error_sp_elx:
	invalid_exception 1, 0x13

lel_sync_aarch64:
	invalid_exception 0, 0x20
lel_irq_aarch64:
	invalid_exception 0, 0x21
lel_fiq_aarch64:
	invalid_exception 0, 0x22
lel_error_aarch64:
	invalid_exception 0, 0x23

lel_sync_aarch32:
	invalid_exception 0, 0x30
lel_irq_aarch32:
	invalid_exception 0, 0x31
lel_fiq_aarch32:
	invalid_exception 0, 0x32
lel_error_aarch32:
	invalid_exception 0, 0x33

/*
 * The location of section
 */
 	.align 4
_image_start:
	.dword __image_start
_image_end:
	.dword __image_end
_data_shadow_start:
	.dword __data_shadow_start
_data_shadow_end:
	.dword __data_shadow_end
_data_start:
	.dword __data_start
_data_end:
	.dword __data_end
_bss_start:
	.dword __bss_start
_bss_end:
	.dword __bss_end
_stack_el3_end:
	.dword __stack_el3_end
_stack_el2_end:
	.dword __stack_el2_end
_stack_el1_end:
	.dword __stack_el1_end
_stack_el0_end:
	.dword __stack_el0_end
