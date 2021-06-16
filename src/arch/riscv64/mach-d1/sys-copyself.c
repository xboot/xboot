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
extern void sys_spinor_init(void);
extern void sys_spinor_exit(void);
extern void sys_spinor_read(int addr, void * buf, int count);

enum {
	BOOT_DEVICE_SPINOR	= 1,
	BOOT_DEVICE_SPINAND	= 2,
	BOOT_DEVICE_SDCARD	= 3,
};

static int get_boot_device(void)
{
	return BOOT_DEVICE_SPINOR;
}

void sys_copyself(void)
{
	int d = get_boot_device();
	void * mem;
	uint32_t size;

	if(d == BOOT_DEVICE_SPINOR)
	{
		mem = (void *)__image_start;
		size = __image_end - __image_start;

		sys_spinor_init();
		sys_spinor_read(0, mem, size);
		sys_spinor_exit();
	}
	else if(d == BOOT_DEVICE_SPINAND)
	{
	}
	else if(d == BOOT_DEVICE_SDCARD)
	{
	}
}
