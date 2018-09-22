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

.macro save_regs
	str lr, [sp, #-4]
	mrs lr, spsr_all
	str lr, [sp, #-8]
	str r1, [sp, #-12]
	str r0, [sp, #-16]
	mov r0, sp
	mrs lr, cpsr_all
	and lr, lr, #~(0x1f)
	orr lr, lr, #0x13
	msr cpsr_c, lr
	ldr r1, [r0, #-4]
	str r1, [sp, #-4]!
	ldr r1, [r0, #-8]
	str r1, [sp, #-(4*16)]
	ldr r1, [r0, #-12]
	ldr r0, [r0, #-16]
	stmdb sp, {r0-r14}^
	sub sp, sp, #(4*16)
	ldr r4, [sp]
	and r0, r4, #0x1f
	cmp r0, #0x10
	beq 10f
	cmp r0, #0x13
	beq 11f
	b .
11:	add r1, sp, #(4*17)
	str r1, [sp, #(4*14)]
	str lr, [sp, #(4*15)]
10:	add r1, sp, #(4*17)
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
20:	ldr lr, [r12, #(4*15)]
	ldmia r12, {r0-r14}^
	movs pc, lr
21:	ldm r12, {r0-r15}^
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
	.long 0, __spl_size
	.byte 'S', 'P', 'L', 2
	.long 0, 0
	.long 0, 0, 0, 0, 0, 0, 0, 0
	.long 0, 0, 0, 0, 0, 0, 0, 0	/* 0x40 - boot params, 0x58 - fel boot type, 0x5c - dram size */

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
	/* Save boot params to 0x00000040 */
	ldr r0, =0x00000040
	str sp, [r0, #0]
	str lr, [r0, #4]
	mrs lr, cpsr
	str lr, [r0, #8]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #12]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #16]

	/* Check boot type just for fel */
	mov r0, #0x0
	ldr r1, [r0, #8]
	ldr r2, =0x4c45462e
	cmp r1, r2
	bne 1f
	ldr r1, =0x1
	str r1, [r0, #0x58]
1:	nop

	/* Enter svc mode and mask interrupts */
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	msr cpsr, r0

	/* Set vector to the low address */
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #(1<<13)
	mcr p15, 0, r0, c1, c0, 0

	/* Copy vector to the correct address */
	adr r0, _vector
	mrc p15, 0, r2, c1, c0, 0
	ands r2, r2, #(1 << 13)
	ldreq r1, =0x00000000
	ldrne r1, =0xffff0000
	ldmia r0!, {r2-r8, r10}
	stmia r1!, {r2-r8, r10}
	ldmia r0!, {r2-r8, r10}
	stmia r1!, {r2-r8, r10}

	/* Initial system clock, ddr add uart */
	bl sys_clock_init
	bl sys_dram_init
	bl sys_uart_init

	/* Boot speed up, leave slower sram */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq _speedup
	ldr r0, =0x81f80000
	adr r1, _start
	ldr r2, =__spl_size
	bl memcpy
	ldr r0, =_speedup
	ldr r1, =_start
	sub r0, r0, r1
	ldr r1, =0x81f80000
	add r0, r0, r1
	mov pc, r0
_speedup:
	nop

	/* Copyself to link address */
	adr r0, _start
	ldr r1, =_start
	cmp r0, r1
	beq 1f
	bl sys_copyself
1:	nop

	/* Initialize stacks */
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r1, r0, #0x1b
	msr cpsr_cxsf, r1
	ldr sp, _stack_und_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x17
	msr cpsr_cxsf, r1
	ldr sp, _stack_abt_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x12
	msr cpsr_cxsf, r1
	ldr sp, _stack_irq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x11
	msr cpsr_cxsf, r1
	ldr sp, _stack_fiq_end

	bic r0, r0, #0x1f
	orr r1, r0, #0x13
	msr cpsr_cxsf, r1
	ldr sp, _stack_srv_end

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
	mov r0, #0
	mov r1, #0
	bl xboot_main
	b _main

	.global return_to_fel
return_to_fel:
	mov r0, #0x4
	mov r1, #'e'
	strb r1, [r0, #0]
	mov r1, #'G'
	strb r1, [r0, #1]
	mov r1, #'O'
	strb r1, [r0, #2]
	mov r1, #'N'
	strb r1, [r0, #3]
	mov r1, #'.'
	strb r1, [r0, #4]
	mov r1, #'F'
	strb r1, [r0, #5]
	mov r1, #'E'
	strb r1, [r0, #6]
	mov r1, #'L'
	strb r1, [r0, #7]
	ldr r0, =0x00000040
	ldr sp, [r0, #0]
	ldr lr, [r0, #4]
	ldr r1, [r0, #16]
	mcr p15, 0, r1, c1, c0, 0
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
_stack_und_end:
	.long __stack_und_end
_stack_abt_end:
	.long __stack_abt_end
_stack_irq_end:
	.long __stack_irq_end
_stack_fiq_end:
	.long __stack_fiq_end
_stack_srv_end:
	.long __stack_srv_end
