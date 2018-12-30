/*
 * framework/hardware/l-nvmem.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

#include <nvmem/nvmem.h>
#include <framework/hardware/l-hardware.h>

static int l_nvmem_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct nvmem_t * m = name ? search_nvmem(name) : search_first_nvmem();
	if(!m)
		return 0;
	lua_pushlightuserdata(L, m);
	luaL_setmetatable(L, MT_HARDWARE_NVMEM);
	return 1;
}

static int l_nvmem_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct nvmem_t * m;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_NVMEM], head)
	{
		m = (struct nvmem_t *)(pos->priv);
		if(!m)
			continue;
		lua_pushlightuserdata(L, m);
		luaL_setmetatable(L, MT_HARDWARE_NVMEM);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_nvmem[] = {
	{"new",		l_nvmem_new},
	{"list",	l_nvmem_list},
	{NULL,	NULL}
};

static int m_nvmem_tostring(lua_State * L)
{
	struct nvmem_t * m = luaL_checkudata(L, 1, MT_HARDWARE_NVMEM);
	lua_pushstring(L, m->name);
	return 1;
}

static int m_nvmem_set(lua_State * L)
{
	struct nvmem_t * m = luaL_checkudata(L, 1, MT_HARDWARE_NVMEM);
	const char * key = luaL_checkstring(L, 2);
	const char * value = luaL_optstring(L, 3, NULL);
	nvmem_set(m, key, value);
	lua_settop(L, 1);
	return 1;
}

static int m_nvmem_get(lua_State * L)
{
	struct nvmem_t * m = luaL_checkudata(L, 1, MT_HARDWARE_NVMEM);
	const char * key = luaL_checkstring(L, 2);
	const char * def = luaL_optstring(L, 3, NULL);
	lua_pushstring(L, nvmem_get(m, key, def));
	return 1;
}

static int m_nvmem_clear(lua_State * L)
{
	struct nvmem_t * m = luaL_checkudata(L, 1, MT_HARDWARE_NVMEM);
	nvmem_clear(m);
	lua_settop(L, 1);
	return 1;
}

static int m_nvmem_sync(lua_State * L)
{
	struct nvmem_t * m = luaL_checkudata(L, 1, MT_HARDWARE_NVMEM);
	nvmem_sync(m);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_nvmem[] = {
	{"__tostring",	m_nvmem_tostring},
	{"set",			m_nvmem_set},
	{"get",			m_nvmem_get},
	{"clear",		m_nvmem_clear},
	{"sync",		m_nvmem_sync},
	{NULL,	NULL}
};

int luaopen_hardware_nvmem(lua_State * L)
{
	luaL_newlib(L, l_nvmem);
	luahelper_create_metatable(L, MT_HARDWARE_NVMEM, m_nvmem);
	return 1;
}
