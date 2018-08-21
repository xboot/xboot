/*
 * framework/event/l-event-dispatcher.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
