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
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
};
static struct arm32_env_t __arm32_env;

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
	case BT_TYPE_POKE_WATCHPOINT:
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
	case BT_TYPE_POKE_WATCHPOINT:
	default:
		break;
	}
	return -1;
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ __volatile__(".word 0xe7ffdeff");
}

static void cpu_register_save(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	memcpy(env, regs, sizeof(struct arm32_env_t));
}

static void cpu_register_restore(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	if(*(uint32_t *)(env->pc) == 0xe7ffdeff)
		env->pc += 4;
	memcpy(regs, env, sizeof(struct arm32_env_t));
}

static int cpu_register_read(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;

	if(n < 13)
	{
		memcpy(buf, &env->r[n], 4);
		return 4;
	}
	else if(n == 13)
	{
		memcpy(buf, &env->sp, 4);
		return 4;
	}
	else if(n == 14)
	{
		memcpy(buf, &env->lr, 4);
		return 4;
	}
	else if(n == 15)
	{
		memcpy(buf, &env->pc, 4);
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
		memcpy(buf, &env->cpsr, 4);
		return 4;
	}
	return 0;
}

static int cpu_register_write(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;

	if(n < 13)
	{
		memcpy(&env->r[n], buf, 4);
		return 4;
	}
	else if(n == 13)
	{
		memcpy(&env->sp, buf, 4);
		return 4;
	}
	else if(n == 14)
	{
		memcpy(&env->lr, buf, 4);
		return 4;
	}
	else if(n == 15)
	{
		memcpy(&env->pc, buf, 4);
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
		memcpy(&env->cpsr, buf, 4);
		return 4;
	}
	return 0;
}

static int cpu_set_program_counter(struct gdb_cpu_t * cpu, virtual_addr_t addr)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	env->pc = (uint32_t)addr;
	return 0;
}

static int cpu_acess_memory(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return 0;
}

static struct gdb_cpu_t __arm32_gdb_cpu = {
	.nregs = 26,
	.register_save = cpu_register_save,
	.register_restore = cpu_register_restore,
	.register_read = cpu_register_read,
	.register_write = cpu_register_write,
	.set_program_counter = cpu_set_program_counter,
	.acess_memory = cpu_acess_memory,
	.breakpoint_insert = cpu_breakpoint_insert,
	.breakpoint_remove = cpu_breakpoint_remove,
	.breakpoint = cpu_breakpoint,
	.singlestep = 0,
	.env = &__arm32_env,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	return &__arm32_gdb_cpu;
}
