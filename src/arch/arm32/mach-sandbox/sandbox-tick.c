/*
 * sandbox-tick.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

static void timer_callback(void)
{
	tick_interrupt();
}

static bool_t tick_timer_init(void)
{
	sandbox_sdl_timer_init(10, timer_callback);
	return TRUE;
}

static struct tick_t sandbox_tick = {
	.hz		= 100,
	.init	= tick_timer_init,
};

static __init void sandbox_tick_init(void)
{
	if(register_tick(&sandbox_tick))
		LOG("Register tick");
	else
		LOG("Failed to register tick");
}
core_initcall(sandbox_tick_init);
