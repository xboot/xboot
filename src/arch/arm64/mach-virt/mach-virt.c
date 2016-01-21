/*
 * mach-virt.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>

static const struct mmap_t mach_map[] = {
	{ 0 },
};

static bool_t mach_detect(void)
{
	return TRUE;
}

static bool_t mach_poweron(void)
{
	return TRUE;
}

static bool_t mach_poweroff(void)
{
	return FALSE;
}

static bool_t mach_reboot(void)
{
	return FALSE;
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_cleanup(void)
{
	return TRUE;
}

static const char * mach_uniqueid(void)
{
	return NULL;
}

static int mach_keygen(const char * msg, void * key)
{
	return 0;
}

static struct machine_t virt = {
	.name 		= "VIRT",
	.desc 		= "ARM Virtual Machine For Cortex-A57",
	.map		= mach_map,
	.detect 	= mach_detect,
	.poweron	= mach_poweron,
	.poweroff	= mach_poweroff,
	.reboot		= mach_reboot,
	.sleep		= mach_sleep,
	.cleanup	= mach_cleanup,
	.uniqueid	= mach_uniqueid,
	.keygen		= mach_keygen,
};

static __init void mach_virt_init(void)
{
	if(register_machine(&virt))
		LOG("Register machine '%s'", virt.name);
	else
		LOG("Failed to register machine '%s'", virt.name);
}

static __exit void mach_virt_exit(void)
{
	if(unregister_machine(&virt))
		LOG("Unregister machine '%s'", virt.name);
	else
		LOG("Failed to unregister machine '%s'", virt.name);
}

core_initcall(mach_virt_init);
core_exitcall(mach_virt_exit);
