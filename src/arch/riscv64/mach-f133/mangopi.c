/*
 * mangopi.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <sha256.h>
#include <ecdsa256.h>

static int mach_detect(struct machine_t * mach)
{
	return 1;
}

static void mach_smpinit(struct machine_t * mach)
{
}

static void mach_smpboot(struct machine_t * mach, void (*func)(void))
{
	extern void sys_smp_secondary_boot(void (*func)(void));
	sys_smp_secondary_boot(func);
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
	virtual_addr_t virt = phys_to_virt(0x02500000);
	int i;

	for(i = 0; i < count; i++)
	{
		while((read32(virt + 0x14) & (0x1 << 6)) == 0);
		write32(virt + 0x00, buf[i]);
	}
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	static char uniqueid[32 + 1] = { 0 };
	virtual_addr_t virt = phys_to_virt(0x03006200);
	uint32_t sid[4];

	sid[0] = read32(virt + 0 * 4);
	sid[1] = read32(virt + 1 * 4);
	sid[2] = read32(virt + 2 * 4);
	sid[3] = read32(virt + 3 * 4);
	snprintf(uniqueid, sizeof(uniqueid), "%08x%08x%08x%08x",sid[0], sid[1], sid[2], sid[3]);
	return uniqueid;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static int mach_verify(struct machine_t * mach)
{
	const uint8_t pubkey[33] = {
		0x03, 0xcf, 0xd1, 0x8e, 0x4a, 0x4b, 0x40, 0xd6,
		0x52, 0x94, 0x48, 0xaa, 0x2d, 0xf8, 0xbb, 0xb6,
		0x77, 0x12, 0x82, 0x58, 0xb8, 0xfb, 0xfc, 0x5b,
		0x9e, 0x49, 0x2f, 0xbb, 0xba, 0x4e, 0x84, 0x83,
		0x2f,
	};
	virtual_addr_t virt = phys_to_virt(0x03006200);
	uint8_t signature[64];
	uint32_t * p = (uint32_t *)&signature[0];

	for(int i = 0; i < sizeof(signature) / 4; i++)
		p[i] = read32(virt + 0x00c0 + i * 4);
	const char * uniqueid = machine_uniqueid();
	if(uniqueid)
	{
		uint8_t sha256[32];
		sha256_hash(uniqueid, strlen(uniqueid), sha256);
		return ecdsa256_verify(pubkey, sha256, signature);
	}
	return 0;
}

static struct machine_t mangopi = {
	.name 		= "mangopi",
	.desc 		= "MangoPi-MQ Based On Allwinner F133 SOC",
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
	.verify		= mach_verify,
};

static __init void mangopi_machine_init(void)
{
	register_machine(&mangopi);
}

static __exit void mangopi_machine_exit(void)
{
	unregister_machine(&mangopi);
}

machine_initcall(mangopi_machine_init);
machine_exitcall(mangopi_machine_exit);
