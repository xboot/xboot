/*
 * framework/hardware/l-led_trigger.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <led/ledtrig.h>
#include <framework/hardware/l-hardware.h>

static int l_ledtrig_list(lua_State * L)
{
	struct device_list_t * pos, * n;
	struct ledtrig_t * trigger;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_LEDTRIG)
		{
			trigger = (struct ledtrig_t *)(pos->device->driver);
			if(!trigger)
				continue;

			lua_pushlightuserdata(L, trigger);
			luaL_setmetatable(L, MT_NAME_HARDWARE_LEDTRIG);
			lua_setfield(L, -2, pos->device->name);
		}
	}
	return 1;
}

static const luaL_Reg l_hardware_ledtrig[] = {
	{"list", l_ledtrig_list},
	{NULL, NULL}
};

static int m_ledtrig_activity(lua_State * L)
{
	struct ledtrig_t * trigger = luaL_checkudata(L, 1, MT_NAME_HARDWARE_LEDTRIG);
	ledtrig_activity(trigger);
	return 0;
}

static const luaL_Reg m_hardware_ledtrig[] = {
	{"activity", m_ledtrig_activity},
	{NULL, NULL}
};

int luaopen_hardware_ledtrig(lua_State * L)
{
	luaL_newlib(L, l_hardware_ledtrig);
	luahelper_create_metatable(L, MT_NAME_HARDWARE_LEDTRIG, m_hardware_ledtrig);
	return 1;
}
