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
	la t0, _start_link
	LREG t1, (t0)
	la t2, _start
	sub t0, t1, t2
	la a0, __stack_end
	add a0, a0, t0
	mv sp, a0

	/* Copyself to link address */
	la t0, _start_link
	LREG t1, (t0)
	la t2, _start
	sub t0, t1, t2
	beq t0, x0, 1f
	la a1, __image_start
	add a0, a1, t0
	la a2, __image_end
	add a2, a2, t0
	sub a2, a2, a0
	call memcpy
1:	nop

	/* Clear bss section */
	la t0, _start_link
	LREG t1, (t0)
	la t2, _start
	sub t0, t1, t2
	la a0, __bss_start
	add a0, a0, t0
	la a2, __bss_end
	add a2, a2, t0
	sub a2, a2, a0
	li a1, 0
	call memset

	/* Call _main */
	la t0, _start_link
	LREG t1, (t0)
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
 * The location of section
 */
	.align 4
_start_link:
	RVPTR _start
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
