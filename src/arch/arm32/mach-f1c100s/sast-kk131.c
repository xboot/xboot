/*
 * sast-kk131.c
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
#include <mmu.h>

static int mach_detect(struct machine_t * mach)
{
	return 1;
}

static void mach_memmap(struct machine_t * mach)
{
	machine_mmap(mach, "ram", 0x80000000, 0x80000000, SZ_1M * 8, MAP_TYPE_CB);
	machine_mmap(mach, "dma", 0x80800000, 0x80800000, SZ_1M * 8, MAP_TYPE_NCNB);
	machine_mmap(mach, "heap", 0x81000000, 0x81000000, SZ_1M * 16, MAP_TYPE_CB);
	mmu_setup(mach);
}

static void mach_smpinit(struct machine_t * mach, int cpu)
{
}

static void mach_smpboot(struct machine_t * mach, int cpu, void (*func)(int cpu))
{
}

static void mach_shutdown(struct machine_t * mach)
{
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
	virtual_addr_t virt = phys_to_virt(0x01c25000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
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

static struct machine_t sast_kk131 = {
	.name 		= "sast-kk131",
	.desc 		= "SAST KK131 Digital Player Based On Allwinner F1C100S",
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
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

static __init void sast_kk131_machine_init(void)
{
	register_machine(&sast_kk131);
}

static __exit void sast_kk131_machine_exit(void)
{
	unregister_machine(&sast_kk131);
}

machine_initcall(sast_kk131_machine_init);
machine_exitcall(sast_kk131_machine_exit);
