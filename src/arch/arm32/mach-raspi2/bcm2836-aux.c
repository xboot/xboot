/*
 * bcm2836-aux.c
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
#include <bcm2836-aux.h>
#include <bcm2836/reg-aux.h>

void bcm2836_aux_enable(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	u32_t val;

	val = read32(virt + AUX_ENB);
	val &= ~(0x1 << id);
	val |= 1 << id;
	write32(virt + AUX_ENB, val);
}

void bcm2836_aux_disable(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	u32_t val;

	val = read32(virt + AUX_ENB);
	val &= ~(0x1 << id);
	val |= 0 << id;
	write32(virt + AUX_ENB, val);
}

int bcm2836_aux_irq_status(int id)
{
	virtual_addr_t virt = phys_to_virt(BCM2836_AUX_BASE);
	return (read32(virt + AUX_IRQ) & (0x1 << id)) ? 1 : 0;
}
