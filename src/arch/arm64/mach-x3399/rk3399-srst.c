/*
 * rk3399-rstcon.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <rk3399/reg-cru.h>
#include <rk3399/reg-pmu-cru.h>
#include <rk3399-srst.h>

void rk3399_soft_reset(int id)
{
	virtual_addr_t virt;
	int bank = id / 16;
	int offset = id % 16;

	if(bank < 21)
		virt = RK3399_CRU_BASE + CRU_SOFTRSTS_CON(bank);
	else if(bank < 23)
		virt = RK3399_PMU_CRU_BASE + PMU_CRU_SOFTRSTS_CON(bank - 21);
	else
		return;

	write32(virt, ((1 << offset) << 16) | (1 << offset));
	udelay(10);
	write32(virt, ((1 << offset) << 16) | (0 << offset));
}
