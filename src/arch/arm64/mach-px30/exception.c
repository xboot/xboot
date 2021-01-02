/*
 * exception.c
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

#include <xboot.h>
#include <arm64.h>
#include <interrupt/interrupt.h>

struct pt_regs_t {
	uint64_t regs[31];
	uint64_t sp;
	uint64_t pc;
	uint64_t pstate;
	uint64_t orig_x0;
	uint64_t syscallno;
};

static const char * esr_class_str[] = {
	[0 ... 0x3f] = "UNRECOGNIZED EC",
	[0x00]	= "Unknown/Uncategorized",
	[0x01]	= "WFI/WFE",
	[0x03]	= "CP15 MCR/MRC",
	[0x04]	= "CP15 MCRR/MRRC",
	[0x05]	= "CP14 MCR/MRC",
	[0x06]	= "CP14 LDC/STC",
	[0x07]	= "ASIMD",
	[0x08]	= "CP10 MRC/VMRS",
	[0x0c]	= "CP14 MCRR/MRRC",
	[0x0e]	= "PSTATE.IL",
	[0x11]	= "SVC (AArch32)",
	[0x12]	= "HVC (AArch32)",
	[0x13]	= "SMC (AArch32)",
	[0x15]	= "SVC (AArch64)",
	[0x16]	= "HVC (AArch64)",
	[0x17]	= "SMC (AArch64)",
	[0x18]	= "MSR/MRS (AArch64)",
	[0x1f]	= "EL3 IMP DEF",
	[0x20]	= "IABT (lower EL)",
	[0x21]	= "IABT (current EL)",
	[0x22]	= "PC Alignment",
	[0x24]	= "DABT (lower EL)",
	[0x25]	= "DABT (current EL)",
	[0x26]	= "SP Alignment",
	[0x28]	= "FP (AArch32)",
	[0x2c]	= "FP (AArch64)",
	[0x2f]	= "SError",
	[0x30]	= "Breakpoint (lower EL)",
	[0x31]	= "Breakpoint (current EL)",
	[0x32]	= "Software Step (lower EL)",
	[0x33]	= "Software Step (current EL)",
	[0x34]	= "Watchpoint (lower EL)",
	[0x35]	= "Watchpoint (current EL)",
	[0x38]	= "BKPT (AArch32)",
	[0x3a]	= "Vector catch (AArch32)",
	[0x3c]	= "BRK (AArch64)",
};

static void show_regs(struct pt_regs_t * regs)
{
	int i;

	printf("pc : [<%016llx>] lr : [<%016llx>] pstate: %08llx\r\n", regs->pc, regs->regs[30], regs->pstate);
	printf("sp : %016llx\r\n", regs->sp);
	for(i = 29; i >= 0; i--)
	{
		printf("x%-2d: %016llx ", i, regs->regs[i]);
		if(i % 2 == 0)
			printf("\r\n");
	}
	printf("\r\n");
	while(1);
}

void arm64_invalid_exception(struct pt_regs_t * regs, int reason, unsigned int esr)
{
	const char * handler[] = {
		"Synchronous Abort",
		"IRQ",
		"FIQ",
		"Error"
	};

	printf("Invalid exception in %s handler detected, code 0x%08x -- %s\r\n", handler[reason], esr, esr_class_str[esr >> 26]);
	show_regs(regs);
}

void arm64_sync_exception(struct pt_regs_t * regs)
{
	uint64_t esr, far;
	uint64_t ec, iss;

	esr = arm64_read_sysreg(esr_el1);
	far = arm64_read_sysreg(far_el1);
	ec = (esr >> 26) & 0x3f;
	iss = (esr >> 0) & 0x1ffffff;

	switch(ec)
	{
	case 0x3c:	/* BRK (AArch64) */
		if(iss == 0x401)
			regs->pc += 4;
		return;

	default:
		break;
	}

	printf("Synchronous exception detected, ec:0x%x iss:0x%x far:0x%x\r\n", ec, iss, far);
	show_regs(regs);
}

void arm64_irq_exception(struct pt_regs_t * regs)
{
	interrupt_handle_exception(regs);
}
