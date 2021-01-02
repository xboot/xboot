/*
 * start.S
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
 * Branch according to exception level
 */
.macro switch_el, xreg, el3_label, el2_label, el1_label
	mrs \xreg, CurrentEL
	cmp \xreg, 0xc
	b.eq \el3_label
	cmp \xreg, 0x8
	b.eq \el2_label
	cmp \xreg, 0x4
	b.eq \el1_label
.endm

/*
 * Branch if current processor is a slave
 */
.macro branch_if_slave, xreg, slave_label
	mrs \xreg, mpidr_el1
	tst \xreg, #0xff
	b.ne \slave_label
	lsr \xreg, \xreg, #8
	tst \xreg, #0xff
	b.ne \slave_label
	lsr \xreg, \xreg, #8
	tst \xreg, #0xff
	b.ne \slave_label
	lsr \xreg, \xreg, #16
	tst \xreg, #0xff
	b.ne \slave_label
.endm

/*
 * Branch if current processor is a master
 */
.macro	branch_if_master, xreg1, xreg2, master_label
	mrs \xreg1, mpidr_el1
	lsr \xreg2, \xreg1, #32
	lsl \xreg2, \xreg2, #32
	lsl \xreg1, \xreg1, #40
	lsr \xreg1, \xreg1, #40
	orr \xreg1, \xreg1, \xreg2
	cbz \xreg1, \master_label
.endm

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
	/* Reset the sctrl register */
	switch_el x1, 3f, 2f, 1f
3:	mrs x0, sctlr_el3
	b 0f
2:	mrs x0, sctlr_el2
	b 0f
1:	mrs x0, sctlr_el1
0:	ldr x1, =0xfdfffffa
	and x0, x0, x1
	switch_el x1, 7f, 6f, 5f
7:	msr sctlr_el3, x0
	b 4f
6:	msr sctlr_el2, x0
	b 4f
5:	msr sctlr_el1, x0
4:	dsb sy
	isb

	/* Invalidate all tlb entries */
	switch_el x1, 3f, 2f, 1f
3:	tlbi alle3
	dsb sy
	isb
	b 0f
2:	tlbi alle2
	dsb sy
	isb
	b 0f
1:	tlbi vmalle1
	dsb sy
	isb
0:	nop

	/*
	 * Initial system with exception level
	 */
	ldr x0, =vectors
	switch_el x1, 3f, 2f, 1f
3:	msr vbar_el3, x0			/* Set el3 vbar */
	mrs x2, scr_el3
	orr x2, x2, #0xf			/* Enable non-secure, irq, fiq and ea */
	msr scr_el3, x2
	msr cptr_el3, xzr			/* Enable fp/simd */
	ldr x2, =24000000
	msr cntfrq_el0, x2			/* Initialize cntfrq */
2:	msr vbar_el2, x0			/* Set el2 vbar */
	mov x2, #0x33ff
	msr cptr_el2, x2			/* Enable fp/simd */
1:	msr vbar_el1, x0			/* Set el1 vbar */
	ldr x2, _stack_el1_end
	msr sp_el1, x2				/* Sel el1 stack */
	mov x2, #3 << 20
	msr cpacr_el1, x2			/* Enable fp/simd */
0:	ldr x2, _stack_el0_end
	msr sp_el0, x2				/* Sel el0 stack */

	/* Enable smpen bit for coherency */
	switch_el x1, 3f, 0f, 0f
3:	mrs x2, S3_1_c15_c2_1
	orr x2, x2, #0x40
	msr S3_1_c15_c2_1, x2
0:	nop

	/* Processor specific initial */
	branch_if_slave x0, 1f
	ldr x0, =0xfee00000
	bl gic_init_secure
1:	ldr x0, =0xfef00000
	bl gic_init_secure_percpu

	branch_if_master x0, x1, master_cpu
slave_cpu:
1:  wfe
	b 1b
master_cpu:

	/* Enable instruction cache */
	mrs x0, sctlr_el1
	orr x0, x0, #(1 << 12)
	msr sctlr_el1, x0

	/* Initialize stacks */
	ldr x0, _stack_el1_end
	mov sp, x0
	ldr x0, _stack_el0_end
	msr sp_el0, x0

	/* Initial system jtag and uart */
	bl sys_jtag_init
	bl sys_uart_init

	/* Uart putchar for new line */
	mov x0, #'\r'
	bl sys_uart_putc
	mov x0, #'\n'
	bl sys_uart_putc

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
	bl xboot_main
	b _main

	/* Initialize secure copy of gic at el3 */
gic_init_secure:
	mov w9, #0x37
	str w9, [x0, 0x0000]
	ldr w9, [x0, 0x0004]
	and w10, w9, #0x1f
	cbz w10, 1f
	add x11, x0, (0x0080 + 4)
	add x12, x0, (0x0d00 + 4)
	mov w9, #~0
0:	str w9, [x11], #0x4
	str wzr, [x12], #0x4
	sub w10, w10, #0x1
	cbnz w10, 0b
1:	ret

	/* Initialize secure copy of gic at el3 for per cpu */
gic_init_secure_percpu:
	mrs x10, mpidr_el1
	lsr x9, x10, #32
	bfi x10, x9, #24, #8
	mov x9, x0
1:	ldr x11, [x9, 0x0008]
	lsr x11, x11, #32
	cmp w10, w11
	b.eq 2f
	add x9, x9, #(2 << 16)
	b 1b
2:	mov w10, #~0x2
	ldr w11, [x9, 0x0014]
	and w11, w11, w10
	str w11, [x9, 0x0014]
	dsb st
	isb
3:	ldr w10, [x9, 0x0014]
	tbnz w10, #2, 3b
	add x10, x9, #(1 << 16)
	mov w11, #~0
	str w11, [x10, 0x0080]
	str wzr, [x10, 0x0d00]
	mov w11, #0x1
	str w11, [x10, 0x0100]

	switch_el x0, el3_sre, el2_sre, el1_sre
el3_sre:
	mrs x10, S3_6_C12_C12_5
	orr x10, x10, #0xf
	msr S3_6_C12_C12_5, x10
	isb
el2_sre:
	mrs x10, S3_4_C12_C9_5
	orr x10, x10, #0xf
	msr S3_4_C12_C9_5, x10
	isb
el1_sre:
	mrs x0, CurrentEL
	cmp x0, 0xc
	b.ne el1_ctlr
el3_ctlr:
	mov x10, #0x3
	msr S3_6_C12_C12_7, x10
	isb
	msr S3_6_C12_C12_4, xzr
	isb
el1_ctlr:
	mov x10, #0x3
	msr S3_0_C12_C12_7, x10
	isb
	msr S3_0_C12_C12_4, xzr
	isb
	mov x10, #0xf0
	msr S3_0_C4_C6_0, x10
	isb
	ret

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
