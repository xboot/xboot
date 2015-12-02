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

struct arm32_breakpoint_t {
	virtual_addr_t addr;
	virtual_size_t size;
	int type;
	uint8_t instruction[4];
};
struct arm32_breakpoint_list_t
{
	struct arm32_breakpoint_t * bp;
	struct list_head entry;
};
static struct arm32_breakpoint_list_t __arm32_breakpoint_list = {
	.entry = {
		.next	= &(__arm32_breakpoint_list.entry),
		.prev	= &(__arm32_breakpoint_list.entry),
	},
};

static struct arm32_breakpoint_t * cpu_breakpoint_search(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int type)
{
	struct arm32_breakpoint_list_t * l = (struct arm32_breakpoint_list_t *)cpu->bplist;
	struct arm32_breakpoint_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		if((pos->bp->addr == addr) && (pos->bp->size == size) && (pos->bp->type == type))
			return pos->bp;
	}
	return NULL;
}

static int cpu_breakpoint_insert(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int type)
{
	struct arm32_breakpoint_list_t * l = (struct arm32_breakpoint_list_t *)cpu->bplist;
	struct arm32_breakpoint_list_t * bl;
	struct arm32_breakpoint_t * bp;
	const uint8_t bpinstr[4] = {0xfe, 0xde, 0xff, 0xe7};	/* 0xe7ffdefe */

	switch(type)
	{
	case 0:	/* breakpoint software */
		break;
	case 1:	/* breakpoint hardware */
	case 2:	/* watchpoint write */
	case 3:	/* watchpoint read */
	case 4:	/* watchpoint access */
	default:
		return -1;
	}

	if(cpu_breakpoint_search(cpu, addr, size, type))
		return -1;

	bl = malloc(sizeof(struct arm32_breakpoint_list_t));
	if(!bl)
		return -1;

	bp = malloc(sizeof(struct arm32_breakpoint_t));
	if(!bp)
	{
		free(bl);
		return -1;
	}

	bp->addr = addr;
	bp->size = size;
	bp->type = type;
	switch(type)
	{
	case 0:	/* breakpoint software */
		memcpy(bp->instruction, (void *)(bp->addr), 4);
		memcpy((void *)(bp->addr), bpinstr, 4);
		break;
	case 1:	/* breakpoint hardware */
	case 2:	/* watchpoint write */
	case 3:	/* watchpoint read */
	case 4:	/* watchpoint access */
	default:
		break;
	}

	bl->bp = bp;
	list_add_tail(&bl->entry, &(l->entry));
	return 0;
}

static int cpu_breakpoint_remove(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int type)
{
	struct arm32_breakpoint_list_t * l = (struct arm32_breakpoint_list_t *)cpu->bplist;
	struct arm32_breakpoint_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		if((pos->bp->addr == addr) && (pos->bp->size == size) && (pos->bp->type == type))
		{
			switch(type)
			{
			case 0:	/* breakpoint software */
				memcpy((void *)(pos->bp->addr), pos->bp->instruction, 4);
				break;
			case 1:	/* breakpoint hardware */
			case 2:	/* watchpoint write */
			case 3:	/* watchpoint read */
			case 4:	/* watchpoint access */
			default:
				break;
			}
			list_del(&(pos->entry));
			free(pos->bp);
			free(pos);
		}
	}
	return 0;
}

static int cpu_breakpoint_remove_all(struct gdb_cpu_t * cpu)
{
	struct arm32_breakpoint_list_t * l = (struct arm32_breakpoint_list_t *)cpu->bplist;
	struct arm32_breakpoint_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		switch(pos->bp->type)
		{
		case 0:	/* breakpoint software */
			memcpy((void *)(pos->bp->addr), pos->bp->instruction, 4);
			break;
		case 1:	/* breakpoint hardware */
		case 2:	/* watchpoint write */
		case 3:	/* watchpoint read */
		case 4:	/* watchpoint access */
		default:
			break;
		}
		list_del(&(pos->entry));
		free(pos->bp);
		free(pos);
	}
	return 0;
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ __volatile__(".word 0xe7ffdeff");
}

static void cpu_save_register(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	memcpy(env, regs, sizeof(struct arm32_env_t));
}

static void cpu_restore_register(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	if(*(uint32_t *)(env->pc) == 0xe7ffdeff)
		env->pc += 4;
	memcpy(regs, env, sizeof(struct arm32_env_t));
}

static int cpu_read_register(struct gdb_cpu_t * cpu, char * buf, int n)
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

static int cpu_write_register(struct gdb_cpu_t * cpu, char * buf, int n)
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
	.env = &__arm32_env,
	.bplist = &__arm32_breakpoint_list,
	.save_register = cpu_save_register,
	.restore_register = cpu_restore_register,
	.read_register = cpu_read_register,
	.write_register = cpu_write_register,
	.set_program_counter = cpu_set_program_counter,
	.acess_memory = cpu_acess_memory,
	.breakpoint_insert = cpu_breakpoint_insert,
	.breakpoint_remove = cpu_breakpoint_remove,
	.breakpoint_remove_all = cpu_breakpoint_remove_all,
	.breakpoint = cpu_breakpoint,
	.singlestep = 0,
	.priv = 0,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	cpu_breakpoint_remove_all(&__arm32_gdb_cpu);
	return &__arm32_gdb_cpu;
}
