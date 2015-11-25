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
.macro switch_el, xreg, el3_label, el2_label, el1_label
	mrs \xreg, CurrentEL
	cmp \xreg, 0xc
	b.eq \el3_label
	cmp	\xreg, 0x8
	b.eq \el2_label
	cmp \xreg, 0x4
	b.eq \el1_label
.endm

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro exception_entry
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

	switch_el x11, 3f, 2f, 1f
3:	mrs	x1, esr_el3
	mrs	x2, elr_el3
	b 0f
2:	mrs	x1, esr_el2
	mrs	x2, elr_el2
	b 0f
1:	mrs	x1, esr_el1
	mrs	x2, elr_el1
0:
	stp	x2, x0, [sp, #-16]!
	mov	x0, sp
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
	.align 7
	b _do_bad_sync
	.align 7
	b _do_bad_irq
	.align 7
	b _do_bad_fiq
	.align 7
	b _do_bad_error
	.align 7
	b _do_sync
	.align 7
	b _do_irq
	.align 7
	b _do_fiq
	.align 7
	b _do_error

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

	.align 4
/* Magic number(16bytes) - xbootmagicnumber */
	.byte 0x78, 0x62, 0x6f, 0x6f, 0x74, 0x6d, 0x61, 0x67, 0x69, 0x63, 0x6e, 0x75, 0x6d, 0x62, 0x65, 0x72
/* Image start and image end information */
	.long __image_start, __image_end
/* Where the image was linked address */
	.long _start
/* Image sha256 digest */
	.long 0, 0, 0, 0
/* Magic number(16bytes) - xbootmagicnumber */
	.byte 0x78, 0x62, 0x6f, 0x6f, 0x74, 0x6d, 0x61, 0x67, 0x69, 0x63, 0x6e, 0x75, 0x6d, 0x62, 0x65, 0x72

/*
 * The actual reset code
 */
reset:
	/*
	 * Disable watchdog
	 */
	ldr x0, =0xC0019000
	mov x1, #0
	str x1, [x0]

	/*
	 * Little Endian, MMU Disabled, I/D Cache Disabled
	 */
	adr x0, vectors
	switch_el x1, 3f, 2f, 1f
3:	msr vbar_el3, x0
	mrs x0, scr_el3
	orr x0, x0, #0xf
	msr scr_el3, x0
	msr cptr_el3, xzr
	ldr	x0, =0x01800000
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
0:	isb

	/* Initialize stacks */
	ldr	x0, _stack_el1_end
	mov	sp, x0
	ldr	x0, _stack_el0_end
	msr	sp_el0, x0

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
