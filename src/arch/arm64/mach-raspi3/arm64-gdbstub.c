/*
 * arm64-gdbstub.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <arm64.h>
#include <xboot/gdbstub.h>

struct arm64_env_t {
	struct {
		uint64_t x[31];
		uint64_t sp;
		uint64_t pc;
		uint64_t pstate;
	} regs;
	struct {
		virtual_addr_t addr;
		uint8_t instr[4];
	} step;
};
static struct arm64_env_t __arm64_env;

static void cpu_register_save(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm64_env_t * env = (struct arm64_env_t *)cpu->env;
	memcpy(&env->regs, regs, sizeof(env->regs));
}

static void cpu_register_restore(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm64_env_t * env = (struct arm64_env_t *)cpu->env;
	memcpy(regs, &env->regs, sizeof(env->regs));
}

static int cpu_register_read(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm64_env_t * env = (struct arm64_env_t *)cpu->env;

	if(n < 31)
	{
		memcpy(buf, &env->regs.x[n], 8);
		return 8;
	}
	else if(n == 31)
	{
		memcpy(buf, &env->regs.sp, 8);
		return 8;
	}
	else if(n == 32)
	{
		memcpy(buf, &env->regs.pc, 8);
		return 8;
	}
	else if(n == 33)
	{
		memcpy(buf, &env->regs.pstate, 8);
		return 8;
	}
	else if(n < 66)		/* v0 - v31 */
	{
		memcpy(buf, 0, 16);
		return 16;
	}
	else if(n == 66)	/* fpsr */
	{
		memcpy(buf, 0, 4);
		return 4;
	}
	else if(n == 67)	/* fpcr */
	{
		memcpy(buf, 0, 4);
		return 4;
	}
	return 0;
}

static int cpu_register_write(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm64_env_t * env = (struct arm64_env_t *)cpu->env;

	if(n < 31)
	{
		memcpy(&env->regs.x[n], buf, 8);
		return 8;
	}
	else if(n == 31)
	{
		memcpy(&env->regs.sp, buf, 8);
		return 8;
	}
	else if(n == 32)
	{
		memcpy(&env->regs.pc, buf, 8);
		return 8;
	}
	else if(n == 33)
	{
		memcpy(&env->regs.pstate, buf, 8);
		return 8;
	}
	else if(n < 66)		/* v0 - v31 */
	{
		return 16;
	}
	else if(n == 66)	/* fpsr */
	{
		return 4;
	}
	else if(n == 67)	/* fpcr */
	{
		return 4;
	}
	return 0;
}

static int cpu_breakpoint_insert(struct gdb_cpu_t * cpu, struct gdb_breakpoint_t * bp)
{
	const uint8_t bpinstr[4] = {0x00, 0x04, 0x00, 0xf2};	/* 0xf2000400 */

	switch(bp->type)
	{
	case BP_TYPE_SOFTWARE_BREAKPOINT:
		memcpy(bp->instr, (void *)(bp->addr), 4);
		memcpy((void *)(bp->addr), bpinstr, 4);
		return 0;
	case BP_TYPE_HARDWARE_BREAKPOINT:
	case BP_TYPE_WRITE_WATCHPOINT:
	case BP_TYPE_READ_WATCHPOINT:
	case BP_TYPE_ACCESS_WATCHPOINT:
	default:
		break;
	}
	return -1;
}

static int cpu_breakpoint_remove(struct gdb_cpu_t * cpu, struct gdb_breakpoint_t * bp)
{
	switch(bp->type)
	{
	case BP_TYPE_SOFTWARE_BREAKPOINT:
		memcpy((void *)(bp->addr), bp->instr, 4);
		return 0;
	case BP_TYPE_HARDWARE_BREAKPOINT:
	case BP_TYPE_WRITE_WATCHPOINT:
	case BP_TYPE_READ_WATCHPOINT:
	case BP_TYPE_ACCESS_WATCHPOINT:
	default:
		break;
	}
	return -1;
}

static int cpu_singlestep_active(struct gdb_cpu_t * cpu)
{
	return -1;
}

static int cpu_singlestep_finish(struct gdb_cpu_t * cpu)
{
	return -1;
}

static int cpu_memory_acess(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return 0;
}

static int cpu_processor(struct gdb_cpu_t * cpu)
{
	return arm64_smp_processor_id();
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ __volatile__("brk %0" : : "I" (0x401));
}

static struct gdb_cpu_t __arm64_gdb_cpu = {
	.nregs = 68,
	.register_save = cpu_register_save,
	.register_restore = cpu_register_restore,
	.register_read = cpu_register_read,
	.register_write = cpu_register_write,
	.breakpoint_insert = cpu_breakpoint_insert,
	.breakpoint_remove = cpu_breakpoint_remove,
	.singlestep_active = cpu_singlestep_active,
	.singlestep_finish = cpu_singlestep_finish,
	.memory_acess = cpu_memory_acess,
	.processor = cpu_processor,
	.breakpoint = cpu_breakpoint,
	.env = &__arm64_env,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	return &__arm64_gdb_cpu;
}
