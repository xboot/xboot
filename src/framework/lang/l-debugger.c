/*
 * framework/lang/l-debugger.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <framework/lang/l-class.h>

static const char debugger_lua[] = X(
local local_stack_level = 6
local stack_top = 0
local stack_offset = 0
local repl
local debugger

local function format_stack_frame_info(info)
	local fname = (info.name or string.format('<%s:%d>', info.short_src, info.linedefined))
	return string.format('%s:%d in %s', info.short_src, info.currentline, fname)
end

local function hook_factory(repl_threshold)
	return function(offset)
		return function(event, _)
			local info = debug.getinfo(2)
			if event == 'call' and info.linedefined >= 0 then
				offset = offset + 1
			elseif event == 'return' and info.linedefined >= 0 then
				if offset <= repl_threshold then
				else
					offset = offset - 1
				end
			elseif event == 'line' and offset <= repl_threshold then
				repl()
			end
		end
	end
end

local hook_step = hook_factory(1)
local hook_next = hook_factory(0)
local hook_finish = hook_factory(-1)

local function table_merge(t1, t2)
	local tbl = {}
	for k, v in pairs(t1) do tbl[k] = v end
	for k, v in pairs(t2) do tbl[k] = v end
	return tbl
end

local function local_bindings(offset, include_globals)
	local level = stack_offset + offset + local_stack_level
	local func = debug.getinfo(level).func
	local bindings = {}

	do local i = 1; repeat
		local name, value = debug.getupvalue(func, i)
		if name then bindings[name] = value end
		i = i + 1
	until name == nil end

	do local i = 1; repeat
		local name, value = debug.getlocal(level, i)
		if name then bindings[name] = value end
		i = i + 1
	until name == nil end

	local varargs = {}
	do local i = -1; repeat
		local name, value = debug.getlocal(level, i)
		table.insert(varargs, value)
		i = i - 1
	until name == nil end
	if #varargs ~= 0 then bindings['...'] = varargs end

	if include_globals then
		local env = bindings._ENV
		return setmetatable(table_merge(env or {}, bindings), {__index = _G})
	else
		return bindings
	end
end

local function compile_chunk(expr, env)
	local source = 'debugger REPL'
	return load('return ' .. expr, source, 't', env)
end

local function cmd_print(expr)
	local env = local_bindings(1, true)
	local chunk = compile_chunk(expr, env)
	if chunk == nil then
		debugger.write('Error: Could not evaluate expression.')
		return false
	end

	local results = {pcall(chunk, table.unpack(rawget(env, '...') or {}))}
	if not results[1] then
		debugger.write('Error: %s', results[2])
	elseif #results == 1 then
		debugger.write(expr .. ' => <no result>')
	else
		local result = ''
		for i = 2, #results do
			result = result .. (i ~= 2 and ', ' or '') .. debugger.pretty(results[i])
		end
		debugger.write(expr .. ' => ' .. result)
	end
	return false
end

local function cmd_up()
	local info = debug.getinfo(stack_offset + local_stack_level + 1)
	if info then
		stack_offset = stack_offset + 1
	else
		debugger.write('Already at the top of the stack.')
	end
	debugger.write('Inspecting frame: ' .. format_stack_frame_info(debug.getinfo(stack_offset + local_stack_level)))
	return false
end

local function cmd_down()
	if stack_offset > stack_top then
		stack_offset = stack_offset - 1
	else
		debugger.write('Already at the bottom of the stack.')
	end
	debugger.write('Inspecting frame: ' .. format_stack_frame_info(debug.getinfo(stack_offset + local_stack_level)))
	return false
end

local function cmd_trace()
	local location = format_stack_frame_info(debug.getinfo(stack_offset + local_stack_level))
	local offset = stack_offset - stack_top
	local message = string.format('Inspecting frame: %d - (%s)', offset, location)
	local str = debug.traceback(message, stack_top + local_stack_level)

	local line_num = -2
	while str and #str ~= 0 do
		local line, rest = string.match(str, '([^\\n]*)\\n?(.*)')
		str = rest
		if line_num >= 0 then line = tostring(line_num)..line end
		debugger.write((line_num + stack_top == stack_offset) and line or line)
		line_num = line_num + 1
	end
	return false
end

local function cmd_locals()
	local bindings = local_bindings(1, false)
	local keys = {}
	for k, _ in pairs(bindings) do table.insert(keys, k) end
	table.sort(keys)
	for _, k in ipairs(keys) do
		local v = bindings[k]

		if not rawequal(v, debugger) and k ~= '_ENV' and k ~= '(*temporary)' then
			debugger.write('\t%s => %s', k, debugger.pretty(v))
		end
	end
	return false
end

local function match_command(line)
	local commands = {
		['c'] = function() return true end,
		['s'] = function() return true, hook_step end,
		['n'] = function() return true, hook_next end,
		['f'] = function() return true, hook_finish end,
		['p%s?(.*)'] = cmd_print,
		['u'] = cmd_up,
		['d'] = cmd_down,
		['t'] = cmd_trace,
		['l'] = cmd_locals,
		['h'] = function()
				debugger.write('[enter]     - enter last command');
				debugger.write('c(ontinue)  - contiue execution');
				debugger.write('s(tep)      - step forward by one line (into functions)');
				debugger.write('n(ext)      - step forward by one line (skipping over functions)');
				debugger.write('f(inish)    - step forward until exiting the current function');
				debugger.write('p(rint) [e] - execute the expression and print the result');
				debugger.write('u(p)        - move up the stack by one frame');
				debugger.write('d(own)      - move down the stack by one frame');
				debugger.write('t(race)     - print the stack trace');
				debugger.write('l(ocals)    - print the function arguments, locals and upvalues.');
				debugger.write('h(elp)      - print this message');
				return false
			end,
	}

	for cmd, cmd_func in pairs(commands) do
		local matches = {string.match(line, '^('..cmd..')$')}
		if matches[1] then
			return cmd_func, select(2, table.unpack(matches))
		end
	end
end

local last_cmd = false
local function run_command(line)
	if line == nil or line == '' then
		if last_cmd then line = last_cmd else return false end
	else
		last_cmd = line
	end

	local command, command_arg = match_command(line)
	if command then
		return table.unpack({command(command_arg)})
	else
		debugger.write('Command %s not recognized', line)
		return false
	end
end

repl = function()
	debugger.write(format_stack_frame_info(debug.getinfo(local_stack_level - 3 + stack_top)))

	repeat
		local success, done, hook = pcall(run_command, debugger.read('>> '))
		if success then
			debug.sethook(hook and hook(0), 'crl')
		else
			local message = string.format('Internal debugger error: %s', done)
			debugger.write(message)
			error(message)
		end
	until done
end

debugger = setmetatable({}, {
	__call = function(self, condition, offset)
		if condition then return end
		offset = (offset or 0)
		stack_offset = offset
		stack_top = offset
		debug.sethook(hook_next(1), 'crl')
		return
	end,
})

function debugger.read(prompt)
	return xboot.readline(prompt)
end

function debugger.write(str, ...)
	print(string.format(str, ...))
end

function debugger.pretty(obj, recurse)
	local function coerceable(tbl)
		local meta = getmetatable(tbl)
		return (meta and meta.__tostring)
	end

	if type(obj) == 'string' then
		return string.format('%q', obj)
	elseif type(obj) == 'table' and not coerceable(obj) and not recurse then
		local str = '{'
		for k, v in pairs(obj) do
			local pair = debugger.pretty(k, true)..' = '..debugger.pretty(v, true)
			str = str..(str == '{' and pair or ', '..pair)
		end
		return str .. '}'
	else
		local success, value = pcall(function() return tostring(obj) end)
		return (success and value or '<!!error in __tostring metamethod!!>')
	end
end

return debugger
);

int luaopen_debugger(lua_State * L)
{
	if(luaL_loadbuffer(L, debugger_lua, sizeof(debugger_lua)-1, "Debugger.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
