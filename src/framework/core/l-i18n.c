/*
 * framework/core/l-i18n.c
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

#include <framework/core/l-i18n.h>

static const char i18n_lua[] = X(
local M = Class()

local function loadlang(path)
	local f = loadfile(path)
	if f and type(f) == "function" then
		return f()
	end
end

function M:init(lang)
	local l = loadlang("assets/i18n/en-US.lua")
	self._language = type(l) == "table" and l or {}
	if lang and lang ~= "en-US" then
		local o = loadlang("assets/i18n/" .. lang .. ".lua")
		if type(o) == "table" then
			for k, v in pairs(o) do
				if v then
					self._language[k] = v
				end
			end
		end
	end
end

function M:__call(text)
	return self._language[text] or text
end

return M
);

int luaopen_i18n(lua_State * L)
{
	if(luaL_loadbuffer(L, i18n_lua, sizeof(i18n_lua) - 1, "I18n.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
