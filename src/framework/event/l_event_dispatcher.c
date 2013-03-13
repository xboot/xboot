/*
 * framework/event/l_event_dispatcher.c
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

#include <framework/event/l_event.h>

static int l_event_dispatcher_run(lua_State * L)
{
	event_base_dispatcher(runtime_get()->__event_base);
	return 0;
}

static int l_event_dispatcher_add_event_listener(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, MT_NAME_EVENT_LISTENER);

	event_base_add_event_listener(runtime_get()->__event_base, el);
	return 0;
}

static int l_event_dispatcher_del_event_listener(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, MT_NAME_EVENT_LISTENER);

	event_base_del_event_listener(runtime_get()->__event_base, el);
	return 0;
}

static const luaL_Reg l_event_dispatcher[] = {
	{"run",					l_event_dispatcher_run},
	{"add_event_listener",	l_event_dispatcher_add_event_listener},
	{"del_event_listener",	l_event_dispatcher_del_event_listener},
	{NULL, NULL}
};

int luaopen_event_dispatcher(lua_State * L)
{
	luaL_newlib(L, l_event_dispatcher);
	return 1;
}
