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
		virtual_addr_t addr;
		uint8_t instruction[4];
	} step;
};
static struct arm32_env_t __arm32_env;

static void cpu_register_save(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	memcpy(&env->regs, regs, sizeof(env->regs));
}

static void cpu_register_restore(struct gdb_cpu_t * cpu, void * regs)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	if(*(uint32_t *)(env->regs.pc) == 0xe7ffdeff)
		env->regs.pc += 4;
	memcpy(regs, &env->regs, sizeof(env->regs));
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

static int cpu_singlestep_active(struct gdb_cpu_t * cpu)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	return 0;
#if 0
	const uint8_t bpinstr[4] = {0xfe, 0xde, 0xff, 0xe7};	/* 0xe7ffdefe */

	if(env->step.enable != 0)
	{
		 unsigned long curins;
        curins = *(unsigned long*)(env->regs.pc);
        if (ins_will_execute(curins))
            //Decode instruction to decide what the next pc will be
            step_addr = target_ins((unsigned long *)env->regs.pc, curins);
        else
            step_addr = env->regs.pc + 4;

		memcpy(env->step.instruction, (void *)(step_addr), 4);
		memcpy((void *)(step_addr), bpinstr, 4);
	}
#endif
}

static int cpu_singlestep_finish(struct gdb_cpu_t * cpu)
{
	struct arm32_env_t * env = (struct arm32_env_t *)cpu->env;
	if(env->step.addr != 0)
		memcpy((void *)(env->step.addr), env->step.instruction, 4);
	env->step.addr = 0;
	return 0;
}

static int cpu_memory_acess(struct gdb_cpu_t * cpu, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return 0;
}

static int cpu_processor(struct gdb_cpu_t * cpu)
{
	return 0;
}

static void cpu_breakpoint(struct gdb_cpu_t * cpu)
{
	__asm__ __volatile__(".word 0xe7ffdeff");
}

static struct gdb_cpu_t __arm32_gdb_cpu = {
	.nregs = 26,
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
	.env = &__arm32_env,
};

struct gdb_cpu_t * arch_gdb_cpu(void)
{
	return &__arm32_gdb_cpu;
}

#if 0
#define CPSR_IRQ_DISABLE	0x80	// IRQ disabled when =1
#define CPSR_FIQ_DISABLE	0x40	// FIQ disabled when =1
#define CPSR_THUMB_ENABLE	0x20	// Thumb mode when =1
#define CPSR_USER_MODE		0x10
#define CPSR_FIQ_MODE		0x11
#define CPSR_IRQ_MODE		0x12
#define CPSR_SUPERVISOR_MODE	0x13
#define CPSR_UNDEF_MODE		0x1B

#define HAL_BREAKINST_ARM          0xE7FFDEFE
#define HAL_BREAKINST_ARM_SIZE     4
#define HAL_BREAKINST_THUMB        0xbebe  // illegal instruction currently
#define HAL_BREAKINST_THUMB_SIZE   2

#define PS_N 0x80000000
#define PS_Z 0x40000000
#define PS_C 0x20000000
#define PS_V 0x10000000

#define PS_I CPSR_IRQ_DISABLE
#define PS_F CPSR_FIQ_DISABLE

#define IS_THUMB_ADDR(addr)     ((addr) & 1)
#define MAKE_THUMB_ADDR(addr) ((addr) | 1)
#define UNMAKE_THUMB_ADDR(addr) ((addr) & ~1)

enum {
    R0, R1, R2, R3, R4, R5, R6, R7,
    R8, R9, R10, FP, IP, SP, LR, PC,
    F0, F1, F2, F3, F4, F5, F6, F7,
    FPS, PS
};

uint32_t get_register(int reg)
{
	struct arm32_env_t * env = &__arm32_env;

	switch(reg)
	{
	case R0:
		return env->regs.r[0];
	case R1:
		return env->regs.r[1];
	case R2:
		return env->regs.r[2];
	case R3:
		return env->regs.r[3];
	case R4:
		return env->regs.r[4];
	case R5:
		return env->regs.r[5];
	case R6:
		return env->regs.r[6];
	case R7:
		return env->regs.r[7];
	case R8:
		return env->regs.r[8];
	case R9:
		return env->regs.r[9];
	case R10:
		return env->regs.r[10];
	case FP:
		return env->regs.r[11];
	case IP:
		return env->regs.r[12];
	case SP:
		return env->regs.sp;
	case LR:
		return env->regs.lr;
	case PC:
		return env->regs.pc;
	case PS:
		return env->regs.cpsr;
	default:
		break;
	}
	return -1;
}
static int ins_will_execute(unsigned long ins)
{
    unsigned long psr = get_register(PS);  // condition codes
    int res = 0;
    switch ((ins & 0xF0000000) >> 28) {
        case 0x0: // EQ
            res = (psr & PS_Z) != 0;
            break;
        case 0x1: // NE
            res = (psr & PS_Z) == 0;
            break;
        case 0x2: // CS
            res = (psr & PS_C) != 0;
            break;
        case 0x3: // CC
            res = (psr & PS_C) == 0;
            break;
        case 0x4: // MI
            res = (psr & PS_N) != 0;
            break;
        case 0x5: // PL
            res = (psr & PS_N) == 0;
            break;
        case 0x6: // VS
            res = (psr & PS_V) != 0;
            break;
        case 0x7: // VC
            res = (psr & PS_V) == 0;
            break;
        case 0x8: // HI
            res = ((psr & PS_C) != 0) && ((psr & PS_Z) == 0);
            break;
        case 0x9: // LS
            res = ((psr & PS_C) == 0) || ((psr & PS_Z) != 0);
            break;
        case 0xA: // GE
            res = ((psr & (PS_N|PS_V)) == (PS_N|PS_V)) ||
                ((psr & (PS_N|PS_V)) == 0);
            break;
        case 0xB: // LT
            res = ((psr & (PS_N|PS_V)) == PS_N) ||
                ((psr & (PS_N|PS_V)) == PS_V);
            break;
        case 0xC: // GT
            res = ((psr & (PS_N|PS_V)) == (PS_N|PS_V)) ||
                ((psr & (PS_N|PS_V)) == 0);
            res = ((psr & PS_Z) == 0) && res;
            break;
        case 0xD: // LE
            res = ((psr & (PS_N|PS_V)) == PS_N) ||
                ((psr & (PS_N|PS_V)) == PS_V);
            res = ((psr & PS_Z) == PS_Z) || res;
            break;
        case 0xE: // AL
            res = 1;
            break;
        case 0xF: // NV
            if (((ins & 0x0E000000) >> 24) == 0xA)
                res = 1;
            else
                res = 0;
            break;
    }
    return res;
}

static unsigned long RmShifted(int shift)
{
    unsigned long Rm = get_register(shift & 0x00F);
    int shift_count;
    if ((shift & 0x010) == 0) {
        shift_count = (shift & 0xF80) >> 7;
    } else {
        shift_count = get_register((shift & 0xF00) >> 8);
    }
    switch ((shift & 0x060) >> 5) {
        case 0x0: // Logical left
            Rm <<= shift_count;
            break;
        case 0x1: // Logical right
            Rm >>= shift_count;
            break;
        case 0x2: // Arithmetic right
            Rm = (unsigned long)((long)Rm >> shift_count);
            break;
        case 0x3: // Rotate right
            if (shift_count == 0) {
                // Special case, RORx
                Rm >>= 1;
                if (get_register(PS) & PS_C) Rm |= 0x80000000;
            } else {
                Rm = (Rm >> shift_count) | (Rm << (32-shift_count));
            }
            break;
    }
    return Rm;
}

// Decide the next instruction to be executed for a given instruction
static unsigned long target_ins(unsigned long *pc, unsigned long ins)
{
    unsigned long new_pc, offset, op2;
    unsigned long Rn;
    int i, reg_count, c;

    switch ((ins & 0x0C000000) >> 26) {
        case 0x0:
            // BX or BLX
            if ((ins & 0x0FFFFFD0) == 0x012FFF10) {
                new_pc = (unsigned long)get_register(ins & 0x0000000F);
                return new_pc;
            }
            // Data processing
            new_pc = (unsigned long)(pc+1);
            if ((ins & 0x0000F000) == 0x0000F000) {
                // Destination register is PC
                if ((ins & 0x0FBF0000) != 0x010F0000) {
                    Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                    if ((ins & 0x000F0000) == 0x000F0000) Rn += 8;  // PC prefetch!
                    if ((ins & 0x02000000) == 0) {
                        op2 = RmShifted(ins & 0x00000FFF);
                    } else {
                        op2 = ins & 0x000000FF;
                        i = (ins & 0x00000F00) >> 8;  // Rotate count
                        op2 = (op2 >> (i*2)) | (op2 << (32-(i*2)));
                    }
                    switch ((ins & 0x01E00000) >> 21) {
                        case 0x0: // AND
                            new_pc = Rn & op2;
                            break;
                        case 0x1: // EOR
                           new_pc = Rn ^ op2;
                            break;
                        case 0x2: // SUB
                            new_pc = Rn - op2;
                            break;
                        case 0x3: // RSB
                            new_pc = op2 - Rn;
                            break;
                        case 0x4: // ADD
                            new_pc = Rn + op2;
                            break;
                        case 0x5: // ADC
                            c = (get_register(PS) & PS_C) != 0;
                            new_pc = Rn + op2 + c;
                            break;
                        case 0x6: // SBC
                            c = (get_register(PS) & PS_C) != 0;
                            new_pc = Rn - op2 + c - 1;
                            break;
                        case 0x7: // RSC
                            c = (get_register(PS) & PS_C) != 0;
                            new_pc = op2 - Rn +c - 1;
                            break;
                        case 0x8: // TST
                        case 0x9: // TEQ
                        case 0xA: // CMP
                        case 0xB: // CMN
                            break; // PC doesn't change
                        case 0xC: // ORR
                            new_pc = Rn | op2;
                            break;
                        case 0xD: // MOV
                            new_pc = op2;
                            break;
                        case 0xE: // BIC
                            new_pc = Rn & ~op2;
                            break;
                        case 0xF: // MVN
                            new_pc = ~op2;
                            break;
                    }
                }
            }
            return new_pc;
        case 0x1:
            if ((ins & 0x02000010) == 0x02000010) {
                // Undefined!
                return (unsigned long)(pc+1);
            } else {
                if ((ins & 0x00100000) == 0) {
                    // STR
                    return (unsigned long)(pc+1);
                } else {
                    // LDR
                    if ((ins & 0x0000F000) != 0x0000F000) {
                        // Rd not PC
                        return (unsigned long)(pc+1);
                    } else {
                        Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                        if ((ins & 0x000F0000) == 0x000F0000) Rn += 8;  // PC prefetch!
                        if (ins & 0x01000000) {
                            // Add/subtract offset before
                            if ((ins & 0x02000000) == 0) {
                                // Immediate offset
                                if (ins & 0x00800000) {
                                    // Add offset
                                    Rn += (ins & 0x00000FFF);
                                } else {
                                    // Subtract offset
                                    Rn -= (ins & 0x00000FFF);
                                }
                            } else {
                                // Offset is in a register
                                if (ins & 0x00800000) {
                                    // Add offset
                                    Rn += RmShifted(ins & 0x00000FFF);
                                } else {
                                    // Subtract offset
                                    Rn -= RmShifted(ins & 0x00000FFF);
                                }
                            }
                        }
                        return *(unsigned long *)Rn;
                    }
                }
            }
            return (unsigned long)(pc+1);
        case 0x2:  // Branch, LDM/STM
            if ((ins & 0x02000000) == 0) {
                // LDM/STM
                if ((ins & 0x00100000) == 0) {
                    // STM
                    return (unsigned long)(pc+1);
                } else {
                    // LDM
                    if ((ins & 0x00008000) == 0) {
                        // PC not in list
                        return (unsigned long)(pc+1);
                    } else {
                        Rn = (unsigned long)get_register((ins & 0x000F0000) >> 16);
                        if ((ins & 0x000F0000) == 0x000F0000) Rn += 8;  // PC prefetch!
                        offset = ins & 0x0000FFFF;
                        reg_count = 0;
                        for (i = 0;  i < 15;  i++) {
                            if (offset & (1<<i)) reg_count++;
                        }
                        if (ins & 0x00800000) {
                            // Add offset
                            Rn += reg_count*4;
                        } else {
                            // Subtract offset
                            Rn -= 4;
                        }
                        return *(unsigned long *)Rn;
                    }
                }
            } else {
                // Branch
                if (ins_will_execute(ins)) {
                    offset = (ins & 0x00FFFFFF) << 2;
                    if (ins & 0x00800000)
                        offset |= 0xFC000000;  // sign extend
                    new_pc = (unsigned long)(pc+2) + offset;
                    // If its BLX, make new_pc a thumb address.
                    if ((ins & 0xFE000000) == 0xFA000000) {
                        if ((ins & 0x01000000) == 0x01000000)
                            new_pc |= 2;
                        new_pc = MAKE_THUMB_ADDR(new_pc);
                    }

                    return new_pc;
                } else {
                    // Falls through
                    return (unsigned long)(pc+1);
                }
            }
        case 0x3:  // Coprocessor & SWI
            if (((ins & 0x03000000) == 0x03000000) && ins_will_execute(ins)) {
                // SWI
                // TODO(wzyy2) some problems.
               // extern unsigned long vector_swi;
                //return vector_swi;
            } else {
                return (unsigned long)(pc+1);
            }
        default:
            // Never reached - but fixes compiler warning.
            return 0;
    }
}
#endif
