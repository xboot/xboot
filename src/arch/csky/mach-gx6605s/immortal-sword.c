/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s.h>

static int mach_detect(struct machine_t * mach)
{
	machine_mmap(mach, "sseg1", SSEG1_BASE, 0x00000000, SZ_256M, 0);
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
	while(count--)
		sys_uart_putc(*buf++);
}

static const char * mach_uniqueid(struct machine_t * mach)
{
	return NULL;
}

static int mach_keygen(struct machine_t * mach, const char * msg, void * key)
{
	return 0;
}

static struct machine_t immortal_sword = {
	.name		= "immortal-sword",
	.desc		= "C-Sky CK610m Development Platform",
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

static __init void immortal_sword_machine_init(void)
{
	register_machine(&immortal_sword);
}

static __exit void immortal_sword_machine_exit(void)
{
	unregister_machine(&immortal_sword);
}

machine_initcall(immortal_sword_machine_init);
machine_exitcall(immortal_sword_machine_exit);
