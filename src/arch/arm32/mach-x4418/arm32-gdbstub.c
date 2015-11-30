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

struct arm32_state_t {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
};

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

static inline int cpu_get_reg32(char * buf , uint32_t reg)
{
	memcpy(buf, &reg, 4);
	return 4;
}

static inline int cpu_set_reg32(uint32_t * reg, char * buf)
{
	memcpy(reg, buf, 4);
	return 4;
}

static void cpu_save_register(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_state_t * env = (struct arm32_state_t *)cpu->env;
	struct pt_regs_t * r = (struct pt_regs_t *)regs;
	env->r[0] = r->r0;
	env->r[1] = r->r1;
	env->r[2] = r->r2;
	env->r[3] = r->r3;
	env->r[4] = r->r4;
	env->r[5] = r->r5;
	env->r[6] = r->r6;
	env->r[7] = r->r7;
	env->r[8] = r->r8;
	env->r[9] = r->r9;
	env->r[10] = r->r10;
	env->r[11] = r->fp;
	env->r[12] = r->ip;
	env->sp = r->sp;
	env->lr = r->lr;
	env->pc = r->pc;
	env->cpsr = r->cpsr;
}

static void cpu_restore_register(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_state_t * env = (struct arm32_state_t *)cpu->env;
	struct pt_regs_t * r = (struct pt_regs_t *)regs;

	r->r0 = env->r[0];
	r->r1 = env->r[1];
	r->r2 = env->r[2];
	r->r3 = env->r[3];
	r->r4 = env->r[4];
	r->r5 = env->r[5];
	r->r6 = env->r[6];
	r->r7 = env->r[7];
	r->r8 = env->r[8];
	r->r9 = env->r[9];
	r->r10 = env->r[10];
	r->fp = env->r[11];
	r->ip = env->r[12];
	r->sp = env->sp;
	r->lr = env->lr;
	r->pc = env->pc;
	r->pc += 4;
}

static int cpu_read_register(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_state_t * env = (struct arm32_state_t *)cpu->env;

	if(n < 13)
	{
		return cpu_get_reg32(buf, env->r[n]);
	}
	else if(n == 13)
	{
		return cpu_get_reg32(buf, env->sp);
	}
	else if(n == 14)
	{
		return cpu_get_reg32(buf, env->lr);
	}
	else if(n == 15)
	{
		return cpu_get_reg32(buf, env->pc);
	}
	else if(n < 24)		/* f0 - f7 */
	{
        memset(buf, 0, 12);
        return 12;
	}
	else if(n == 24)	/* fps */
	{
		return cpu_get_reg32(buf, 0);
	}
	else if(n == 25)
	{
		return cpu_get_reg32(buf, env->cpsr);
	}
	return 0;
}

static int cpu_write_register(struct gdb_cpu_t * cpu, char * buf, int n)
{
	struct arm32_state_t * env = (struct arm32_state_t *)cpu->env;

	if(n < 13)
	{
		return cpu_set_reg32(&env->r[n], buf);
	}
	else if(n == 13)
	{
		return cpu_set_reg32(&env->sp, buf);
	}
	else if(n == 14)
	{
		return cpu_set_reg32(&env->lr, buf);
	}
	else if(n == 15)
	{
		return cpu_set_reg32(&env->pc, buf);
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
		return cpu_set_reg32(&env->cpsr, buf);
	}
	return 0;
}

static int cpu_set_pc(struct gdb_cpu_t * cpu, virtual_addr_t addr)
{
	struct arm32_state_t * env = (struct arm32_state_t *)cpu->env;
	env->pc = (uint32_t)addr;
	return 0;
}

static int cpu_mem_access(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return 0;
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ (".word 0xe7ffdeff");
}

static struct arm32_state_t __arm32_env;

static struct gdb_cpu_t __arm32_gdb_cpu = {
	.nregs = 26,
	.save_register = cpu_save_register,
	.restore_register = cpu_restore_register,
	.read_register = cpu_read_register,
	.write_register = cpu_write_register,
	.set_pc = cpu_set_pc,
	.mem_access = cpu_mem_access,
	.breakpoint = cpu_breakpoint,
	.env = &__arm32_env,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	return &__arm32_gdb_cpu;
}

void do_undefined_instruction(struct pt_regs_t * regs)
{
	gdbserver_handle_exception(regs);
}
