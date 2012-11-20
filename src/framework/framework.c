/*
 * framework/framework.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

#include <framework/xfs/xfs.h>
#include <framework/framework.h>

extern int luaopen_xboot_sample (lua_State * L);
extern int luaopen_xboot_boot(lua_State * L);
//-------------------------------------------

static bool_t vm_preload(lua_State * L, const char * name, lua_CFunction f)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);

	return TRUE;
}

static const luaL_Reg xboot_libs[] = {
	{ "xboot.sample", luaopen_xboot_sample },
	{ "xboot.boot", luaopen_xboot_boot },
	{ NULL, NULL }
};

int luaopen_xboot(lua_State * L)
{
	int i;

	lua_getglobal(L, "xboot");
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "xboot");
	}

	lua_pushstring(L, XBOOT_VERSION);
	lua_setfield(L, -2, "_VERSION");

	lua_pushnumber(L, XBOOT_MAJOY);
	lua_setfield(L, -2, "_MAJOY");

	lua_pushnumber(L, XBOOT_MINIOR);
	lua_setfield(L, -2, "_MINIOR");

	lua_pushnumber(L, XBOOT_PATCH);
	lua_setfield(L, -2, "_PATCH");

	lua_pushstring(L, __ARCH__);
	lua_setfield(L, -2, "_ARCH");

	lua_pushstring(L, __MACH__);
	lua_setfield(L, -2, "_MACH");

	for(i = 0; xboot_libs[i].name != 0; i++)
	{
		vm_preload(L, xboot_libs[i].name, xboot_libs[i].func);
	}

	return 1;
}

struct vm_t * vm_alloc(const char * path, int argc, char * argv[])
{
	struct vm_t * vm;
	int i;

	vm = malloc(sizeof(struct vm_t));
	if(!vm)
		return NULL;

	vm->lua = luaL_newstate();
	vm->xfs = __xfs_init(NULL);

	luaL_openlibs(vm->lua);

	vm_preload(vm->lua, "xboot", luaopen_xboot);
	luaopen_xboot(vm->lua);

	do {
		lua_newtable(vm->lua);

		if(argc > 0)
		{
			lua_pushstring(vm->lua, argv[0]);
			lua_rawseti(vm->lua, -2, -2);
		}

		lua_pushstring(vm->lua, "embedded boot.lua");
		lua_rawseti(vm->lua, -2, -1);

		for(i = 1; i < argc; i++)
		{
			lua_pushstring(vm->lua, argv[i]);
			lua_rawseti(vm->lua, -2, i);
		}

		lua_setglobal(vm->lua, "arg");
	} while(0);

	return vm;
}

void vm_free(struct vm_t * vm)
{
	if(!vm)
		return;

	if(vm->lua)
		lua_close(vm->lua);

	if(vm->xfs)
		__xfs_exit(vm->xfs);

	free(vm);
}

int vm_run(struct vm_t * vm)
{
	int ret = -1;

	if(!vm)
		return ret;

	luaopen_xboot_boot(vm->lua);
	lua_call(vm->lua, 0, 1);

	if(lua_isnumber(vm->lua, 1))
		ret = (int)lua_tonumber(vm->lua, 1);
	lua_pop(vm->lua, 1);

	return ret;
}

int vm_test(const char * path, int argc, char * argv[])
{
	struct vm_t * vm;
	int ret = -1;

	vm = vm_alloc(path, argc, argv);
	if(!vm)
		return ret;

	ret = vm_run(vm);
	vm_free(vm);

	return ret;
}
