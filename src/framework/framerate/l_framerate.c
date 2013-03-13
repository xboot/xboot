/*
 * framework/framerate/l_framerate.c
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

#include <framework/framerate/l_framerate.h>

struct framerate_t {
	u32_t frames;
	double timestamp_prev;
	double timestamp_curr;
	double timestamp_update_fps;
	double dt;
	double fps;
};

static double get_time_stamp(void)
{
	return (double)jiffies / get_system_hz();
}

static int l_new(lua_State * L)
{
	struct framerate_t * fr = lua_newuserdata(L, sizeof(struct framerate_t));
	memset(fr, 0, sizeof(struct framerate_t));
	luaL_setmetatable(L, MT_NAME_FRAMERATE);
	return 1;
}

static const luaL_Reg l_framerate[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_step(lua_State * L)
{
	struct framerate_t * fr = luaL_checkudata(L, 1, MT_NAME_FRAMERATE);
	double t;

	fr->frames++;
	fr->timestamp_prev = fr->timestamp_curr;
	fr->timestamp_curr = get_time_stamp();
	fr->dt = fr->timestamp_curr - fr->timestamp_prev;

	t = fr->timestamp_curr - fr->timestamp_update_fps;
	if(t > 1)
	{
		fr->fps = fr->frames / t;
		fr->timestamp_update_fps = fr->timestamp_curr;
		fr->frames = 0;
	}

	return 0;
}

static int m_sleep(lua_State * L)
{
	double s = luaL_checknumber(L, 2);
	if(s > 0)
		mdelay((u32_t)(s * 1000));
	return 0;
}

static int m_gettime(lua_State * L)
{
	lua_pushnumber(L, get_time_stamp());
	return 1;
}

static int m_getdelta(lua_State * L)
{
	struct framerate_t * fr = luaL_checkudata(L, 1, MT_NAME_FRAMERATE);
	lua_pushnumber(L, fr->dt);
	return 1;
}

static int m_getfps(lua_State * L)
{
	struct framerate_t * fr = luaL_checkudata(L, 1, MT_NAME_FRAMERATE);
	lua_pushnumber(L, fr->fps);
	return 1;
}

static const luaL_Reg m_framerate[] = {
	{"step", m_step},
	{"sleep", m_sleep},
	{"gettime", m_gettime},
	{"getdelta", m_getdelta},
	{"getfps", m_getfps},
	{NULL, NULL}
};

int luaopen_framerate(lua_State * L)
{
	luaL_newlib(L, l_framerate);
	luahelper_create_metatable(L, MT_NAME_FRAMERATE, m_framerate);
	return 1;
}
