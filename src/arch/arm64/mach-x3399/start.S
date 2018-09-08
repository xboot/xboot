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

/*
 * Vector entry
 */
.macro ventry label
	.align 7
	b \label
.endm

/*
 * Kernel entry
 */
.macro kernel_entry, el
	sub sp, sp, #8 * 36
	stp x0, x1, [sp, #16 * 0]
	stp x2, x3, [sp, #16 * 1]
	stp x4, x5, [sp, #16 * 2]
	stp x6, x7, [sp, #16 * 3]
	stp x8, x9, [sp, #16 * 4]
	stp x10, x11, [sp, #16 * 5]
	stp x12, x13, [sp, #16 * 6]
	stp x14, x15, [sp, #16 * 7]
	stp x16, x17, [sp, #16 * 8]
	stp x18, x19, [sp, #16 * 9]
	stp x20, x21, [sp, #16 * 10]
	stp x22, x23, [sp, #16 * 11]
	stp x24, x25, [sp, #16 * 12]
	stp x26, x27, [sp, #16 * 13]
	stp x28, x29, [sp, #16 * 14]
	.if \el == 0
		mrs x21, sp_el0
	.else
		add x21, sp, #8 * 36
	.endif
	mrs x22, elr_el1
	mrs x23, spsr_el1
	stp x30, x21, [sp, #8 * 30]
	stp x22, x23, [sp, #8 * 32]
	.if \el == 0
		mvn x21, xzr
		str x21, [sp, #8 * 35]
	.endif
.endm

/*
 * Kernel exit
 */
.macro kernel_exit, el
	ldp x21, x22, [sp, #8 * 32]
	.if \el == 0
		ldr x23, [sp, #8 * 31]
		msr sp_el0, x23
	.endif
	msr elr_el1, x21
	msr spsr_el1, x22
	ldp x0, x1, [sp, #16 * 0]
	ldp x2, x3, [sp, #16 * 1]
	ldp x4, x5, [sp, #16 * 2]
	ldp x6, x7, [sp, #16 * 3]
	ldp x8, x9, [sp, #16 * 4]
	ldp x10, x11, [sp, #16 * 5]
	ldp x12, x13, [sp, #16 * 6]
	ldp x14, x15, [sp, #16 * 7]
	ldp x16, x17, [sp, #16 * 8]
	ldp x18, x19, [sp, #16 * 9]
	ldp x20, x21, [sp, #16 * 10]
	ldp x22, x23, [sp, #16 * 11]
	ldp x24, x25, [sp, #16 * 12]
	ldp x26, x27, [sp, #16 * 13]
	ldp x28, x29, [sp, #16 * 14]
	ldr x30, [sp, #8 * 30]
	add sp, sp, #8 * 36
	eret
.endm

/*
 * Invalid mode handlers
 */
.macro inv_entry, el, reason
	kernel_entry el
	mov x0, sp
	mov x1, #\reason
	mrs x2, esr_el1
	bl arm64_invalid_exception
	kernel_exit el
.endm

	.global _start
_start:
	b reset
/*
 * Exception vectors.
 */
	.align 11
	.globl vectors
vectors:
	ventry el1_sync_invalid
	ventry el1_irq_invalid
	ventry el1_fiq_invalid
	ventry el1_error_invalid

	ventry el1_sync
	ventry el1_irq
	ventry el1_fiq_invalid
	ventry el1_error_invalid

	ventry el0_sync_invalid
	ventry el0_irq_invalid
	ventry el0_fiq_invalid
	ventry el0_error_invalid

	ventry el0_sync_invalid
	ventry el0_irq_invalid
	ventry el0_fiq_invalid
	ventry el0_error_invalid

/*
 * The actual reset code
 */
reset:
	/* Check processor id, stop all slave cores */
	mrs x0, mpidr_el1
	and x0, x0, #0xff
	cbz x0, 2f
1:  wfe
	b 1b
2:	nop

	/*
	 * Initial system with exception level
	 */
	adr x0, vectors
	mrs x1, CurrentEL
	cmp x1, 0xc
	b.eq 3f
	cmp x1, 0x8
	b.eq 2f
	cmp x1, 0x4
	b.eq 1f
3:	msr vbar_el3, x0		/* Set el3 vbar */
	mov x2, #0x33ff
	msr cptr_el3, x2		/* Disable el3 coprocessor traps */
	ldr x2, =19200000
	msr cntfrq_el0, x2		/* Initialize cntfrq */
	mov x2, #0x5b1
	msr scr_el3, x2			/* Set el2 to 64bit, hvc enable, smc disable, disable routing, non-secure */
2:	msr vbar_el2, x0		/* Set el2 vbar */
	mov x2, #0x33ff
	msr cptr_el2, x2		/* Disable el2 coprocessor traps */
	mrs x2, cnthctl_el2
	orr x2, x2, #3
	msr cnthctl_el2, x2		/* Enable cntp for el1 */
	msr cntvoff_el2, xzr	/* Virtual offset to zero */
	mrs x2, hcr_el2
	orr x2, x2, #(1 << 31)	/* Set el1 to 64bit */
	orr x2, x2, #(1 << 1)	/* Hardwired swio */
	msr hcr_el2, x2
1:	msr vbar_el1, x0		/* Set el1 vbar */
	ldr x2, _stack_el1_end
	msr sp_el1, x2			/* Sel el1 stack */
	mrs x2, cpacr_el1
	orr x2, x2, #(3 << 20)
	msr cpacr_el1, x2		/* Enable fp/simd */
0:	ldr x2, _stack_el0_end
	msr sp_el0, x2			/* Sel el0 stack */

	/*
	 * Change exception level to el1
	 */
/*	mrs x1, CurrentEL
	cmp x1, 0xc
	b.eq 3f
	cmp x1, 0x8
	b.eq 2f
	cmp x1, 0x4
	b.eq 1f
3:	mov x2, #0x3c9
	msr spsr_el3, x2
	adr x2, 2f
	msr elr_el3, x2
	eret
2:	mov x2, #0x3c5
	msr spsr_el2, x2
	adr x2, 1f
	msr elr_el2, x2
	eret
1:	nop
*/

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

	.align 6
el1_sync:
	kernel_entry 1
	mov x0, sp
	bl arm64_sync_exception
	kernel_exit 1
ENDPROC(el1_sync)

	.align 6
el1_irq:
	kernel_entry 1
	msr daifclr, #8
	mov x0, sp
	bl arm64_irq_exception
	kernel_exit 1
ENDPROC(el1_irq)

el1_sync_invalid:
	inv_entry 1, 0
ENDPROC(el1_sync_invalid)

el1_irq_invalid:
	inv_entry 1, 1
ENDPROC(el1_irq_invalid)

el1_fiq_invalid:
	inv_entry 1, 2
ENDPROC(el1_fiq_invalid)

el1_error_invalid:
	inv_entry 1, 3
ENDPROC(el1_error_invalid)

el0_sync_invalid:
	inv_entry 0, 0
ENDPROC(el0_sync_invalid)

el0_irq_invalid:
	inv_entry 0, 1
ENDPROC(el0_irq_invalid)

el0_fiq_invalid:
	inv_entry 0, 2
ENDPROC(el0_fiq_invalid)

el0_error_invalid:
	inv_entry 0, 3
ENDPROC(el0_error_invalid)

/*
 * The location of section
 */
 	.align 4
_image_start:
	.dword __image_start
_image_end:
	.dword __image_end
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
