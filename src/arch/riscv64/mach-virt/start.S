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

	.global _start
_start:
	j reset

/*
 * The actual reset code
 */
reset:
	/* Mask all interrupts */
	csrw sie, zero

	/* Initialize stacks */
	la a0, _stack_end
	LREG a0, (a0)
	mv sp, a0

	/* Setup exception vectors */
	la t1, _image_start
	LREG t1, (t1)
	la t2, _start
	sub t0, t1, t2
	la a0, vectors
	add a0, a0, t0
	csrw mtvec, a0

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
	li a0, 1
	li a1, 0
	call xboot_main
	j _main

/*
 * Exception vectors.
 */
	.align 4
	.globl vectors
vectors:
	csrrw tp, sscratch, tp
	SREG s0, 2 * REGSZ(tp)
	add s0, sp, zero
	LREG sp, 1 * REGSZ(tp)
	addi sp, sp, -(35 * REGSZ)
	SREG s0, 2 * REGSZ(sp)
	LREG s0, 2 * REGSZ(tp)
	SREG s0, 8 * REGSZ(sp)
	csrrw tp, sscratch, tp
	SREG ra, 1 * REGSZ(sp)
	SREG gp, 3 * REGSZ(sp)
	SREG tp, 4 * REGSZ(sp)
	SREG t0, 5 * REGSZ(sp)
	SREG t1, 6 * REGSZ(sp)
	SREG t2, 7 * REGSZ(sp)
	SREG s1, 9 * REGSZ(sp)
	SREG a0, 10 * REGSZ(sp)
	SREG a1, 11 * REGSZ(sp)
	SREG a2, 12 * REGSZ(sp)
	SREG a3, 13 * REGSZ(sp)
	SREG a4, 14 * REGSZ(sp)
	SREG a5, 15 * REGSZ(sp)
	SREG a6, 16 * REGSZ(sp)
	SREG a7, 17 * REGSZ(sp)
	SREG s2, 18 * REGSZ(sp)
	SREG s3, 19 * REGSZ(sp)
	SREG s4, 20 * REGSZ(sp)
	SREG s5, 21 * REGSZ(sp)
	SREG s6, 22 * REGSZ(sp)
	SREG s7, 23 * REGSZ(sp)
	SREG s8, 24 * REGSZ(sp)
	SREG s9, 25 * REGSZ(sp)
	SREG s10, 26 * REGSZ(sp)
	SREG s11, 27 * REGSZ(sp)
	SREG t3, 28 * REGSZ(sp)
	SREG t4, 29 * REGSZ(sp)
	SREG t5, 30 * REGSZ(sp)
	SREG t6, 31 * REGSZ(sp)
	csrr s0, sepc
	SREG s0, 32 * REGSZ(sp)
	csrr s1, sstatus
	SREG s1, 33 * REGSZ(sp)
	add	s2, sp, 35 * REGSZ
	SREG s2, 34 * REGSZ(sp)
	add	a0, sp, zero
	call riscv64_handle_exception
	LREG s0, 34 * REGSZ(sp)
	csrr s1, sscratch
	SREG s0, 1 * REGSZ(s1)
	LREG s2, 32 * REGSZ(sp)
	csrw sepc, s2
	LREG s3, 33 * REGSZ(sp)
	csrw sstatus, s3
	LREG ra, 1 * REGSZ(sp)
	LREG gp, 3 * REGSZ(sp)
	LREG tp, 4 * REGSZ(sp)
	LREG t0, 5 * REGSZ(sp)
	LREG t1, 6 * REGSZ(sp)
	LREG t2, 7 * REGSZ(sp)
	LREG s0, 8 * REGSZ(sp)
	LREG s1, 9 * REGSZ(sp)
	LREG a0, 10 * REGSZ(sp)
	LREG a1, 11 * REGSZ(sp)
	LREG a2, 12 * REGSZ(sp)
	LREG a3, 13 * REGSZ(sp)
	LREG a4, 14 * REGSZ(sp)
	LREG a5, 15 * REGSZ(sp)
	LREG a6, 16 * REGSZ(sp)
	LREG a7, 17 * REGSZ(sp)
	LREG s2, 18 * REGSZ(sp)
	LREG s3, 19 * REGSZ(sp)
	LREG s4, 20 * REGSZ(sp)
	LREG s5, 21 * REGSZ(sp)
	LREG s6, 22 * REGSZ(sp)
	LREG s7, 23 * REGSZ(sp)
	LREG s8, 24 * REGSZ(sp)
	LREG s9, 25 * REGSZ(sp)
	LREG s10, 26 * REGSZ(sp)
	LREG s11, 27 * REGSZ(sp)
	LREG t3, 28 * REGSZ(sp)
	LREG t4, 29 * REGSZ(sp)
	LREG t5, 30 * REGSZ(sp)
	LREG t6, 31 * REGSZ(sp)
	LREG sp, 2 * REGSZ(sp)
	sret

/*
 * The location of section
 */
	.align 4
_image_start:
	RVPTR __image_start
_image_end:
	RVPTR __image_end
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
