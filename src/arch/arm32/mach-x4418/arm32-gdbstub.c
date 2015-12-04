/*
 * arm32-gdbstub.c
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

#include <xboot.h>
#include <xboot/gdbstub.h>

struct arm32_env_t {
	struct {
		uint32_t r[13];
		uint32_t sp;
		uint32_t lr;
		uint32_t pc;
		uint32_t cpsr;
	} regs;
	struct {
		int enable;
		uint8_t instruction[4];
	} step;
};
static struct arm32_env_t __arm32_env;

static void cpu_debug_begin(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	memcpy(&env->regs, regs, sizeof(env->regs));
	if(env->step.enable != 0)
	{
		env->step.enable = 0;
	}
}

static void cpu_debug_end(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	if(*(uint32_t *)(env->regs.pc) == 0xe7ffdeff)
		env->regs.pc += 4;
	memcpy(regs, &env->regs, sizeof(env->regs));
}

static int cpu_processor_id(struct gdb_cpu_t * cpu)
{
	return 0;
}

static int cpu_register_read(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;

	if(n < 13)
	{
		memcpy(buf, &env->regs.r[n], 4);
		return 4;
	}
	else if(n == 13)
	{
		memcpy(buf, &env->regs.sp, 4);
		return 4;
	}
	else if(n == 14)
	{
		memcpy(buf, &env->regs.lr, 4);
		return 4;
	}
	else if(n == 15)
	{
		memcpy(buf, &env->regs.pc, 4);
		return 4;
	}
	else if(n < 24)		/* f0 - f7 */
	{
        memset(buf, 0, 12);
        return 12;
	}
	else if(n == 24)	/* fps */
	{
		memset(buf, 0, 4);
		return 4;
	}
	else if(n == 25)
	{
		memcpy(buf, &env->regs.cpsr, 4);
		return 4;
	}
	return 0;
}

static int cpu_register_write(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;

	if(n < 13)
	{
		memcpy(&env->regs.r[n], buf, 4);
		return 4;
	}
	else if(n == 13)
	{
		memcpy(&env->regs.sp, buf, 4);
		return 4;
	}
	else if(n == 14)
	{
		memcpy(&env->regs.lr, buf, 4);
		return 4;
	}
	else if(n == 15)
	{
		memcpy(&env->regs.pc, buf, 4);
		return 4;
	}
	else if(n < 24)		/* f0 - f7 */
	{
		return 12;
	}
	else if(n == 24)	/* fps */
	{
		return 4;
	}
	else if(n == 25)
	{
		memcpy(&env->regs.cpsr, buf, 4);
		return 4;
	}
	return 0;
}

static int cpu_acess_memory(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return 0;
}

static int cpu_breakpoint_insert(struct gdb_cpu_t * cpu, struct gdb_breakpoint_t * bp)
{
	const uint8_t bpinstr[4] = {0xfe, 0xde, 0xff, 0xe7};	/* 0xe7ffdefe */

	switch(bp->type)
	{
	case BP_TYPE_SOFTWARE_BREAKPOINT:
		memcpy(bp->instruction, (void *)(bp->addr), 4);
		memcpy((void *)(bp->addr), bpinstr, 4);
		return 0;
	case BP_TYPE_HARDWARE_BREAKPOINT:
	case BT_TYPE_WRITE_WATCHPOINT:
	case BT_TYPE_READ_WATCHPOINT:
	case BT_TYPE_ACCESS_WATCHPOINT:
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
		memcpy((void *)(bp->addr), bp->instruction, 4);
		return 0;
	case BP_TYPE_HARDWARE_BREAKPOINT:
	case BT_TYPE_WRITE_WATCHPOINT:
	case BT_TYPE_READ_WATCHPOINT:
	case BT_TYPE_ACCESS_WATCHPOINT:
	default:
		break;
	}
	return -1;
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ __volatile__(".word 0xe7ffdeff");
}

static void cpu_singlestep(struct gdb_cpu_t * cpu)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	env->step.enable = 1;
}

static struct gdb_cpu_t __arm32_gdb_cpu = {
	.nregs = 26,
	.processor_id = cpu_processor_id,
	.debug_begin = cpu_debug_begin,
	.debug_end = cpu_debug_end,
	.register_read = cpu_register_read,
	.register_write = cpu_register_write,
	.acess_memory = cpu_acess_memory,
	.breakpoint_insert = cpu_breakpoint_insert,
	.breakpoint_remove = cpu_breakpoint_remove,
	.breakpoint = cpu_breakpoint,
	.singlestep = cpu_singlestep,
	.env = &__arm32_env,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	return &__arm32_gdb_cpu;
}
