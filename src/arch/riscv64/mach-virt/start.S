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
	/* Initialize stacks */
	la t0, _stack_end
	ld a0, (t0)
	mv sp, a0

	/* Copyself to link address */
/*	adr x0, _start
	ldr x1, =_start
	cmp x0, x1
	beq 1f
	ldr x0, _image_start
	adr x1, _start
	ldr x2, _image_end
	sub x2, x2, x0
	jal memcpy
1:	nop*/

	/* Load data section */
/*	la a0, _data_lma
	la a1, _data
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
*/

	/* Clear bss section */
	la t0, _bss_start
	ld a0, (t0)
	la t1, _bss_end
	ld a1, (t1)
	bgeu a0, a1, 2f
1:	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:	nop

	/* Call _main */
_main:
	call xboot_main
	j _main

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
_stack_start:
	.dword __stack_start
_stack_end:
	.dword __stack_end
