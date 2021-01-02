/*
 * sys-copyself.c
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

extern unsigned char __image_start[];
extern unsigned char __image_end[];

#define irom_sdmmc_to_mem(sector, count, mem) \
	(((u32_t(*)(u32_t, u32_t, u32_t *))(*((u32_t *)(0x02020030))))(sector, count, mem))

void sys_copyself(void)
{
	virtual_addr_t addr = 0x10020000;
	u32_t * mem;
	u32_t size;
	u8_t om;

	/*
	 * Read om register, om[4..1]
	 */
	om = (u8_t)((read32(addr) >> 1) & 0x0f);

	/* SDMMC CH2 */
	if(om == 0x2)
	{
		mem = (u32_t *)__image_start;
		size = (__image_end - __image_start + 0x00040000) >> 18;
		size = size << 9;
		irom_sdmmc_to_mem(1, size, mem);
	}
	/* eMMC43 CH0 */
	else if(om == 0x3)
	{
	}
	/* eMMC44 CH4 */
	else if(om == 0x4)
	{
	}
	/* NAND 512B 8ECC */
	else if(om == 0x8)
	{
	}
	/* NAND 2KB OVER */
	else if(om == 0x9)
	{
	}
}
