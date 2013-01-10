/*
 * framework/event/m_event_dispatch.c
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

#include <framework/framework.h>

static int l_event_dispatch_run(lua_State * L)
{
	event_base_dispatch(runtime_get()->__event_base);
	return 0;
}

static int l_event_dispatch_send(lua_State * L)
{
	event_send((struct event_t *)luaL_checkudata(L, 1, TYPE_EVENT));
	return 0;
}

static int l_event_dispatch_add_listener(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, TYPE_EVENT_LISTENER);

	event_base_add_listener(runtime_get()->__event_base, el);
	return 0;
}

static int l_event_dispatch_del_listener(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, TYPE_EVENT_LISTENER);

	event_base_del_listener(runtime_get()->__event_base, el);
	return 0;
}

static const luaL_Reg l_event_dispatch[] = {
	{"run",			l_event_dispatch_run},
	{"send",		l_event_dispatch_send},
	{"addlistener",	l_event_dispatch_add_listener},
	{"dellistener",	l_event_dispatch_del_listener},
	{NULL, NULL}
};

int luaopen_event_dispatch(lua_State * L)
{
	luaL_newlib(L, l_event_dispatch);
	return 1;
}
