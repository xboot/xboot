/*
 * framework/vm.c
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

#include <framework/vm.h>

struct vm_t * vm_alloc(const char * path, int argc, char * argv[])
{
	struct vm_t * vm;
	int i;

	if(!xfs_init(path))
		return NULL;

	vm = malloc(sizeof(struct vm_t));
	if(!vm)
		return NULL;

	vm->lua = luaL_newstate();

	luaL_openlibs(vm->lua);
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

	free(vm);
}

int vm_run(struct vm_t * vm)
{
	int ret = -1;

	if(!vm)
		return ret;

	luaopen_boot(vm->lua);
	lua_call(vm->lua, 0, 1);

	if(lua_isnumber(vm->lua, 1))
		ret = (int)lua_tonumber(vm->lua, 1);
	lua_pop(vm->lua, 1);

	return ret;
}

int vm_exec(const char * path, int argc, char * argv[])
{
	struct runtime_t * r;
	struct vm_t * vm;
	int ret = -1;

	if(!runtime_alloc_save(&r))
		return ret;

	vm = vm_alloc(path, argc, argv);
	if(!vm)
	{
		runtime_free_restore(r);
		return ret;
	}

	ret = vm_run(vm);

	vm_free(vm);
	runtime_free_restore(r);

	return ret;
}
