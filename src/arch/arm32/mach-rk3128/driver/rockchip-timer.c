/*
 * driver/rockchip-timer.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <rockchip-timer.h>

enum {
	TIMER_LOAD_COUNT0		= 0x00,
	TIMER_LOAD_COUNT1		= 0x04,
	TIMER_CURRENT_VALUE0	= 0x08,
	TIMER_CURRENT_VALUE1	= 0x0c,
	TIMER_CONTROL_REG		= 0x10,
	TIMER_INT_STATUS		= 0x18,
};

void rockchip_timer_start(virtual_addr_t virt, int irqon, int oneshot)
{
	u32_t val = ((irqon ? 1 : 0) << 2) | ((oneshot ? 1 : 0) << 1) | (1 << 0);
	write32(virt + TIMER_CONTROL_REG, val);
}

void rockchip_timer_stop(virtual_addr_t virt)
{
	write32(virt + TIMER_CONTROL_REG, 0);
}

void rockchip_timer_count(virtual_addr_t virt, u64_t cnt)
{
	u32_t upper = (u32_t)((cnt >> 32) & 0xffffffff);
	u32_t lower = (u32_t)((cnt >> 0) & 0xffffffff);

	write32(virt + TIMER_LOAD_COUNT1, upper);
	write32(virt + TIMER_LOAD_COUNT0, lower);
}

u32_t rockchip_timer_read32(virtual_addr_t virt)
{
	return read32(virt + TIMER_CURRENT_VALUE0);
}

u64_t rockchip_timer_read64(virtual_addr_t virt)
{
	u32_t upper, lower;

	do {
		upper = read32(virt + TIMER_CURRENT_VALUE1);
		lower = read32(virt + TIMER_CURRENT_VALUE0);
	} while (upper != read32(virt + TIMER_CURRENT_VALUE1));

	return ((u64_t)upper << 32) | lower;
}

void rockchip_timer_irq_clear(virtual_addr_t virt)
{
	write32(virt + TIMER_INT_STATUS, 0x1);
}
