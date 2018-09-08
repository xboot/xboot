/*
 * x3288.c
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
#include <rk3288/reg-cru.h>
#include <rk3288/reg-grf.h>

static int mach_detect(struct machine_t * mach)
{
	u32_t val;

	/*
	 * Select pwm solution, using rk_pwm.
	 */
	val = (1 << (16 + 0)) | (1 << 0);
	write32(phys_to_virt(RK3288_GRF_BASE + GRF_SOC_CON2), val);

	/*
	 * Select voltage lcdc(vop) iodomain 3.3V.
	 */
	val = (1 << (16 + 0)) | (0 << 0);
	write32(phys_to_virt(RK3288_GRF_BASE + GRF_IO_VSEL), val);

	return 1;
}

static void mach_memmap(struct machine_t * mach)
{
	machine_mmap(mach, "ram", 0x60000000, 0x60000000, SZ_128M, MAP_TYPE_CB);
	machine_mmap(mach, "dma", 0x68000000, 0x68000000, SZ_128M, MAP_TYPE_NCNB);
	machine_mmap(mach, "heap", 0x70000000, 0x70000000, SZ_256M, MAP_TYPE_CB);
	mmu_setup(mach);
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
	write32(phys_to_virt(RK3288_CRU_BASE) + CRU_GLB_SRST_FST_VALUE, 0xfdb9);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0xff690000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x7c) & (0x1 << 1)) == 0);
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

static struct machine_t x3288 = {
	.name 		= "x3288",
	.desc 		= "X3288 Based On RK3288 SOC",
	.detect 	= mach_detect,
	.memmap		= mach_memmap,
	.shutdown	= mach_shutdown,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.logger		= mach_logger,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void x3288_machine_init(void)
{
	register_machine(&x3288);
}

static __exit void x3288_machine_exit(void)
{
	unregister_machine(&x3288);
}

machine_initcall(x3288_machine_init);
machine_exitcall(x3288_machine_exit);
