/*
 * start.S
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

.macro save_regs
	str lr, [sp, #-4]
	mrs lr, spsr_all
	str lr, [sp, #-8]
	str r1, [sp, #-12]
	str r0, [sp, #-16]
	mov r0, sp
	cps #0x13
	ldr r1, [r0, #-4]
	str r1, [sp, #-4]!
	ldr r1, [r0, #-8]
	str r1, [sp, #-(4 * 16)]
	ldr r1, [r0, #-12]
	ldr r0, [r0, #-16]
	stmdb sp, {r0 - r14}^
	sub sp, sp, #(4 * 16)
	ldr r4, [sp]
	and r0, r4, #0x1f
	cmp r0, #0x10
	beq 10f
	cmp r0, #0x13
	beq 11f
	b .
11:	add r1, sp, #(4 * 17)
	str r1, [sp, #(4 * 14)]
	str lr, [sp, #(4 * 15)]
10:	add r1, sp, #(4 * 17)
	str r1, [sp, #-4]!
	mov r0, sp
.endm

.macro restore_regs
	mov r12, sp
	ldr sp, [r12], #4
	ldr r1, [r12], #4
	msr spsr_cxsf, r1
	and r0, r1, #0x1f
	cmp r0, #0x10
	beq 20f
	cmp r0, #0x13
	beq 21f
	b .
20:	ldr lr, [r12, #(4 * 15)]
	ldmia r12, {r0 - r14}^
	movs pc, lr
21:	ldm r12, {r0 - r15}^
	mov r0, r0
.endm

/*
 * Exception vector table
 */
.text
	.arm

	.global _start
_start:
	/* Boot head information for BROM */
	.long 0xea000016
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

_vector:
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

/*
 * The actual reset code
 */
reset:
	/* Save boot params to 0x00047fe0 */
	ldr r0, =0x00047fe0
	str sp, [r0, #0]
	str lr, [r0, #4]
	mrs lr, cpsr
	str lr, [r0, #8]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #12]
	mrc p15, 0, lr, c12, c0, 0
	str lr, [r0, #16]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #20]

	/* Check flag and enter to fel mode */
	ldr r0, =0x07090108
	ldr r1, [r0]
	ldr r2, =0x5aa5a55a
	cmp r1, r2
	bne _entry
	mov r1, #0
	str r1, [r0]
	bl return_to_fel

_entry:
	/* Enter svc mode cleanly and mask interrupts */
	mrs r0, cpsr
	eor r0, r0, #0x1a
	tst r0, #0x1f
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	bne 1f
	orr r0, r0, #0x100
	adr lr, 2f
	msr spsr_cxsf, r0
	.word 0xe12ef30e /* msr elr_hyp, lr */
	.word 0xe160006e /* eret */
1:	msr cpsr_c, r0
2:	nop

	/* Set vector base address register */
	ldr r0, =_vector
	mcr p15, 0, r0, c12, c0, 0
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1 << 13)
	mcr p15, 0, r0, c1, c0, 0

	/* Enable SMP mode for dcache, by setting bit 6 of auxiliary ctl reg */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, r0, #(1 << 6)
	mcr p15, 0, r0, c1, c0, 1

	/* Enable neon/vfp unit */
	mrc p15, 0, r0, c1, c0, 2
	orr r0, r0, #(0xf << 20)
	mcr p15, 0, r0, c1, c0, 2
	isb
	mov r0, #0x40000000
	vmsr fpexc, r0

	/* Check initialized and using master core */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq _speedup
	mrc p15, 0, r0, c0, c0, 5
	ands r0, #0xf
	bne _speedup

	/* Initial system jtag, uart and clock */
	bl sys_jtag_init
	bl sys_uart_init
	bl sys_clock_init

	/* Avoid using dram binary space, copy self to 0x00037fe0 */
	ldr r0, =0x00037fe0
	adr r1, _start
	cmp r0, r1
	beq _avoid
	ldr r2, =__spl_size
	bl memcpy
	ldr r0, =_avoid
	ldr r1, =_start
	sub r0, r0, r1
	ldr r1, =0x00037fe0
	add r0, r0, r1
	mov pc, r0
_avoid:
	nop

	/* Copy ddr bin to 0x00028000 */
	ldr r0, _ddr_bin_start
	ldr r2, _ddr_bin_end
	sub r2, r2, r0
	ldr r1, _image_start
	sub r0, r0, r1
	adr r1, _start
	add r1, r1, r0
	ldr r0, =0x00028000
	bl memcpy

	/* Copy ddr param to 0x00028038 */
	ldr r0, _ddr_param_start
	ldr r2, _ddr_param_end
	sub r2, r2, r0
	ldr r1, _image_start
	sub r0, r0, r1
	adr r1, _start
	add r1, r1, r0
	ldr r0, =0x00028038
	bl memcpy

	/* Initial ddr */
	bl sys_dram_init

	/* Boot speed up, leave slower sram */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq _speedup
	ldr r0, =0x43f80000
	adr r1, _start
	ldr r2, =__spl_size
	bl memcpy
	ldr r0, =_speedup
	ldr r1, =_start
	sub r0, r0, r1
	ldr r1, =0x43f80000
	add r0, r0, r1
	mov pc, r0
_speedup:
	nop

	/* Initialize stacks */
	mrc p15, 0, r4, c0, c0, 5
	and r4, r4, #0xf
	mov r5, #2

	ldr r0, _stack_und_start
	ldr r1, _stack_und_end
	sub r0, r1, r0
	mov r1, r5
	bl udiv32
	mul r2, r0, r4
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x1b
	msr cpsr_cxsf, r1
	ldr sp, _stack_und_end
	sub sp, sp, r2

	ldr r0, _stack_abt_start
	ldr r1, _stack_abt_end
	sub r0, r1, r0
	mov r1, r5
	bl udiv32
	mul r2, r0, r4
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x17
	msr cpsr_cxsf, r1
	ldr sp, _stack_abt_end
	sub sp, sp, r2

	ldr r0, _stack_irq_start
	ldr r1, _stack_irq_end
	sub r0, r1, r0
	mov r1, r5
	bl udiv32
	mul r2, r0, r4
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x12
	msr cpsr_cxsf, r1
	ldr sp, _stack_irq_end
	sub sp, sp, r2

	ldr r0, _stack_fiq_start
	ldr r1, _stack_fiq_end
	sub r0, r1, r0
	mov r1, r5
	bl udiv32
	mul r2, r0, r4
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x11
	msr cpsr_cxsf, r1
	ldr sp, _stack_fiq_end
	sub sp, sp, r2

	ldr r0, _stack_srv_start
	ldr r1, _stack_srv_end
	sub r0, r1, r0
	mov r1, r5
	bl udiv32
	mul r2, r0, r4
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x13
	msr cpsr_cxsf, r1
	ldr sp, _stack_srv_end
	sub sp, sp, r2

	/* Initail smp by precessor id */
	mrc p15, 0, r0, c0, c0, 5
	and r0, #1
	mov r4, #0
	cmp r0, r4
	beq _master
_slave:
	wfe
	b _slave
_master:
	nop

	/* Copyself to link address */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq 1f
	bl sys_copyself
1:	nop

	/* Clear bss section */
	ldr r0, _bss_start
	ldr r2, _bss_end
	sub r2, r2, r0
	mov r1, #0
	bl memset

	/* Call _main */
	ldr r1, =_main
	mov pc, r1
_main:
	bl mmu_setup
	bl mmu_enable
	bl xboot_main
	b _main

	.global udiv32
udiv32:
	cmp r1, #0
	beq 3f
	mov r2, r1
	mov r1, r0
	mov r0, #0
	mov r3, #1
1:	cmp r2, #0
	blt 2f
	cmp r2, r1
	lslls r2, r2, #1
	lslls r3, r3, #1
	bls 1b
2:	cmp r1, r2
	subge r1, r1, r2
	addge r0, r0, r3
	lsr r2, r2, #1
	lsrs r3, r3, #1
	bcc 2b
3:	mov pc, lr

	.global return_to_fel
return_to_fel:
	ldr r0, =0x00047fe0
	ldr sp, [r0, #0]
	ldr lr, [r0, #4]
	ldr r1, [r0, #20]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #16]
	mcr p15, 0, r1, c12, c0, 0
	ldr r1, [r0, #12]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #8]
	msr cpsr, r1
	bx lr

/*
 * Exception handlers
 */
	.align 5
undefined_instruction:
	sub lr, lr, #4
	save_regs
	bl arm32_do_undefined_instruction
	restore_regs

	.align 5
software_interrupt:
	sub lr, lr, #4
	save_regs
	bl arm32_do_software_interrupt
	restore_regs

	.align 5
prefetch_abort:
	sub lr, lr, #4
	save_regs
	bl arm32_do_prefetch_abort
	restore_regs

	.align 5
data_abort:
	sub lr, lr, #8
	save_regs
	bl arm32_do_data_abort
	restore_regs

	.align 5
not_used:
	b .

	.align 5
irq:
	sub lr, lr, #4
	save_regs
	bl arm32_do_irq
	restore_regs

	.align 5
fiq:
	sub lr, lr, #4
	save_regs
	bl arm32_do_fiq
	restore_regs

/*
 * The location of section
 */
 	.align 4
_image_start:
	.long __image_start
_image_end:
	.long __image_end
_data_start:
	.long __data_start
_data_end:
	.long __data_end
_bss_start:
	.long __bss_start
_bss_end:
	.long __bss_end
_stack_und_start:
	.long __stack_und_start
_stack_und_end:
	.long __stack_und_end
_stack_abt_start:
	.long __stack_abt_start
_stack_abt_end:
	.long __stack_abt_end
_stack_irq_start:
	.long __stack_irq_start
_stack_irq_end:
	.long __stack_irq_end
_stack_fiq_start:
	.long __stack_fiq_start
_stack_fiq_end:
	.long __stack_fiq_end
_stack_srv_start:
	.long __stack_srv_start
_stack_srv_end:
	.long __stack_srv_end
_ddr_param_start:
	.long __ddr_param_start
_ddr_param_end:
	.long __ddr_param_end
_ddr_bin_start:
	.long __ddr_bin_start
_ddr_bin_end:
	.long __ddr_bin_end
