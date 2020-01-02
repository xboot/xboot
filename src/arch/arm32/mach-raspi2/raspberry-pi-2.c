/*
 * raspberry-pi-2.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <bcm2836-mbox.h>

static int mach_detect(struct machine_t * mach)
{
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
	virtual_addr_t virt = phys_to_virt(0x3f201000);
	static int initial = 0;
	int i;

	if(!initial)
	{
		u64_t clk = bcm2836_mbox_clock_get_rate(MBOX_CLOCK_ID_UART);
		u32_t div, rem, frac;

		div = clk / (16 * 115200);
		rem = clk % (16 * 115200);
		frac = (8 * rem / 115200) >> 1;
		frac += (8 * rem / 115200) & 1;

		write32(virt + 0x24, div);
		write32(virt + 0x28, frac);
		write32(virt + 0x2c, (0x3 << 5) | (0x0 << 3) | (0x0 << 1) | (0x1 << 4));
		write32(virt + 0x30, (1 << 0) | (1 << 8) | (1 << 9));
	}

	for(i = 0; i < count; i++)
	{
		while((read8(virt + 0x18) & (0x1 << 5)));
		write8(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[16 + 1] = { 0 };
	uint64_t sn;

	if(bcm2836_mbox_hardware_get_serial(&sn) < 0)
		return NULL;
	sprintf(uniqueid, "%02x%02x%02x%02x%02x%02x%02x%02x",
		(sn >> 56) & 0xff, (sn >> 48) & 0xff, (sn >> 40) & 0xff, (sn >> 32) & 0xff,
		(sn >> 24) & 0xff, (sn >> 16) & 0xff, (sn >> 8) & 0xff, (sn >> 0) & 0xff);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t raspberry_pi_2 = {
	.name 		= "raspberry-pi-2",
	.desc 		= "Raspberry Pi 2 Model B",
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

static __init void raspberry_pi_2_machine_init(void)
{
	register_machine(&raspberry_pi_2);
}

static __exit void raspberry_pi_2_machine_exit(void)
{
	unregister_machine(&raspberry_pi_2);
}

machine_initcall(raspberry_pi_2_machine_init);
machine_exitcall(raspberry_pi_2_machine_exit);
