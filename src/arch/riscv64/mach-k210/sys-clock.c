/*
 * sys-clock.c
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
#include <k210/reg-sysctl.h>

static void sysctl_pll_set_rate(virtual_addr_t base, int channel, u64_t prate, u64_t rate)
{
	virtual_addr_t addr = base + SYSCTL_PLL0 + (channel << 2);
	u32_t r;

	double vco_min = 3.5e+08;
	double vco_max = 1.75e+09;
	double ref_min = 1.36719e+07;
	double ref_max = 1.75e+09;
	int nr_min = 1;
	int nr_max = 16;
	int nf_min = 1;
	int nf_max = 64;
	int no_min = 1;
	int no_max = 16;
	int nb_min = 1;
	int nb_max = 64;
	int max_vco = 1;
	int ref_rng = 1;

	int nr = 0;
	int nrx = 0;
	int nf = 0;
	int nfi = 0;
	int no = 0;
	int noe = 0;
	int not = 0;
	int nor = 0;
	int nore = 0;
	int nb = 0;
	int first = 0;
	int firstx = 0;
	int found = 0;

	long long nfx = 0;
	double fin = 0, fout = 0, fvco = 0;
	double val = 0, nval = 0, err = 0, merr = 0, terr = 0;
	int x_nrx = 0, x_no = 0, x_nb = 0;
	long long x_nfx = 0;
	double x_fvco = 0, x_err = 0;

	fin = prate;
	fout = rate;
	val = fout / fin;
	terr = 0.5 / ((double)(nf_max / 2));
	first = firstx = 1;
	if(terr != -2)
	{
		first = 0;
		if(terr == 0)
			terr = 1e-16;
		merr = fabs(terr);
	}
    found = 0;
	for(nfi = val; nfi < nf_max; ++nfi)
	{
		nr = rint(((double)nfi) / val);
		if(nr == 0)
			continue;
		if((ref_rng) && (nr < nr_min))
			continue;
		if(fin / ((double)nr) > ref_max)
			continue;
		nrx = nr;
		nf = nfx = nfi;
		nval = ((double)nfx) / ((double)nr);
		if(nf == 0)
			nf = 1;
		err = 1 - nval / val;

		if((first) || (fabs(err) < merr * (1 + 1e-6)) || (fabs(err) < 1e-16))
		{
			not = floor(vco_max / fout);
			for(no = (not > no_max) ? no_max : not; no > no_min; --no)
			{
				if((ref_rng) && ((nr / no) < nr_min))
					continue;
				if((nr % no) == 0)
					break;
			}
			if((nr % no) != 0)
				continue;
			nor = ((not > no_max) ? no_max : not) / no;
			nore = nf_max / nf;
			if(nor > nore)
				nor = nore;
			noe = ceil(vco_min / fout);
			if(!max_vco)
			{
				nore = (noe - 1) / no + 1;
				nor = nore;
				not = 0;
			}
			if((((no * nor) < (not >> 1)) || ((no * nor) < noe)) && ((no * nor) < (nf_max / nf)))
			{
				no = nf_max / nf;
				if(no > no_max)
					no = no_max;
				if(no > not)
					no = not;
				nfx *= no;
				nf *= no;
				if((no > 1) && (!firstx))
					continue;
			}
			else
			{
				nrx /= no;
				nfx *= nor;
				nf *= nor;
				no *= nor;
				if(no > no_max)
					continue;
				if((nor > 1) && (!firstx))
					continue;
			}

			nb = nfx;
			if(nb < nb_min)
				nb = nb_min;
			if(nb > nb_max)
				continue;

			fvco = fin / ((double)nrx) * ((double)nfx);
			if(fvco < vco_min)
				continue;
			if(fvco > vco_max)
				continue;
			if(nf < nf_min)
				continue;
			if((ref_rng) && (fin / ((double)nrx) < ref_min))
				continue;
			if((ref_rng) && (nrx > nr_max))
				continue;
			if(!(((firstx) && (terr < 0)) || (fabs(err) < merr * (1 - 1e-6)) || ((max_vco) && (no > x_no))))
				continue;
			if((!firstx) && (terr >= 0) && (nrx > x_nrx))
				continue;

			found = 1;
			x_no = no;
			x_nrx = nrx;
			x_nfx = nfx;
			x_nb = nb;
			x_fvco = fvco;
			x_err = err;
			first = firstx = 0;
			merr = fabs(err);
			if(terr != -1)
				continue;
		}
	}
	if(!found)
		return;

	nrx = x_nrx;
	nfx = x_nfx;
	no = x_no;
	nb = x_nb;
	fvco = x_fvco;
	err = x_err;
	if((terr != -2) && (fabs(err) >= terr * (1 - 1e-6)))
		return;

	r = read32(addr);
	r &= ~(0xfffff << 0);
	r |= (nrx - 1) << 0;
	r |= (nfx - 1) << 4;
	r |= (no - 1) << 10;
	r |= (nb - 1) << 14;
	write32(addr, r);
}

static void sysctl_pll_wait_lock(virtual_addr_t base, int channel)
{
	virtual_addr_t addr = base + SYSCTL_PLL_LOCK;
	u32_t val;

	while(1)
	{
		val = read32(addr);
		if(((val >> (channel << 3)) & 0x3) == 0x3)
			return;
		val |= 0x1 << ((channel << 3) + 2);
		write32(addr, val);
	}
}

void sys_clock_pll_set_rate(virtual_addr_t base, int channel, u64_t prate, u64_t rate)
{
	u32_t val;

	if(channel == 0)
	{
		/* Change aclk to xtal */
		val = read32(base + SYSCTL_CLK_SEL0);
		val &= ~(1 << 0);
		write32(base + SYSCTL_CLK_SEL0, val);

		/* Do not bypass pll */
		val = read32(base + SYSCTL_PLL0);
		val &= ~(1 << 23);
		write32(base + SYSCTL_PLL0, val);

		/* Disable pll output */
		val = read32(base + SYSCTL_PLL0);
		val &= ~(1 << 25);
		write32(base + SYSCTL_PLL0, val);

		/* Power off pll */
		val = read32(base + SYSCTL_PLL0);
		val &= ~(1 << 21);
		write32(base + SYSCTL_PLL0, val);

		/* Set pll new value */
		sysctl_pll_set_rate(base, channel, prate, rate);

		/* Power on pll */
		val = read32(base + SYSCTL_PLL0);
		val |= (1 << 21);
		write32(base + SYSCTL_PLL0, val);

		/* Reset pll */
		val = read32(base + SYSCTL_PLL0);
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL0, val);
		val |= (1 << 20);
		write32(base + SYSCTL_PLL0, val);
		__asm__ __volatile__("nop");
		__asm__ __volatile__("nop");
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL0, val);

		/* Wait pll stable */
		sysctl_pll_wait_lock(base, channel);

		/* Enable pll output */
		val = read32(base + SYSCTL_PLL0);
		val |= (1 << 25);
		write32(base + SYSCTL_PLL0, val);

		/* Change aclk to pll */
		val = read32(base + SYSCTL_CLK_SEL0);
		val |= (1 << 0);
		write32(base + SYSCTL_CLK_SEL0, val);
	}
	else if(channel == 1)
	{
		/* Do not bypass pll */
		val = read32(base + SYSCTL_PLL1);
		val &= ~(1 << 23);
		write32(base + SYSCTL_PLL1, val);

		/* Disable pll output */
		val = read32(base + SYSCTL_PLL1);
		val &= ~(1 << 25);
		write32(base + SYSCTL_PLL1, val);

		/* Power off pll */
		val = read32(base + SYSCTL_PLL1);
		val &= ~(1 << 21);
		write32(base + SYSCTL_PLL1, val);

		/* Set pll new value */
		sysctl_pll_set_rate(base, channel, prate, rate);

		/* Power on pll */
		val = read32(base + SYSCTL_PLL1);
		val |= (1 << 21);
		write32(base + SYSCTL_PLL1, val);

		/* Reset pll */
		val = read32(base + SYSCTL_PLL1);
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL1, val);
		val |= (1 << 20);
		write32(base + SYSCTL_PLL1, val);
		__asm__ __volatile__("nop");
		__asm__ __volatile__("nop");
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL1, val);

		/* Wait pll stable */
		sysctl_pll_wait_lock(base, channel);

		/* Enable pll output */
		val = read32(base + SYSCTL_PLL1);
		val |= (1 << 25);
		write32(base + SYSCTL_PLL1, val);
	}
	else if(channel == 2)
	{
		/* Do not bypass pll */
		val = read32(base + SYSCTL_PLL2);
		val &= ~(1 << 23);
		write32(base + SYSCTL_PLL2, val);

		/* Disable pll output */
		val = read32(base + SYSCTL_PLL2);
		val &= ~(1 << 25);
		write32(base + SYSCTL_PLL2, val);

		/* Power off pll */
		val = read32(base + SYSCTL_PLL2);
		val &= ~(1 << 21);
		write32(base + SYSCTL_PLL2, val);

		/* Set pll new value */
		sysctl_pll_set_rate(base, channel, prate, rate);

		/* Power on pll */
		val = read32(base + SYSCTL_PLL2);
		val |= (1 << 21);
		write32(base + SYSCTL_PLL2, val);

		/* Reset pll */
		val = read32(base + SYSCTL_PLL2);
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL2, val);
		val |= (1 << 20);
		write32(base + SYSCTL_PLL2, val);
		__asm__ __volatile__("nop");
		__asm__ __volatile__("nop");
		val &= ~(1 << 20);
		write32(base + SYSCTL_PLL2, val);

		/* Wait pll stable */
		sysctl_pll_wait_lock(base, channel);

		/* Enable pll output */
		val = read32(base + SYSCTL_PLL2);
		val |= (1 << 25);
		write32(base + SYSCTL_PLL2, val);
	}
}

void sys_clock_init(void)
{
	virtual_addr_t base = K210_SYSCTL_BASE;
	u64_t xin = 26 * 1000 * 1000;

	sys_clock_pll_set_rate(base, 0, xin, 806 * 1000 * 1000);
	sys_clock_pll_set_rate(base, 1, xin, 200 * 1000 * 1000);
	sys_clock_pll_set_rate(base, 2, xin, 45158400);
}
