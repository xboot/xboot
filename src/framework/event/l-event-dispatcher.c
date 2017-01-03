/*
 * framework/event/l-event-dispatcher.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <framework/event/l-event-dispatcher.h>

static const char event_dispatcher_lua[] =
"local M = Class()"																"\n"
""																				"\n"
"function M:init()"																"\n"
"	self.maps = {}"																"\n"
"end"																			"\n"
""																				"\n"
"function M:hasEventListener(type, listener, data)"								"\n"
"	local els = self.maps[type]"												"\n"
""																				"\n"
"	if not els or #els == 0 then"												"\n"
"		return false"															"\n"
"	end"																		"\n"
""																				"\n"
"	for i, v in ipairs(els) do"													"\n"
"		if v.listener == listener and v.data == data then"						"\n"
"			return true"														"\n"
"		end"																	"\n"
"	end"																		"\n"
""																				"\n"
"	return false"																"\n"
"end"																			"\n"
""																				"\n"
"function M:addEventListener(type, listener, data)"								"\n"
"	local table = table"														"\n"
"	local data = data or self"													"\n"
""																				"\n"
"	if self:hasEventListener(type, listener, data) then"						"\n"
"		return self"															"\n"
"	end"																		"\n"
""																				"\n"
"	if not self.maps[type] then"												"\n"
"		self.maps[type] = {}"													"\n"
"	end"																		"\n"
""																				"\n"
"	local els = self.maps[type]"												"\n"
"	local el = {type = type, listener = listener, data = data}"					"\n"
"	table.insert(els, el)"														"\n"
""																				"\n"
"	return self"																"\n"
"end"																			"\n"
""																				"\n"
"function M:removeEventListener(type, listener, data)"							"\n"
"	local table = table"														"\n"
"	local data = data or self"													"\n"
"	local els = self.maps[type]"												"\n"
""																				"\n"
"	if not els or #els == 0 then"												"\n"
"		return self"															"\n"
"	end"																		"\n"
""																				"\n"
"	for i, v in ipairs(els) do"													"\n"
"		if v.type == type and v.listener == listener and v.data == data then"	"\n"
"			table.remove(els, i)"												"\n"
"			break"																"\n"
"		end"																	"\n"
"	end"																		"\n"
""																				"\n"
"	return self"																"\n"
"end"																			"\n"
""																				"\n"
"function M:dispatchEvent(event)"												"\n"
"	if event.stop then"															"\n"
"		return self"															"\n"
"	end"																		"\n"
""																				"\n"
"	local els = self.maps[event.type]"											"\n"
""																				"\n"
"	if not els or #els == 0 then"												"\n"
"		return self"															"\n"
"	end"																		"\n"
""																				"\n"
"	for i, v in ipairs(els) do"													"\n"
"		if v.type == event.type then"											"\n"
"			v.listener(v.data, event)"											"\n"
"		end"																	"\n"
"	end"																		"\n"
""																				"\n"
"	return self"																"\n"
"end"																			"\n"
""																				"\n"
"return M"																		"\n"
;

int luaopen_event_dispatcher(lua_State * L)
{
	if(luaL_loadbuffer(L, event_dispatcher_lua, sizeof(event_dispatcher_lua)-1, "EventDispatcher.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
