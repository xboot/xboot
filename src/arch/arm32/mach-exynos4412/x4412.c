/*
 * x4412.c
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

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(0x1002330c);
	write32(virt, (read32(virt) & ~(0x3 << 8)) | (0x3 << 8));
	return 1;
}

static void mach_smpinit(struct machine_t * mach)
{
}

static void mach_smpboot(struct machine_t * mach, void (*func)(void))
{
}

static void mach_shutdown(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(0x1002330c);
	write32(virt, (read32(virt) & ~(0x3 << 8)) | (0x2 << 8));
}

static void mach_reboot(struct machine_t * mach)
{
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x13830000);
	int i;

	if((read32(virt + 0x8) & (1 << 0)))
	{
		for(i = 0; i < count; i++)
		{
			while((read32(virt + 0x18) & (1 << 24)));
			write8(virt + 0x20, buf[i]);
		}
	}
	else
	{
		for(i = 0; i < count; i++)
		{
			while(!(read32(virt + 0x10) & (1 << 1)));
			write8(virt + 0x20, buf[i]);
		}
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	return NULL;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t x4412 = {
	.name 		= "x4412",
	.desc 		= "X4412 Based On Samsung Exynos4412",
	.detect 	= mach_detect,
	.smpinit	= mach_smpinit,
	.smpboot	= mach_smpboot,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void x4412_machine_init(void)
{
	register_machine(&x4412);
}

static __exit void x4412_machine_exit(void)
{
	unregister_machine(&x4412);
}

machine_initcall(x4412_machine_init);
machine_exitcall(x4412_machine_exit);
