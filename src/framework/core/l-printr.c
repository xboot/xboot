/*
 * framework/core/l-printr.c
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

#include <framework/core/l-printr.h>

static const char printr_lua[] = X(
return function(o)
	if type(o) == "table" then
		local cache = {[o] = "."}
		local function dump(t, space, name)
			local temp = {}
			for k, v in pairs(t) do
				local key = tostring(k)
				if cache[v] then
					table.insert(temp, "+" .. key .. " {" .. cache[v] .. "}")
				elseif type(v) == "table" then
					local nkey = name .. "." .. key
					cache[v] = nkey
					table.insert(temp, "+" .. key .. dump(v, space .. (next(t, k) and "|" or " " ) .. string.rep(" ", #key), nkey))
				else
					table.insert(temp, "+" .. key .. " [" .. tostring(v) .. "]")
				end
			end
			return table.concat(temp, "\r\n" .. space)
		end
		print(dump(o, "", ""))
	else
		print(o)
	end
end
);

int luaopen_printr(lua_State * L)
{
	if(luaL_loadbuffer(L, printr_lua, sizeof(printr_lua) - 1, "Printr.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
