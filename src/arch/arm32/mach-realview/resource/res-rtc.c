/*
 * resource/res-rtc.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <realview-rtc.h>

static struct realview_rtc_data_t realview_rtc_data = {
	.regbase	= REALVIEW_RTC_BASE,
};

static struct resource_t res_rtc = {
	.name		= "realview-rtc",
	.id			= -1,
	.data		= &realview_rtc_data,
};

static __init void resource_rtc_init(void)
{
	if(register_resource(&res_rtc))
		LOG("Register resource %s:'%s.%d'", res_rtc.mach, res_rtc.name, res_rtc.id);
	else
		LOG("Failed to register resource %s:'%s.%d'", res_rtc.mach, res_rtc.name, res_rtc.id);
}

static __exit void resource_rtc_exit(void)
{
	if(unregister_resource(&res_rtc))
		LOG("Unregister resource %s:'%s.%d'", res_rtc.mach, res_rtc.name, res_rtc.id);
	else
		LOG("Failed to unregister resource %s:'%s.%d'", res_rtc.mach, res_rtc.name, res_rtc.id);
}

resource_initcall(resource_rtc_init);
resource_exitcall(resource_rtc_exit);
