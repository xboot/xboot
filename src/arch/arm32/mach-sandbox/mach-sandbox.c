/*
 * mach-sandbox.c
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
#include <sandbox.h>

static bool_t mach_detect(void)
{
	return TRUE;
}

static bool_t mach_poweron(void)
{
	return (sandbox_pm_poweron() == 0) ? TRUE : FALSE;
}

static bool_t mach_poweroff(void)
{
	return (sandbox_pm_poweroff() == 0) ? TRUE : FALSE;
}

static bool_t mach_reboot(void)
{
	return (sandbox_pm_reboot() == 0) ? TRUE : FALSE;
}

static bool_t mach_sleep(void)
{
	return (sandbox_pm_sleep() == 0) ? TRUE : FALSE;
}

static bool_t mach_cleanup(void)
{
	return TRUE;
}

const char * mach_uniqueid(void)
{
	return NULL;
}

static struct machine_t sandbox = {
	.name 				= "sandbox",
	.desc 				= "xboot sandbox runtime enverionment",

	.banks = {
		[0] = {
			.start		= 0,
			.size		= 0,
		},
	},

	.detect 			= mach_detect,
	.poweron			= mach_poweron,
	.poweroff			= mach_poweroff,
	.reboot				= mach_reboot,
	.sleep				= mach_sleep,
	.cleanup			= mach_cleanup,
	.uniqueid			= mach_uniqueid,
};

static __init void mach_sandbox_init(void)
{
	if(register_machine(&sandbox))
		LOG("Register machine '%s'", sandbox.name);
	else
		LOG("Failed to register machine '%s'", sandbox.name);
}

static __exit void mach_sandbox_exit(void)
{
	if(unregister_machine(&sandbox))
		LOG("Unregister machine '%s'", sandbox.name);
	else
		LOG("Failed to unregister machine '%s'", sandbox.name);
}

core_initcall(mach_sandbox_init);
core_exitcall(mach_sandbox_exit);
