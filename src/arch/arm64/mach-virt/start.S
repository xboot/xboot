/*
 * start.S
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

/*
 * Branch according to exception level
 */
.macro	switch_el, xreg, el3_label, el2_label, el1_label
	mrs \xreg, CurrentEL
	cmp \xreg, 0xc
	b.eq	\el3_label
	cmp	\xreg, 0x8
	b.eq	\el2_label
	cmp \xreg, 0x4
	b.eq	\el1_label
.endm

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro	exception_entry
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	/* Could be running at EL3/EL2/EL1 */
	switch_el x11, 3f, 2f, 1f
3:	mrs	x1, esr_el3
	mrs	x2, elr_el3
	b	0f
2:	mrs	x1, esr_el2
	mrs	x2, elr_el2
	b	0f
1:	mrs	x1, esr_el1
	mrs	x2, elr_el1
0:
	stp	x2, x0, [sp, #-16]!
	mov	x0, sp
.endm

	.global	_start
_start:
	b	reset

/*
 * Exception vectors.
 */
	.align	11
	.globl	vectors
vectors:
	.align	7
	b	_do_bad_sync
	.align	7
	b	_do_bad_irq
	.align	7
	b	_do_bad_fiq
	.align	7
	b	_do_bad_error
	.align	7
	b	_do_sync
	.align	7
	b	_do_irq
	.align	7
	b	_do_fiq
	.align	7
	b	_do_error

_do_bad_sync:
	exception_entry
//	bl	do_bad_sync
_do_bad_irq:
	exception_entry
//	bl	do_bad_irq
_do_bad_fiq:
	exception_entry
//	bl	do_bad_fiq
_do_bad_error:
	exception_entry
//	bl	do_bad_error
_do_sync:
	exception_entry
//	bl	do_sync
_do_irq:
	exception_entry
//	bl	do_irq
_do_fiq:
	exception_entry
//	bl	do_fiq
_do_error:
	exception_entry
//	bl	do_error

/*
 * The actual reset code
 */
reset:
	adrp x0, __stack_end
	mov sp, x0

	bl copy_shadow

/*
 * Clear bss section
 */
clear_bss:
	ldr	x0, =__bss_start
	ldr	x1, =__bss_end
	mov	x2, #0
loop:
	str	x2, [x0]
	add	x0, x0, #8
	cmp	x0, x1
	b.lo loop

	bl	xboot_main

/*
 * The section location
 */
 	.align 8
_text_start:
	.dword	__text_start
_text_end:
	.dword	__text_end
_romdisk_start:
	.dword	__romdisk_start
_romdisk_end:
	.dword	__romdisk_end
_data_shadow_start:
	.dword	 __data_shadow_start
_data_shadow_end:
	.dword	 __data_shadow_end
_data_start:
	.dword	__data_start
_data_end:
	.dword	__data_end
_bss_start:
	.dword	__bss_start
_bss_end:
	.dword	__bss_end
_stack_start:
	.dword	__stack_start
_stack_end:
	.dword	__stack_end

.end
