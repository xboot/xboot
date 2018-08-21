/*
 * transition.S
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

/*
 * SCTLR_EL1/SCTLR_EL2/SCTLR_EL3 bits definitions
 */
#define CR_M					(1 << 0)	/* MMU enable */
#define CR_A					(1 << 1)	/* Alignment abort enable */
#define CR_C					(1 << 2)	/* Dcache enable */
#define CR_SA					(1 << 3)	/* Stack Alignment Check Enable	*/
#define CR_I					(1 << 12)	/* Icache enable */
#define CR_WXN					(1 << 19)	/* Write Permision Imply XN */
#define CR_EE					(1 << 25)	/* Exception (Big) Endian */

/*
 * SCR_EL3 bits definitions
 */
#define SCR_EL3_RW_AARCH64		(1 << 10)	/* Next lower level is AArch64 */
#define SCR_EL3_RW_AARCH32		(0 << 10)	/* Lower lowers level are AArch32 */
#define SCR_EL3_HCE_EN			(1 << 8)	/* Hypervisor Call enable */
#define SCR_EL3_SMD_DIS			(1 << 7)	/* Secure Monitor Call disable */
#define SCR_EL3_RES1			(3 << 4)	/* Reserved, RES1 */
#define SCR_EL3_NS_EN			(1 << 0)	/* EL0 and EL1 in Non-scure state */

/*
 * SPSR_EL3/SPSR_EL2 bits definitions
 */
#define SPSR_EL_END_LE			(0 << 9)	/* Exception Little-endian */
#define SPSR_EL_DEBUG_MASK		(1 << 9)	/* Debug exception masked */
#define SPSR_EL_ASYN_MASK		(1 << 8)	/* Asynchronous data abort masked */
#define SPSR_EL_SERR_MASK		(1 << 8)	/* System Error exception masked */
#define SPSR_EL_IRQ_MASK		(1 << 7)	/* IRQ exception masked */
#define SPSR_EL_FIQ_MASK		(1 << 6)	/* FIQ exception masked */
#define SPSR_EL_T_A32			(0 << 5)	/* AArch32 instruction set A32 */
#define SPSR_EL_M_AARCH64		(0 << 4)	/* Exception taken from AArch64 */
#define SPSR_EL_M_AARCH32		(1 << 4)	/* Exception taken from AArch32 */
#define SPSR_EL_M_SVC			(0x3)		/* Exception taken from SVC mode */
#define SPSR_EL_M_HYP			(0xa)		/* Exception taken from HYP mode */
#define SPSR_EL_M_EL1H			(5)			/* Exception taken from EL1h mode */
#define SPSR_EL_M_EL2H			(9)			/* Exception taken from EL2h mode */

/*
 * CPTR_EL2 bits definitions
 */
#define CPTR_EL2_RES1			(3 << 12 | 0x3ff)	/* Reserved, RES1 */

/*
 * SCTLR_EL2 bits definitions
 */
#define SCTLR_EL2_RES1			(3 << 28 | 3 << 22 | 1 << 18 | 1 << 16 | 1 << 11 | 3 << 4)	/* Reserved, RES1 */
#define SCTLR_EL2_EE_LE			(0 << 25)	/* Exception Little-endian */
#define SCTLR_EL2_WXN_DIS		(0 << 19)	/* Write permission is not XN */
#define SCTLR_EL2_ICACHE_DIS	(0 << 12)	/* Instruction cache disabled */
#define SCTLR_EL2_SA_DIS		(0 << 3)	/* Stack Alignment Check disabled */
#define SCTLR_EL2_DCACHE_DIS	(0 << 2)	/* Data cache disabled */
#define SCTLR_EL2_ALIGN_DIS		(0 << 1)	/* Alignment check disabled */
#define SCTLR_EL2_MMU_DIS		(0)			/* MMU disabled */

/*
 * CNTHCTL_EL2 bits definitions
 */
#define CNTHCTL_EL2_EL1PCEN_EN	(1 << 1)	/* Physical timer regs accessible */
#define CNTHCTL_EL2_EL1PCTEN_EN	(1 << 0)	/* Physical counter accessible */

/*
 * HCR_EL2 bits definitions
 */
#define HCR_EL2_RW_AARCH64		(1 << 31)	/* EL1 is AArch64 */
#define HCR_EL2_RW_AARCH32		(0 << 31)	/* Lower levels are AArch32 */
#define HCR_EL2_HCD_DIS			(1 << 29)	/* Hypervisor Call disabled */

/*
 * CPACR_EL1 bits definitions
 */
#define CPACR_EL1_FPEN_EN		(3 << 20)	/* SIMD and FP instruction enabled */

/*
 * SCTLR_EL1 bits definitions
 */
#define SCTLR_EL1_RES1			(3 << 28 | 3 << 22 | 1 << 20 | 1 << 11) /* Reserved, RES1 */
#define SCTLR_EL1_UCI_DIS		(0 << 26)	/* Cache instruction disabled */
#define SCTLR_EL1_EE_LE			(0 << 25)	/* Exception Little-endian */
#define SCTLR_EL1_WXN_DIS		(0 << 19)	/* Write permission is not XN */
#define SCTLR_EL1_NTWE_DIS		(0 << 18)	/* WFE instruction disabled */
#define SCTLR_EL1_NTWI_DIS		(0 << 16)	/* WFI instruction disabled */
#define SCTLR_EL1_UCT_DIS		(0 << 15)	/* CTR_EL0 access disabled */
#define SCTLR_EL1_DZE_DIS		(0 << 14)	/* DC ZVA instruction disabled */
#define SCTLR_EL1_ICACHE_DIS	(0 << 12)	/* Instruction cache disabled */
#define SCTLR_EL1_UMA_DIS		(0 << 9)	/* User Mask Access disabled */
#define SCTLR_EL1_SED_EN		(0 << 8)	/* SETEND instruction enabled */
#define SCTLR_EL1_ITD_EN		(0 << 7)	/* IT instruction enabled */
#define SCTLR_EL1_CP15BEN_DIS	(0 << 5)	/* CP15 barrier operation disabled */
#define SCTLR_EL1_SA0_DIS		(0 << 4)	/* Stack Alignment EL0 disabled */
#define SCTLR_EL1_SA_DIS		(0 << 3)	/* Stack Alignment EL1 disabled */
#define SCTLR_EL1_DCACHE_DIS	(0 << 2)	/* Data cache disabled */
#define SCTLR_EL1_ALIGN_DIS		(0 << 1)	/* Alignment check disabled */
#define SCTLR_EL1_MMU_DIS		(0)			/* MMU disabled */

/*
 * Branch according to exception level
 */
.macro switch_el, xreg, el3_label, el2_label, el1_label
	mrs	\xreg, CurrentEL
	cmp	\xreg, 0xc
	b.eq \el3_label
	cmp	\xreg, 0x8
	b.eq \el2_label
	cmp	\xreg, 0x4
	b.eq \el1_label
.endm

/*
 * Switch from EL3 to EL2 for ARMv8
 */
.macro armv8_switch_to_el2_m, ep, tmp
	msr	cptr_el3, xzr		/* Disable coprocessor traps to EL3 */
	mov	\tmp, #CPTR_EL2_RES1
	msr	cptr_el2, \tmp		/* Disable coprocessor traps to EL2 */

	/* Initialize Generic Timers */
	msr	cntvoff_el2, xzr

	/*
	 * Initialize SCTLR_EL2 setting RES1 bits (29,28,23,22,18,16,11,5,4) to 1
	 * and RES0 bits (31,30,27,26,24,21,20,17,15-13,10-6) + EE,WXN,I,SA,C,A,M to 0
	 */
	ldr	\tmp, =(SCTLR_EL2_RES1 | SCTLR_EL2_EE_LE | \
			SCTLR_EL2_WXN_DIS | SCTLR_EL2_ICACHE_DIS | \
			SCTLR_EL2_SA_DIS | SCTLR_EL2_DCACHE_DIS | \
			SCTLR_EL2_ALIGN_DIS | SCTLR_EL2_MMU_DIS)
	msr	sctlr_el2, \tmp

	mov	\tmp, sp
	msr	sp_el2, \tmp		/* Migrate SP */
	mrs	\tmp, vbar_el3
	msr	vbar_el2, \tmp		/* Migrate VBAR */

	/*
	 * The next lower exception level is AArch64, 64bit EL2 | HCE |
	 * RES1 (Bits[5:4]) | Non-secure EL0/EL1.
	 * and the SMD depends on requirements.
	 */
	ldr	\tmp, =(SCR_EL3_RW_AARCH64 | SCR_EL3_HCE_EN | \
			SCR_EL3_SMD_DIS | SCR_EL3_RES1 | SCR_EL3_NS_EN)
	msr	scr_el3, \tmp

	/* Return to the EL2_SP2 mode from EL3 */
	ldr	\tmp, =(SPSR_EL_DEBUG_MASK | SPSR_EL_SERR_MASK | \
			SPSR_EL_IRQ_MASK | SPSR_EL_FIQ_MASK | \
			SPSR_EL_M_AARCH64 | SPSR_EL_M_EL2H)
	msr	spsr_el3, \tmp
	msr	elr_el3, \ep
	eret
.endm

/*
 * Switch from EL2 to EL1 for ARMv8
 */
.macro armv8_switch_to_el1_m, ep, tmp
	/* Initialize Generic Timers */
	mrs	\tmp, cnthctl_el2
	/* Enable EL1 access to timers */
	orr	\tmp, \tmp, #(CNTHCTL_EL2_EL1PCEN_EN | CNTHCTL_EL2_EL1PCTEN_EN)
	msr	cnthctl_el2, \tmp
	msr	cntvoff_el2, xzr

	/* Initilize MPID/MPIDR registers */
	mrs	\tmp, midr_el1
	msr	vpidr_el2, \tmp
	mrs	\tmp, mpidr_el1
	msr	vmpidr_el2, \tmp

	/* Disable coprocessor traps */
	mov	\tmp, #CPTR_EL2_RES1
	msr	cptr_el2, \tmp		/* Disable coprocessor traps to EL2 */
	msr	hstr_el2, xzr		/* Disable coprocessor traps to EL2 */
	mov	\tmp, #CPACR_EL1_FPEN_EN
	msr	cpacr_el1, \tmp		/* Enable FP/SIMD at EL1 */

	/*
	 * Initialize SCTLR_EL1 setting RES1 bits (29,28,23,22,20,11) to 1
	 * and RES0 bits (31,30,27,21,17,13,10,6) +
	 * UCI,EE,EOE,WXN,nTWE,nTWI,UCT,DZE,I,UMA,SED,ITD,
	 * CP15BEN,SA0,SA,C,A,M to 0
	 */
	ldr	\tmp, =(SCTLR_EL1_RES1 | SCTLR_EL1_UCI_DIS | \
			SCTLR_EL1_EE_LE | SCTLR_EL1_WXN_DIS | \
			SCTLR_EL1_NTWE_DIS | SCTLR_EL1_NTWI_DIS | \
			SCTLR_EL1_UCT_DIS | SCTLR_EL1_DZE_DIS | \
			SCTLR_EL1_ICACHE_DIS | SCTLR_EL1_UMA_DIS | \
			SCTLR_EL1_SED_EN | SCTLR_EL1_ITD_EN | \
			SCTLR_EL1_CP15BEN_DIS | SCTLR_EL1_SA0_DIS | \
			SCTLR_EL1_SA_DIS | SCTLR_EL1_DCACHE_DIS | \
			SCTLR_EL1_ALIGN_DIS | SCTLR_EL1_MMU_DIS)
	msr	sctlr_el1, \tmp

	mov	\tmp, sp
	msr	sp_el1, \tmp		/* Migrate SP */
	mrs	\tmp, vbar_el2
	msr	vbar_el1, \tmp		/* Migrate VBAR */

	/* Initialize HCR_EL2 */
	ldr	\tmp, =(HCR_EL2_RW_AARCH64 | HCR_EL2_HCD_DIS)
	msr	hcr_el2, \tmp

	/* Return to the EL1_SP1 mode from EL2 */
	ldr	\tmp, =(SPSR_EL_DEBUG_MASK | SPSR_EL_SERR_MASK | \
			SPSR_EL_IRQ_MASK | SPSR_EL_FIQ_MASK | \
			SPSR_EL_M_AARCH64 | SPSR_EL_M_EL1H)
	msr spsr_el2, \tmp
	msr elr_el2, \ep
	eret
.endm

ENTRY(armv8_switch_to_el2)
	switch_el x4, 1f, 0f, 0f
0:	br x3
1:	armv8_switch_to_el2_m x3, x4
ENDPROC(armv8_switch_to_el2)

ENTRY(armv8_switch_to_el1)
	switch_el x4, 0f, 1f, 0f
0:	br x3
1:	armv8_switch_to_el1_m x3, x4
ENDPROC(armv8_switch_to_el1)
