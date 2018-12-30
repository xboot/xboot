/*
 * framework/core/l-event-dispatcher.c
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

#include <framework/core/l-event-dispatcher.h>

static const char event_dispatcher_lua[] = X(
local table = table
local M = Class()

function M:init()
	self.eventListenerMap = {}
end

function M:hasEventListener(type, listener, data)
	local data = data or self
	local elm = self.eventListenerMap[type]

	if not elm or #elm == 0 then
		return false
	end

	for i, v in ipairs(elm) do
		if v.listener == listener and v.data == data then
			return true
		end
	end

	return false
end

function M:addEventListener(type, listener, data)
	local data = data or self

	if self:hasEventListener(type, listener, data) then
		return self
	end

	if not self.eventListenerMap[type] then
		self.eventListenerMap[type] = {}
	end

	local elm = self.eventListenerMap[type]
	local el = {type = type, listener = listener, data = data}
	table.insert(elm, el)

	return self
end

function M:removeEventListener(type, listener, data)
	local data = data or self
	local elm = self.eventListenerMap[type]

	if not elm or #elm == 0 then
		return self
	end

	for i, v in ipairs(elm) do
		if v.type == type and v.listener == listener and v.data == data then
			table.remove(elm, i)
			break
		end
	end

	return self
end

function M:dispatchEvent(event)
	if event.stop then
		return self
	end

	local elm = self.eventListenerMap[event.type]

	if not elm or #elm == 0 then
		return self
	end

	for i, v in ipairs(elm) do
		if v.type == event.type then
			v.listener(v.data, event)
		end
	end

	return self
end

return M
);

int luaopen_event_dispatcher(lua_State * L)
{
	if(luaL_loadbuffer(L, event_dispatcher_lua, sizeof(event_dispatcher_lua) - 1, "EventDispatcher.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
