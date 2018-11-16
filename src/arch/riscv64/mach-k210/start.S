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
#include <riscv64.h>

	.global _start
_start:
	j reset
	.word 0xdeadbeef

/*
 * The actual reset code
 */
reset:
	/* Mask all interrupts */
	csrw mideleg, zero
	csrw medeleg, zero
	csrw mie, zero
	csrw mip, zero

	/* Setup exception vectors */
	la t1, _image_start
	LREG t1, (t1)
	la t2, _start
	sub t0, t1, t2
	la a0, vectors
	add a0, a0, t0
	csrw mtvec, a0

	/* Initial registers */
	li x1, 0
	li x2, 0
	li x3, 0
	li x4, 0
	li x5, 0
	li x6, 0
	li x7, 0
	li x8, 0
	li x9, 0
	li x10, 0
	li x11, 0
	li x12, 0
	li x13, 0
	li x14, 0
	li x15, 0
	li x16, 0
	li x17, 0
	li x18, 0
	li x19, 0
	li x20, 0
	li x21, 0
	li x22, 0
	li x23, 0
	li x24, 0
	li x25, 0
	li x26, 0
	li x27, 0
	li x28, 0
	li x29, 0
	li x30, 0
	li x31, 0

	/* Enable FPU and accelerator if present */
	li t0, MSTATUS_FS | MSTATUS_XS
	csrs mstatus, t0

#if __riscv_flen == 32
	fssr x0
	fmv.s.x f0, x0
	fmv.s.x f1, x0
	fmv.s.x f2, x0
	fmv.s.x f3, x0
	fmv.s.x f4, x0
	fmv.s.x f5, x0
	fmv.s.x f6, x0
	fmv.s.x f7, x0
	fmv.s.x f8, x0
	fmv.s.x f9, x0
	fmv.s.x f10, x0
	fmv.s.x f11, x0
	fmv.s.x f12, x0
	fmv.s.x f13, x0
	fmv.s.x f14, x0
	fmv.s.x f15, x0
	fmv.s.x f16, x0
	fmv.s.x f17, x0
	fmv.s.x f18, x0
	fmv.s.x f19, x0
	fmv.s.x f20, x0
	fmv.s.x f21, x0
	fmv.s.x f22, x0
	fmv.s.x f23, x0
	fmv.s.x f24, x0
	fmv.s.x f25, x0
	fmv.s.x f26, x0
	fmv.s.x f27, x0
	fmv.s.x f28, x0
	fmv.s.x f29, x0
	fmv.s.x f30, x0
	fmv.s.x f31, x0
#elif __riscv_flen == 64
	fssr x0
	fmv.d.x f0, x0
	fmv.d.x f1, x0
	fmv.d.x f2, x0
	fmv.d.x f3, x0
	fmv.d.x f4, x0
	fmv.d.x f5, x0
	fmv.d.x f6, x0
	fmv.d.x f7, x0
	fmv.d.x f8, x0
	fmv.d.x f9, x0
	fmv.d.x f10, x0
	fmv.d.x f11, x0
	fmv.d.x f12, x0
	fmv.d.x f13, x0
	fmv.d.x f14, x0
	fmv.d.x f15, x0
	fmv.d.x f16, x0
	fmv.d.x f17, x0
	fmv.d.x f18, x0
	fmv.d.x f19, x0
	fmv.d.x f20, x0
	fmv.d.x f21, x0
	fmv.d.x f22, x0
	fmv.d.x f23, x0
	fmv.d.x f24, x0
	fmv.d.x f25, x0
	fmv.d.x f26, x0
	fmv.d.x f27, x0
	fmv.d.x f28, x0
	fmv.d.x f29, x0
	fmv.d.x f30, x0
	fmv.d.x f31, x0
#endif

	/* Initialize global pointer */
.option push
.option norelax
	la t0, _global_pointer$
	LREG gp, (t0)
.option pop

	/* Initialize stacks */
	la t1, _stack_start
	LREG t1, (t1)
	la t2, _stack_end
	LREG t2, (t2)
	sub t0, t2, t1
	csrr t3, mhartid
	li t4, CONFIG_MAX_CPUS
	div t0, t0, t4
	mul t0, t0, t3
	sub sp, t2, t0

	/* Check processor id, stop all slave cores */
	csrr t0, mhartid
	beqz t0, 2f
1:	wfi
	j 1b
2:	nop

	/* Initial system clock and uart */
	call sys_clock_init
	call sys_uart_init

	/* Copyself to link address */
	la t0, _start
	la t1, _image_start
	LREG t1, (t1)
	beq t0, t1, 1f
	la a0, _image_start
	LREG a0, (a0)
	la a2, _image_end
	LREG a2, (a2)
	sub a2, a2, a0
	la a1, _start
	call memcpy
1:	nop

	/* Clear bss section */
	la a0, _bss_start
	LREG a0, (a0)
	la a2, _bss_end
	LREG a2, (a2)
	sub a2, a2, a0
	li a1, 0
	call memset

	/* Call _main */
	la t1, _image_start
	LREG t1, (t1)
	la t2, _start
	sub t0, t1, t2
	la a0, _main
	add a0, a0, t0
	jr a0
_main:
	li a0, 0
	li a1, 0
	call xboot_main
	j _main

/*
 * Exception vectors.
 */
	.align 4
	.globl vectors
vectors:
	csrw mscratch, sp
	addi sp, sp, -(37 * REGSZ)
	SREG x1, 1 * REGSZ(x2)
	SREG x3, 3 * REGSZ(x2)
	SREG x4, 4 * REGSZ(x2)
	SREG x5, 5 * REGSZ(x2)
	SREG x6, 6 * REGSZ(x2)
	SREG x7, 7 * REGSZ(x2)
	SREG x8, 8 * REGSZ(x2)
	SREG x9, 9 * REGSZ(x2)
	SREG x10, 10 * REGSZ(x2)
	SREG x11, 11 * REGSZ(x2)
	SREG x12, 12 * REGSZ(x2)
	SREG x13, 13 * REGSZ(x2)
	SREG x14, 14 * REGSZ(x2)
	SREG x15, 15 * REGSZ(x2)
	SREG x16, 16 * REGSZ(x2)
	SREG x17, 17 * REGSZ(x2)
	SREG x18, 18 * REGSZ(x2)
	SREG x19, 19 * REGSZ(x2)
	SREG x20, 20 * REGSZ(x2)
	SREG x21, 21 * REGSZ(x2)
	SREG x22, 22 * REGSZ(x2)
	SREG x23, 23 * REGSZ(x2)
	SREG x24, 24 * REGSZ(x2)
	SREG x25, 25 * REGSZ(x2)
	SREG x26, 26 * REGSZ(x2)
	SREG x27, 27 * REGSZ(x2)
	SREG x28, 28 * REGSZ(x2)
	SREG x29, 29 * REGSZ(x2)
	SREG x30, 30 * REGSZ(x2)
	SREG x31, 31 * REGSZ(x2)
	csrrw t0, mscratch, x0
	csrr s0, mstatus
	csrr t1, mepc
	csrr t2, mbadaddr
	csrr t3, mcause
	SREG t0, 2 * REGSZ(x2)
	SREG s0, 32 * REGSZ(x2)
	SREG t1, 33 * REGSZ(x2)
	SREG t2, 34 * REGSZ(x2)
	SREG t3, 35 * REGSZ(x2)
	li x5, -1
	SREG x5, 36 * REGSZ(x2)
	move a0, sp
	jal riscv64_handle_exception
	csrr a0, mscratch
	LREG x1, 1 * REGSZ(a0)
	LREG x2, 2 * REGSZ(a0)
	LREG x3, 3 * REGSZ(a0)
	LREG x4, 4 * REGSZ(a0)
	LREG x5, 5 * REGSZ(a0)
	LREG x6, 6 * REGSZ(a0)
	LREG x7, 7 * REGSZ(a0)
	LREG x8, 8 * REGSZ(a0)
	LREG x9, 9 * REGSZ(a0)
	LREG x11, 11 * REGSZ(a0)
	LREG x12, 12 * REGSZ(a0)
	LREG x13, 13 * REGSZ(a0)
	LREG x14, 14 * REGSZ(a0)
	LREG x15, 15 * REGSZ(a0)
	LREG x16, 16 * REGSZ(a0)
	LREG x17, 17 * REGSZ(a0)
	LREG x18, 18 * REGSZ(a0)
	LREG x19, 19 * REGSZ(a0)
	LREG x20, 20 * REGSZ(a0)
	LREG x21, 21 * REGSZ(a0)
	LREG x22, 22 * REGSZ(a0)
	LREG x23, 23 * REGSZ(a0)
	LREG x24, 24 * REGSZ(a0)
	LREG x25, 25 * REGSZ(a0)
	LREG x26, 26 * REGSZ(a0)
	LREG x27, 27 * REGSZ(a0)
	LREG x28, 28 * REGSZ(a0)
	LREG x29, 29 * REGSZ(a0)
	LREG x30, 30 * REGSZ(a0)
	LREG x31, 31 * REGSZ(a0)
	LREG x10, 10 * REGSZ(a0)
	mret

/*
 * The location of section
 */
	.align 3
_image_start:
	RVPTR __image_start
_image_end:
	RVPTR __image_end
_global_pointer$:
	RVPTR __global_pointer$
_data_start:
	RVPTR __data_start
_data_end:
	RVPTR __data_end
_bss_start:
	RVPTR __bss_start
_bss_end:
	RVPTR __bss_end
_stack_start:
	RVPTR __stack_start
_stack_end:
	RVPTR __stack_end
