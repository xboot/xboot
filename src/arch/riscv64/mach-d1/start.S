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
#include <riscv64.h>

	.global _start
_start:
	/* Boot head information for BROM */
	.long 0x0300006f
	.byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'
	.long 0x12345678				/* checksum */
	.long __spl_size				/* spl size */
	.long 0x30						/* boot header size */
	.long 0x30303033				/* boot header version */
	.long 0x00020000				/* return value */
	.long 0x00020000				/* run address */
	.long 0x0						/* eGON version */
	.byte 0x00, 0x00, 0x00, 0x00	/* platform information - 8byte */
	.byte 0x34, 0x2e, 0x30, 0x00

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

	/* Enable FPU and accelerator if present */
	li t0, MSTATUS_FS | MSTATUS_XS
	csrs mstatus, t0

	/* Enable theadisaee */
	li t1, 0x1 << 22
	.word 0x7c032073	/* csrs mxstatus, t1 */

	/* Invaild icache/dcache/btb/bht */
	li t1, 0x30013
	.word 0x7c232073	/* csrs mcor, t1 */

	/* Check processor id and initialized */
	csrr t0, mhartid
	bnez t0, _avoid
	la t0, _start
	la t1, _image_start
	LREG t1, (t1)
	beq t0, t1, _avoid

	/* Initial system jtag, uart and clock */
	call sys_jtag_init
	call sys_uart_init
	call sys_clock_init

	/* Copy ddr bin to 0x00030000 */
	la t1, _ddr_bin_start
	LREG t1, (t1)
	la t2, _ddr_bin_end
	LREG t2, (t2)
	sub a2, t2, t1
	la t1, _image_start
	LREG t1, (t1)
	la t2, _ddr_bin_start
	LREG t2, (t2)
	sub t0, t2, t1
	la a1, _start
	add a1, a1, t0
	li a0, 0x00030000
	call memcpy

	/* Initial ddr controller */
	call sys_dram_init
_avoid:
	nop

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
	li t4, CONFIG_MAX_SMP_CPUS
	div t0, t0, t4
	mul t0, t0, t3
	sub sp, t2, t0

	/* Check processor id, and startup slave cores */
	csrr a0, mhartid
	beqz a0, 2f
1:	call sys_smp_secondary_startup
	j 1b
2:	nop

	/* Copyself to link address */
	la t0, _start
	la t1, _image_start
	LREG t1, (t1)
	beq t0, t1, 1f
	call sys_copyself
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
_ddr_bin_start:
	RVPTR __ddr_bin_start
_ddr_bin_end:
	RVPTR __ddr_bin_end
