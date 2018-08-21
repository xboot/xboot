/*
 * exception.c
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

#include <xboot.h>
#include <arm64.h>
#include <interrupt/interrupt.h>
#include <xboot/gdbstub.h>

struct arm64_regs_t {
	uint64_t x[30];
	uint64_t lr;
	uint64_t sp;
	uint64_t pc;
	uint64_t pstate;
};

static void show_regs(struct arm64_regs_t * regs)
{
	int i;

	printf("pc : [<%016llx>] lr : [<%016llx>] pstate: %08llx\r\n", regs->pc, regs->lr, regs->pstate);
	printf("sp : %016llx\r\n", regs->sp);
	for(i = 29; i >= 0; i--)
	{
		printf("x%-2d: %016llx ", i, regs->x[i]);
		if(i % 2 == 0)
			printf("\r\n");
	}
	printf("\r\n");
	while(1);
}

void arm64_invalid_exception(struct arm64_regs_t * regs, int reason)
{
	const char * handler[] = {
		"Synchronous Abort",
		"IRQ",
		"FIQ",
		"Error"
	};

	printf("Invalid exception in %s handler detected, code 0x%02x\r\n", handler[reason & 0x3], reason);
	show_regs(regs);
}

void arm64_sync_exception(struct arm64_regs_t * regs)
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
		gdbserver_handle_exception(regs);
		return;

	default:
		break;
	}

	printf("Synchronous exception detected, ec:0x%x iss:0x%x far:0x%x\r\n", ec, iss, far);
	show_regs(regs);
}

void arm64_irq_exception(struct arm64_regs_t * regs)
{
	interrupt_handle_exception(regs);
}
