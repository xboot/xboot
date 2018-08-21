/*
 * yi-smart-camera.c
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

static const struct mmap_t mach_map[] = {
	{"ram",  0x80000000, 0x80000000, SZ_1M * 8, MAP_TYPE_CB},
	{"dma",  0x80800000, 0x80800000, SZ_1M * 8, MAP_TYPE_NCNB},
	{"heap", 0x81000000, 0x81000000, SZ_1M * 16, MAP_TYPE_CB},
	{ 0 },
};

static int mach_detect(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(0x20050000);
	uint32_t id = (((read32(virt + 0xeec) & 0xff) << 24) |
				((read32(virt + 0xee8) & 0xff) << 16) |
				((read32(virt + 0xee4) & 0xff) <<  8) |
				((read32(virt + 0xee0) & 0xff) <<  0));

	if(id == 0x35180100)
		return 1;
	return 0;
}

static void mach_memmap(struct machine_t * mach)
{
	mmu_setup(mach->map);
}

static void mach_shutdown(struct machine_t * mach)
{
}

static void mach_reboot(struct machine_t * mach)
{
	virtual_addr_t virt = phys_to_virt(0x20050000);

	write32(virt + 0x44, 0x1acce551);
	write32(virt + 0x04, 0x00000001);
	write32(virt + 0x44, 0x00000000);
}

static void mach_sleep(struct machine_t * mach)
{
}

static void mach_cleanup(struct machine_t * mach)
{
}

static void mach_logger(struct machine_t * mach, const char * buf, int count)
{
	virtual_addr_t virt = phys_to_virt(0x20080000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read8(virt + 0x18) & (0x1 << 5)));
		write8(virt + 0x00, buf[i]);
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

static struct machine_t yi_smart_camera = {
	.name 		= "yi-smart-camera",
	.desc 		= "XiaoMi YI Smart Camera Based On Hi3518e",
	.map		= mach_map,
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

static __init void yi_smart_camera_machine_init(void)
{
	register_machine(&yi_smart_camera);
}

static __exit void yi_smart_camera_machine_exit(void)
{
	unregister_machine(&yi_smart_camera);
}

machine_initcall(yi_smart_camera_machine_init);
machine_exitcall(yi_smart_camera_machine_exit);
