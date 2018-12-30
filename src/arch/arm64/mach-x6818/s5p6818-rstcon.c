/*
 * s5p6818-rstcon.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <s5p6818/reg-rstcon.h>
#include <s5p6818-rstcon.h>

static void s5p6818_ip_setrst(int id, int reset)
{
	virtual_addr_t virt = phys_to_virt(S5P6818_IP_RSTCON_BASE);
	u32_t val;

	if(id < 32)
		virt += IP_RSTCON0;
	else if(id < 64)
		virt += IP_RSTCON1;
	else if(id < 96)
		virt += IP_RSTCON2;
	else
		return;

	val = read32(virt);
	val &= ~(0x1 << (id & 0x1f));
	val |= (reset ? 1 : 0) << (id & 0x1f);
	write32(virt, val);
}

static int s5p6818_ip_getrst(int id)
{
	virtual_addr_t virt = phys_to_virt(S5P6818_IP_RSTCON_BASE);
	u32_t val;

	if(id < 32)
		virt += IP_RSTCON0;
	else if(id < 64)
		virt += IP_RSTCON1;
	else if(id < 96)
		virt += IP_RSTCON2;
	else
		return 1;

	val = read32(virt);
	return (val >> (id & 0x1f)) & 0x1;
}

void s5p6818_ip_reset(int id, int force)
{
	if(force || !s5p6818_ip_getrst(id))
	{
		s5p6818_ip_setrst(id, 0);
		udelay(10);
		s5p6818_ip_setrst(id, 1);
	}
}
