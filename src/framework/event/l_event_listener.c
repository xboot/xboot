/*
 * framework/event/l_event_listener.c
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

struct event_listener_callback_data_t {
	lua_State * l;
	int r;
};

static void __event_listener_callback(struct event_t * event, void * data)
{
	struct event_listener_callback_data_t * d = (struct event_listener_callback_data_t *)data;
	struct event_t * e;

	if(!d || !event)
		return;

	/* push function */
	lua_rawgeti(d->l, LUA_REGISTRYINDEX, d->r);

	/* push event */
	e = (struct event_t *)lua_newuserdata(d->l, sizeof(struct event_t));
	memcpy(e, event, sizeof(struct event_t));
	luaL_setmetatable(d->l, MT_NAME_EVENT);

	/* call function */
	lua_pcall(d->l, 1, 0, 0);
}

static int l_event_listener_new(lua_State * L)
{
	struct event_listener_t * el;
	struct event_listener_callback_data_t * d;
	enum event_type_t type;
	int r;

	luaL_checktype(L, 1, LUA_TNUMBER);
	luaL_checktype(L, 2, LUA_TFUNCTION);

	type = (enum event_type_t)luaL_optint(L, 1, 0);
	lua_pushvalue(L, 2);
	r = luaL_ref(L, LUA_REGISTRYINDEX);

	d = malloc(sizeof(struct event_listener_callback_data_t));
	if(!d)
		return 0;
	d->l = L;
	d->r = r;

	el = (struct event_listener_t *)lua_newuserdata(L, sizeof(struct event_listener_t));
	el->type = type;
	el->callback = __event_listener_callback;
	el->data = d;
	luaL_setmetatable(L, MT_NAME_EVENT_LISTENER);

	return 1;
}

static const luaL_Reg l_event_listener[] = {
	{"new",			l_event_listener_new},
	{NULL, NULL}
};

static int m_event_listener_gc(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, MT_NAME_EVENT_LISTENER);
	struct event_listener_callback_data_t * d;

	if(el && (d = el->data))
	{
		luaL_unref(d->l, LUA_REGISTRYINDEX, d->r);
		free(d);
	}
	return 0;
}

static int m_event_listener_tostring(lua_State * L)
{
	struct event_listener_t * el = (struct event_listener_t *)luaL_checkudata(L, 1, MT_NAME_EVENT_LISTENER);

	lua_pushfstring(L, "%s %p", MT_NAME_EVENT_LISTENER, (void *)el);
	return 1;
}

static const luaL_Reg m_event_listener[] = {
	{"__gc",		m_event_listener_gc},
	{"__tostring",	m_event_listener_tostring},
	{NULL, NULL}
};

int luaopen_event_listener(lua_State * L)
{
	luaL_newlib(L, l_event_listener);

	luaL_newmetatable(L, MT_NAME_EVENT_LISTENER);
	luaL_setfuncs(L, m_event_listener, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	return 1;
}
