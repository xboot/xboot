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

#include <xconfigs.h>
#include <linkage.h>

/*
 * Enable and disable interrupts
 */
.macro disable_irq
	msr daifset, #2
.endm

.macro enable_irq
	msr daifclr, #2
.endm

/*
 * Enable and disable debug exceptions
 */
.macro disable_dbg
	msr	daifset, #8
.endm

.macro enable_dbg
	msr daifclr, #8
.endm

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
	sub	sp, sp, #8 * 36
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	.if	\el == 0
		mrs	x21, sp_el0
		get_thread_info tsk
		ldr	x19, [tsk, #TI_FLAGS]
		disable_step_tsk x19, x20
	.else
		add	x21, sp, #8 * 36
	.endif
	mrs	x22, elr_el1
	mrs	x23, spsr_el1
	stp	x30, x21, [sp, #8 * 30]
	stp	x22, x23, [sp, #8 * 32]
	.if	\el == 0
		mvn	x21, xzr
		str	x21, [sp, #8 * 35]
	.endif
.endm

/*
 * Kernel exit
 */
.macro kernel_exit, el, ret = 0
	ldp	x21, x22, [sp, #8 * 32]
	.if	\el == 0
		ct_user_enter
		ldr	x23, [sp, #8 * 31]
		msr	sp_el0, x23
	.endif
	msr	elr_el1, x21
	msr	spsr_el1, x22
	.if	\ret
		ldr	x1, [sp, #8 * 1]
	.else
		ldp	x0, x1, [sp, #16 * 0]
	.endif
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #8 * 30]
	add	sp, sp, #8 * 36
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
	b bad_mode
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
	ventry el1_irq_invalid
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
 * EL1 mode handlers.
 */
	.align 6
el1_sync:
	kernel_entry 1
	mrs	x1, esr_el1
	lsr	x24, x1, #26
	cmp	x24, #0x25
	b.eq el1_da
	cmp	x24, #0x18
	b.eq el1_undef
	cmp	x24, #0x26
	b.eq el1_sp_pc
	cmp	x24, #0x22
	b.eq el1_sp_pc
	cmp	x24, #0x00
	b.eq el1_undef
	cmp	x24, #0x31
	b.ge el1_dbg
	b el1_inv
el1_da:
	/*
	 * Data abort handling
	 */
	mrs	x0, far_el1
	enable_dbg
	tbnz x23, #7, 1f
	enable_irq
1:	mov x2, sp
	bl do_mem_abort
	disable_irq
	kernel_exit 1
el1_sp_pc:
	/*
	 * Stack or PC alignment exception handling
	 */
	mrs	x0, far_el1
	enable_dbg
	mov	x2, sp
	b do_sp_pc_abort
el1_undef:
	/*
	 * Undefined instruction
	 */
	enable_dbg
	mov	x0, sp
	b do_undefinstr
el1_dbg:
	/*
	 * Debug exception handling
	 */
	cmp	x24, #0x3c
	cinc x24, x24, eq
	tbz	x24, #0, el1_inv
	mrs	x0, far_el1
	mov	x2, sp
	bl do_debug_exception
	kernel_exit 1
el1_inv:
	enable_dbg
	mov	x0, sp
	mov	x1, #0
	mrs	x2, esr_el1
	b bad_mode
ENDPROC(el1_sync)

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
