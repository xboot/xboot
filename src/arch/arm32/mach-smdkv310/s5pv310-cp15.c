/*
 * s5pv310-cp15.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <types.h>
#include <s5pv310-cp15.h>

/*
 * read co-processor 15, register #1 (control register)
 */
static u32_t read_p15_c1(void)
{
	u32_t value;

	__asm__ __volatile__(
		"mrc p15, 0, %0, c1, c0, 0"
		: "=r" (value)
		:
		: "memory");

	return value;
}

/*
 * write to co-processor 15, register #1 (control register)
 */
static void write_p15_c1(u32_t value)
{
	__asm__ __volatile__(
		"mcr p15, 0, %0, c1, c0, 0"
		:
		: "r" (value)
		: "memory");

	read_p15_c1();
}

void irq_enable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"bic %0, %0, #(1<<7)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void irq_disable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"orr %0, %0, #(1<<7)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void fiq_enable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"bic %0, %0, #(1<<6)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void fiq_disable(void)
{
	u32_t tmp;

	__asm__ __volatile__(
		"mrs %0, cpsr\n"
		"orr %0, %0, #(1<<6)\n"
		"msr cpsr_cxsf, %0"
		: "=r" (tmp)
		:
		: "memory");
}

void icache_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | (1<<12));
}

void icache_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~(1<<12));
}

void dcache_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | (1<<2));
}

void dcache_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~(1<<2));
}

void mmu_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | (1<<0));
}

void mmu_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~(1<<0));
}

void vic_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | (1<<24));
}

void vic_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~(1<<24));
}

void branch_enable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg | (1<<11));
}

void branch_disable(void)
{
	u32_t reg;

	reg = read_p15_c1();
	write_p15_c1(reg & ~(1<<11));
}
