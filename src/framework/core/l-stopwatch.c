/*
 * framework/core/l-stopwatch.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <core/l-stopwatch.h>

#define	MT_STOPWATCH	"__mt_stopwatch__"

struct stopwatch_t {
	uint64_t start;
};

static int l_new(lua_State * L)
{
	struct stopwatch_t * stopwatch = lua_newuserdata(L, sizeof(struct stopwatch_t));
	stopwatch->start = ktime_to_ns(ktime_get());
	luaL_setmetatable(L, MT_STOPWATCH);
	return 1;
}

static const luaL_Reg l_stopwatch[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_elapsed(lua_State * L)
{
	struct stopwatch_t * stopwatch = luaL_checkudata(L, 1, MT_STOPWATCH);
	task_yield();
	lua_pushnumber(L, (lua_Number)(ktime_to_ns(ktime_get()) - stopwatch->start) / (lua_Number)1000000000.0);
	return 1;
}

static int m_reset(lua_State * L)
{
	struct stopwatch_t * stopwatch = luaL_checkudata(L, 1, MT_STOPWATCH);
	stopwatch->start = ktime_to_ns(ktime_get());
	return 0;
}

static const luaL_Reg m_stopwatch[] = {
	{"elapsed",	m_elapsed},
	{"reset",	m_reset},
	{NULL,		NULL}
};

int luaopen_stopwatch(lua_State * L)
{
	luaL_newlib(L, l_stopwatch);
	luahelper_create_metatable(L, MT_STOPWATCH, m_stopwatch);
	return 1;
}
