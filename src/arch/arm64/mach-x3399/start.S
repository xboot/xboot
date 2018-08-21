/*
 * start.S
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xconfigs.h>
#include <linkage.h>

.macro ventry label
	.align 7
	b \label
.endm

.macro save_regs
	sub sp, sp, #0x20
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
	add	x21, sp, #0x110
	mrs	x22, elr_el1
	mrs	x23, spsr_el1
	stp	x30, x21, [sp, #0xf0]
	stp	x22, x23, [sp, #0x100]
.endm

.macro restore_regs
	ldp x22, x23, [sp, #0x100]
	ldp x30, x28, [sp, #0xf0]
	msr elr_el1, x22
	msr	spsr_el1, x23
	mov x29, sp
	mov sp, x28
	ldp	x0, x1, [x29], #16
	ldp	x2, x3, [x29], #16
	ldp	x4, x5, [x29], #16
	ldp	x6, x7, [x29], #16
	ldp	x8, x9, [x29], #16
	ldp	x10, x11, [x29], #16
	ldp	x12, x13, [x29], #16
	ldp	x14, x15, [x29], #16
	ldp	x16, x17, [x29], #16
	ldp	x18, x19, [x29], #16
	ldp	x20, x21, [x29], #16
	ldp	x22, x23, [x29], #16
	ldp	x24, x25, [x29], #16
	ldp	x26, x27, [x29], #16
	ldr	x28, [x29], #8
	ldr	x29, [x29]
	eret
.endm

/*
 * Invalid mode handlers
 */
.macro invalid_exception, reason
	save_regs
	mov x0, sp
	mov x1, #\reason
	bl arm64_invalid_exception
	restore_regs
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
3:	msr vbar_el3, x0
	mrs x0, scr_el3
	orr	x0, x0, #0xf
	msr scr_el3, x0
	msr cptr_el3, xzr
	ldr x0, =0x01800000
	msr	cntfrq_el0, x0
	b 0f
2:	msr vbar_el2, x0
	mov x0, #0x33ff
	msr cptr_el2, x0
	b 0f
1:	msr vbar_el1, x0
	mrs x0, cpacr_el1
	orr x0, x0, #(0x3 << 20)
	msr cpacr_el1, x0
0:	nop

	/*
	 * Change exception level to el1
	 */
	adr	x3, el2
	bl armv8_switch_to_el2
el2:adr	x3, el1
	bl armv8_switch_to_el1
el1:nop

	/* Enable instruction cache */
	mrs x0, sctlr_el1
	orr x0, x0, #(1 << 12)
	msr sctlr_el1, x0

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
	invalid_exception 0x00
cel_irq_sp_el0:
	invalid_exception 0x01
cel_fiq_sp_el0:
	invalid_exception 0x02
cel_error_sp_el0:
	invalid_exception 0x03

cel_sync_sp_elx:
	save_regs
	mov x0, sp
	bl arm64_sync_exception
	restore_regs
cel_irq_sp_elx:
	save_regs
	mov x0, sp
	bl arm64_irq_exception
	restore_regs
cel_fiq_sp_elx:
	invalid_exception 0x12
cel_error_sp_elx:
	invalid_exception 0x13

lel_sync_aarch64:
	invalid_exception 0x20
lel_irq_aarch64:
	invalid_exception 0x21
lel_fiq_aarch64:
	invalid_exception 0x22
lel_error_aarch64:
	invalid_exception 0x23

lel_sync_aarch32:
	invalid_exception 0x30
lel_irq_aarch32:
	invalid_exception 0x31
lel_fiq_aarch32:
	invalid_exception 0x32
lel_error_aarch32:
	invalid_exception 0x33

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
