/*
 * exception.c
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

#include <xboot.h>
#include <arm32.h>
#include <interrupt/interrupt.h>

struct arm_regs_t {
	uint32_t esp;
	uint32_t cpsr;
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
};

static void show_regs(struct arm_regs_t * regs)
{
	int i;

	LOG("pc : [<%08lx>] lr : [<%08lx>] cpsr: %08lx\r\n", regs->pc, regs->lr, regs->cpsr);
	LOG("sp : %08lx esp : %08lx\r\n", regs->sp, regs->esp);
	for(i = 12; i >= 0; i--)
		LOG("r%-2d: %08lx\r\n", i, regs->r[i]);
	LOG("\r\n");
}

void arm32_do_undefined_instruction(struct arm_regs_t * regs)
{
	show_regs(regs);
	regs->pc += 4;
}

void arm32_do_software_interrupt(struct arm_regs_t * regs)
{
	show_regs(regs);
	regs->pc += 4;
}

void arm32_do_prefetch_abort(struct arm_regs_t * regs)
{
	show_regs(regs);
	regs->pc += 4;
}

void arm32_do_data_abort(struct arm_regs_t * regs)
{
	show_regs(regs);
	regs->pc += 4;
}

void arm32_do_irq(struct arm_regs_t * regs)
{
	interrupt_handle_exception(regs);
}

void arm32_do_fiq(struct arm_regs_t * regs)
{
	interrupt_handle_exception(regs);
}
