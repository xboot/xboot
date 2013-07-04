/*
 * framework/timecounter/l_timecounter.c
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

#include <framework/timecounter/l_timecounter.h>

struct timecounter_t {
	double __current_time;
	double __last_time;
	double __start_time;
};

static double __time_stamp(void)
{
	return (double)jiffies / get_system_hz();
}

static int l_new(lua_State * L)
{
	struct timecounter_t * tc = lua_newuserdata(L, sizeof(struct timecounter_t));
	tc->__current_time = __time_stamp();
	tc->__last_time = tc->__current_time;
	tc->__start_time = tc->__current_time;
	luaL_setmetatable(L, MT_NAME_TIMECOUNTER);
	return 1;
}

static const luaL_Reg l_timecounter[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_reset(lua_State * L)
{
	struct timecounter_t * tc = luaL_checkudata(L, 1, MT_NAME_TIMECOUNTER);
	tc->__current_time = __time_stamp();
	tc->__last_time = tc->__current_time;
	tc->__start_time = tc->__current_time;
	return 0;
}

static int m_delta(lua_State * L)
{
	struct timecounter_t * tc = luaL_checkudata(L, 1, MT_NAME_TIMECOUNTER);
	tc->__last_time = tc->__current_time;
	tc->__current_time = __time_stamp();
	lua_pushnumber(L, tc->__current_time - tc->__last_time);
	return 1;
}

static int m_uptime(lua_State * L)
{
	struct timecounter_t * tc = luaL_checkudata(L, 1, MT_NAME_TIMECOUNTER);
	lua_pushnumber(L, __time_stamp() - tc->__start_time);
	return 1;
}

static const luaL_Reg m_timecounter[] = {
	{"reset", m_reset},
	{"delta", m_delta},
	{"uptime", m_uptime},
	{NULL, NULL}
};

int luaopen_timecounter(lua_State * L)
{
	luaL_newlib(L, l_timecounter);
	luahelper_create_metatable(L, MT_NAME_TIMECOUNTER, m_timecounter);
	return 1;
}
