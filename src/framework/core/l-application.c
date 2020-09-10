/*
 * framework/core/l-application.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <xfs/xfs.h>
#include <framework/core/l-image.h>
#include <framework/core/l-application.h>

struct lapplication_t {
	struct package_t * pkg;
};

static int l_application_new(lua_State * L)
{
	const char * path = ((struct vmctx_t *)luahelper_vmctx(L))->w->task->name;
	struct package_t * pkg = package_search(path);
	struct lapplication_t * app;
	if(pkg)
	{
		app = lua_newuserdata(L, sizeof(struct lapplication_t));
		app->pkg = pkg;
		luaL_setmetatable(L, MT_APPLICATION);
		return 1;
	}
	return 0;
}

static int l_application_list(lua_State * L)
{
	struct lapplication_t * app;
	struct hmap_entry_t * e;

	lua_newtable(L);
	hmap_for_each_entry(e, __package_list)
	{
		app = lua_newuserdata(L, sizeof(struct lapplication_t));
		app->pkg = e->value;
		luaL_setmetatable(L, MT_APPLICATION);
		lua_setfield(L, -2, e->key);
	}
	return 1;
}

static const luaL_Reg l_application[] = {
	{"new",		l_application_new},
	{"list",	l_application_list},
	{NULL,	NULL}
};

static int m_application_get_path(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_path(app->pkg));
	return 1;
}

static int m_application_get_name(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_name(app->pkg));
	return 1;
}

static int m_application_get_description(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_desc(app->pkg));
	return 1;
}

static int m_application_get_developer(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_developer(app->pkg));
	return 1;
}

static int m_application_get_version(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_version(app->pkg));
	return 1;
}

static int m_application_get_url(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	lua_pushstring(L, package_get_url(app->pkg));
	return 1;
}

static int m_application_get_icon(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	if(!app->pkg->icon)
		return 0;
	struct limage_t * img = lua_newuserdata(L, sizeof(struct limage_t));
	img->s = surface_clone(package_get_icon(app->pkg), 0, 0, 0, 0, 0);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static int m_application_get_panel(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	if(!app->pkg->panel)
		return 0;
	struct limage_t * img = lua_newuserdata(L, sizeof(struct limage_t));
	img->s = surface_clone(package_get_panel(app->pkg), 0, 0, 0, 0, 0);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static int m_application_execute(lua_State * L)
{
	struct lapplication_t * app = luaL_checkudata(L, 1, MT_APPLICATION);
	const char * fb = luaL_optstring(L, 2, NULL);
	const char * input = luaL_optstring(L, 3, NULL);
	lua_pushboolean(L, (vmexec(package_get_path(app->pkg), fb, input) < 0) ? 0 : 1);
	return 1;
}

static const luaL_Reg m_application[] = {
	{"getPath",			m_application_get_path},
	{"getName",			m_application_get_name},
	{"getDescription",	m_application_get_description},
	{"getDeveloper",	m_application_get_developer},
	{"getVersion",		m_application_get_version},
	{"getUrl",			m_application_get_url},
	{"getIcon",			m_application_get_icon},
	{"getPanel",		m_application_get_panel},
	{"execute",			m_application_execute},
	{NULL,	NULL}
};

int luaopen_application(lua_State * L)
{
	luaL_newlib(L, l_application);
	luahelper_create_metatable(L, MT_APPLICATION, m_application);
	return 1;
}
