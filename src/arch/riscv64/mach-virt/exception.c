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
#include <riscv64.h>
#include <interrupt/interrupt.h>
#include <xboot/gdbstub.h>

#if defined(__riscv_flen)
#if __riscv_flen >= 32
extern void f32_read(int n, uint32_t * v);
extern void f32_write(int n, uint32_t * v);
#endif
#if __riscv_flen >= 64
extern void f64_read(int n, uint64_t * v);
extern void f64_write(int n, uint64_t * v);
#endif
#endif

#define EXTRACT_FIELD(val, which)			(((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval)	(((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

struct pt_regs_t {
	unsigned long x[32];
	unsigned long status;
	unsigned long epc;
	unsigned long badvaddr;
	unsigned long cause;
	unsigned long insn;
};

struct insn_fetch_t {
	unsigned long error;
	uint32_t insn;
};

union endian_buf_t {
	uint8_t  b[8];
	uint16_t h[4];
	uint32_t w[2];
	uint64_t d[1];
	unsigned long v;
};

struct instruction_info_t {
	uint32_t opcode;
	uint32_t mask;
	unsigned int reg_shift;
	unsigned int reg_mask;
	unsigned int reg_addition;
	unsigned int is_fp : 1;
	unsigned int is_load : 1;
	unsigned int width : 8;
	unsigned int sign_extend : 1;
};

static struct irq_handler_t core_interrupt_handler[8] = { 0 };

static struct instruction_info_t insn_info[] = {
#if __riscv_xlen == 128
	{ 0x00002000, 0x0000e003,  2,  7, 8, 0, 1, 16, 1},	/* C.LQ */
#else
	{ 0x00002000, 0x0000e003,  2,  7, 8, 1, 1,  8, 0},	/*  C.FLD */
#endif
	{ 0x00004000, 0x0000e003,  2,  7, 8, 0, 1,  4, 1},	/*  C.LW */
#if __riscv_xlen == 32
	{ 0x00006000, 0x0000e003,  2,  7, 8, 1, 1,  4, 0},	/*  C.FLW */
#else
	{ 0x00006000, 0x0000e003,  2,  7, 8, 0, 1,  8, 1},	/*  C.LD */
#endif

#if __riscv_xlen == 128
	{ 0x0000a000, 0x0000e003,  2,  7, 8, 0, 0, 16, 0},	/*  C.SQ */
#else
	{ 0x0000a000, 0x0000e003,  2,  7, 8, 1, 0,  8, 0},	/*  C.FSD */
#endif
	{ 0x0000c000, 0x0000e003,  2,  7, 8, 0, 0,  4, 0},	/*  C.SW */
#if __riscv_xlen == 32
	{ 0x0000e000, 0x0000e003,  2,  7, 8, 1, 0,  4, 0},	/*  C.FSW */
#else
	{ 0x0000e000, 0x0000e003,  2,  7, 8, 0, 0,  8, 0},	/*  C.SD */
#endif

#if __riscv_xlen == 128
	{ 0x00002002, 0x0000e003,  7, 15, 0, 0, 1, 16, 1},	/*  C.LQSP */
#else
	{ 0x00002002, 0x0000e003,  7, 15, 0, 1, 1,  8, 0},	/*  C.FLDSP */
#endif
	{ 0x00004002, 0x0000e003,  7, 15, 0, 0, 1,  4, 1},	/*  C.LWSP */
#if __riscv_xlen == 32
	{ 0x00006002, 0x0000e003,  7, 15, 0, 1, 1,  4, 0},	/*  C.FLWSP */
#else
	{ 0x00006002, 0x0000e003,  7, 15, 0, 0, 1,  8, 1},	/*  C.LDSP */
#endif

#if __riscv_xlen == 128
	{ 0x0000a002, 0x0000e003,  2, 15, 0, 0, 0, 16, 0},	/*  C.SQSP */
#else
	{ 0x0000a002, 0x0000e003,  2, 15, 0, 1, 0,  8, 0},	/*  C.FSDSP */
#endif
	{ 0x0000c002, 0x0000e003,  2, 15, 0, 0, 0,  4, 0},	/*  C.SWSP */
#if __riscv_xlen == 32
	{ 0x0000e002, 0x0000e003,  2, 15, 0, 1, 0,  4, 0},	/*  C.FSWSP */
#else
	{ 0x0000e002, 0x0000e003,  2, 15, 0, 0, 0,  8, 0},	/*  C.SDSP */
#endif

	{ 0x00000003, 0x0000707f,  7, 15, 0, 0, 1,  1, 1},	/*  LB */
	{ 0x00001003, 0x0000707f,  7, 15, 0, 0, 1,  2, 1},	/*  LH */
	{ 0x00002003, 0x0000707f,  7, 15, 0, 0, 1,  4, 1},	/*  LW */
#if __riscv_xlen > 32
	{ 0x00003003, 0x0000707f,  7, 15, 0, 0, 1,  8, 1},	/*  LD */
#endif
	{ 0x00004003, 0x0000707f,  7, 15, 0, 0, 1,  1, 0},	/*  LBU */
	{ 0x00005003, 0x0000707f,  7, 15, 0, 0, 1,  2, 0},	/*  LHU */
	{ 0x00006003, 0x0000707f,  7, 15, 0, 0, 1,  4, 0},	/*  LWU */

	{ 0x00000023, 0x0000707f, 20, 15, 0, 0, 0,  1, 0},	/*  SB */
	{ 0x00001023, 0x0000707f, 20, 15, 0, 0, 0,  2, 0},	/*  SH */
	{ 0x00002023, 0x0000707f, 20, 15, 0, 0, 0,  4, 0},	/*  SW */
#if __riscv_xlen > 32
	{ 0x00003023, 0x0000707f, 20, 15, 0, 0, 0,  8, 0},	/*  SD */
#endif

#if defined(__riscv_flen)
#if __riscv_flen >= 32
	{ 0x00002007, 0x0000707f,  7, 15, 0, 1, 1,  4, 0},	/*  FLW */
	{ 0x00003007, 0x0000707f,  7, 15, 0, 1, 1,  8, 0},	/*  FLD */
#endif

#if __riscv_flen >= 64
	{ 0x00002027, 0x0000707f, 20, 15, 0, 1, 0,  4, 0},	/*  FSW */
	{ 0x00003027, 0x0000707f, 20, 15, 0, 1, 0,  8, 0},	/*  FSD */
#endif
#endif
};

static const char * interrupt_names[] = {
	"User software interrupt",
	"Supervisor software interrupt",
	"Hypervisor software interrupt",
	"Machine software interrupt",
	"User timer interrupt",
	"Supervisor timer interrupt",
	"Hypervisor timer interrupt",
	"Machine timer interrupt",
	"User external interrupt",
	"Supervisor external interrupt",
	"Hypervisor external interrupt",
	"Machine external interrupt",
};

static const char * exception_names[] = {
	"Instruction address misaligned",
	"Instruction access fault",
	"Illegal instruction",
	"Breakpoint",
	"Load address misaligned",
	"Load access fault",
	"Store address misaligned",
	"Store access fault",
	"Environment call from U-mode",
	"Environment call from S-mode",
	"Reserved (10)",
	"Environment call from M-mode",
	"Instruction page fault",
	"Load page fault",
	"Reserved (14)",
	"Store page fault",
};

static const char * mstatus_to_previous_mode(unsigned long ms)
{
	switch((ms >> 11) & 0x3)
	{
		case 0x0:
			return "User";
		case 0x1:
			return "Supervisor";
		case 0x2:
			return "Hypervisor";
		case 0x3:
			return "Machine";
		default:
			break;
	}
	return "unknown";
}

static void show_regs(struct pt_regs_t * regs)
{
	if(regs->cause & (1UL << 63))
	{
		if((regs->cause & ~(1UL << 63)) < ARRAY_SIZE(interrupt_names))
			LOG("Interrupt:          %s", interrupt_names[regs->cause & ~(1UL << 63)]);
		else
			LOG("Trap:               Unknown cause %p", (void *)regs->cause);
	}
	else
	{
		if(regs->cause < ARRAY_SIZE(exception_names))
			LOG("Exception:          %s", exception_names[regs->cause]);
		else
			LOG("Trap:               Unknown cause %p", (void *)regs->cause);
	}
	LOG("Previous mode:      %s%s", mstatus_to_previous_mode(csr_read(mstatus)), (regs->status & (1 << 17)) ? " (MPRV)" : "");
	LOG("Bad instruction pc: %p", (void *)regs->epc);
	LOG("Bad address:        %p", (void *)regs->badvaddr);
	LOG("Stored ra:          %p", (void*) regs->x[1]);
	LOG("Stored sp:          %p", (void*) regs->x[2]);
}

static struct instruction_info_t * match_instruction(unsigned long insn)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(insn_info); i++)
		if((insn_info[i].mask & insn) == insn_info[i].opcode)
			return &(insn_info[i]);
	return NULL;
}


static int fetch_16bit_instruction(unsigned long vaddr, unsigned long * insn)
{
	uint16_t ins = mprv_read_mxr_u16((uint16_t *)vaddr);
	if(EXTRACT_FIELD(ins, 0x3) != 3)
	{
		*insn = ins;
		return 0;
	}
	return -1;
}

static int fetch_32bit_instruction(unsigned long vaddr, unsigned long * insn)
{
	uint32_t l = (uint32_t)mprv_read_mxr_u16((uint16_t *)vaddr + 0);
	uint32_t h = (uint32_t)mprv_read_mxr_u16((uint16_t *)vaddr + 2);
	uint32_t ins = (h << 16) | l;
	if((EXTRACT_FIELD(ins, 0x3) == 3) && (EXTRACT_FIELD(ins, 0x1c) != 0x7))
	{
		*insn = ins;
		return 0;
	}
	return -1;
}

static void redirect_trap(void)
{
	csr_write(sbadaddr, csr_read(mbadaddr));
	csr_write(sepc, csr_read(mepc));
	csr_write(scause, csr_read(mcause));
	csr_write(mepc, csr_read(stvec));

	unsigned long status = csr_read(mstatus);
	unsigned long mpp = EXTRACT_FIELD(status, 0x00001800);
	status = INSERT_FIELD(status, 0x00001800, 1);
	status = INSERT_FIELD(status, 0x00000100, mpp & 1);
	csr_write(mstatus, status);
}

static void handle_misaligned(struct pt_regs_t * regs)
{
	struct instruction_info_t * match;
	unsigned long insn = 0;
	union endian_buf_t buff;
	uint8_t * addr;
	int done, n;

	/* Try to fetch 16 / 32 bits instruction */
	if(fetch_16bit_instruction(regs->epc, &insn))
	{
		if(fetch_32bit_instruction(regs->epc, &insn))
			redirect_trap();
	}

	/* Matching instruction */
	match = match_instruction(insn);
	if(!match)
	{
		redirect_trap();
		return;
	}

	n = ((insn >> match->reg_shift) & match->reg_mask);
	n = n + match->reg_addition;
	buff.v = 0;
	if(match->is_load)
	{
		/* Load operation */
		/* Reading from memory by bytes prevents misaligned memory access */
		for(int i = 0; i < match->width; i++)
		{
			uint8_t * addr = (uint8_t *)(regs->badvaddr + i);
			buff.b[i] = mprv_read_u8(addr);
		}

		/* Sign extend for signed integer loading */
		if(match->sign_extend)
		{
			if(buff.v >> (8 * match->width - 1))
				buff.v |= -1 << (8 * match->width);
		}

		/* Write to register */
		if(match->is_fp)
		{
			int done = 0;
#if defined(__riscv_flen)
#if __riscv_flen >= 32
			/* Single-precision floating-point */
			if(match->width == 4)
			{
				f32_write(n, buff.w);
				done = 1;
			}
#endif
#if __riscv_flen >= 64
			/* Double-precision floating-point */
			if(match->width == 8)
			{
				f64_write(n, buff.d);
				done = 1;
			}
#endif
#endif
			if(!done)
				redirect_trap();
		}
		else
		{
			regs->x[n] = buff.v;
		}
	}
	else
	{
		/* Store operation */
		/* Reading from register */
		if(match->is_fp)
		{
			done = 0;
#if defined(__riscv_flen)
#if __riscv_flen >= 32
			if (match->width == 4) {
				f32_read(n, buff.w);
				done = 1;
			}
#endif
#if __riscv_flen >= 64
			if (match->width == 8) {
				f64_read(n, buff.d);
				done = 1;
			}
#endif
#endif
			if(!done)
				redirect_trap();
		}
		else
		{
			buff.v = regs->x[n];
		}

		/* Writing to memory by bytes prevents misaligned memory access */
		for(int i = 0; i < match->width; i++)
		{
			addr = (uint8_t *)(regs->badvaddr + i);
			mprv_write_u8(addr, buff.b[i]);
		}
	}
}

void riscv64_handle_exception(struct pt_regs_t * regs)
{
	csr_write(mscratch, regs);
	if(regs->cause & (1UL << 63))
	{
		unsigned long cause = regs->cause & ~(1UL << 63);
		unsigned long pending = csr_read(mip) & (1 << cause);
		switch(cause)
		{
		case 0:		/* User software interrupt */
		case 1:		/* Supervisor software interrupt */
		case 2:		/* Hypervisor software interrupt */
		case 3:		/* Machine software interrupt */
		case 4:		/* User timer interrupt */
		case 5:		/* Supervisor timer interrupt */
		case 6:		/* Hypervisor timer interrupt */
		case 7:		/* Machine timer interrupt */
			csr_clear(mip, pending);
			(core_interrupt_handler[cause].func)(core_interrupt_handler[cause].data);
			break;
		case 8:		/* User external interrupt */
		case 9:		/* Supervisor external interrupt */
		case 10:	/* Hypervisor external interrupt */
		case 11:	/* Machine external interrupt */
			csr_clear(mip, pending);
			interrupt_handle_exception(regs);
			break;
		default:
			show_regs(regs);
			break;
		}
	}
	else
	{
		switch(regs->cause)
		{
		case 0x0:	/* Misaligned fetch */
		case 0x1:	/* Fetch access */
		case 0x2:	/* Illegal instruction */
		case 0x3:	/* Breakpoint */
			show_regs(regs);
			break;
		case 0x4:	/* Misaligned load */
			handle_misaligned(regs);
			break;
		case 0x5:	/* Load acces */
			show_regs(regs);
			break;
		case 0x6:	/* Misaligned store */
			handle_misaligned(regs);
			break;
		case 0x7:	/* Store accesss */
		case 0x8:	/* User ecall */
		case 0x9:	/* Supervisor ecall */
		case 0xa:	/* Hypervisor ecall */
		case 0xb:	/* Machine ecall */
			show_regs(regs);
			break;
		default:
			show_regs(regs);
			break;
		}
	}
}

static void dummy_interrupt_function(void * data)
{
}

void hook_core_interrupt(int cause, void (*func)(void *), void * data)
{
	if(cause < ARRAY_SIZE(core_interrupt_handler))
	{
		if(func)
		{
			core_interrupt_handler[cause].func = func;
			core_interrupt_handler[cause].data = data;
		}
		else
		{
			core_interrupt_handler[cause].func = dummy_interrupt_function;
			core_interrupt_handler[cause].data = NULL;
		}
	}
}

static __init void exception_pure_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(core_interrupt_handler); i++)
	{
		core_interrupt_handler[i].func = dummy_interrupt_function;
		core_interrupt_handler[i].data = NULL;
	}
}
pure_initcall(exception_pure_init);
