/*
 * framework/core/l-stage.c
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

#include <framework/core/l-stage.h>

static const char stage_lua[] = X(
local M = Class(DisplayObject)

function M:init()
	self.exiting = false
	self.timerlist = {}
	self.display = Display.new()
	self.super:init(self.display:getSize())
end

function M:exit()
	self.exiting = true
	return self
end

function M:hasTimer(timer)
	local tl = self.timerlist

	if not tl or #tl == 0 then
		return false
	end

	for i, v in ipairs(tl) do
		if v == timer then
			return true
		end
	end

	return false
end

function M:addTimer(timer)
	if self:hasTimer(timer) then
		return false
	end

	timer:start()
	table.insert(self.timerlist, timer)
	return true
end

function M:removeTimer(timer)
	local tl = self.timerlist

	if not tl or #tl == 0 then
		return false
	end

	for i, v in ipairs(tl) do
		if v == timer then
			v:pause()
			table.remove(tl, i)
			return true
		end
	end

	return false
end

function M:schedTimer(dt)
	for i, v in ipairs(self.timerlist) do
		if v.running then
			v.runtime = v.runtime + dt

			if v.runtime >= v.delay then
				v.runcount = v.runcount + 1
				v.listener(v)
				v.runtime = 0

				if v.iteration ~= 0 and v.runcount >= v.iteration then
					self:removeTimer(v)
				end
			end
		end
	end
end

function M:getSize()
	return self.display:getSize()
end

function M:getPhysicalSize()
	return self.display:getPhysicalSize()
end

function M:getBpp()
	return self.display:getBpp()
end

function M:setBacklight(brightness)
	return self.display:setBacklight(brightness)
end

function M:getBacklight()
	return self.display:getBacklight()
end

function M:showobj(value)
	self.display:showobj(value)
	return self
end

function M:showfps(value)
	self.display:showfps(value)
	return self
end

function M:snapshot()
	return self.display:snapshot()
end

function M:loop()
	local Event = Event
	local display = self.display
	local stopwatch = Stopwatch.new()

	self:addTimer(Timer.new(1 / 60, 0, function(t)
		self:render(Event.new(Event.ENTER_FRAME))
		display:present()
		collectgarbage("step")
	end))

	self:addEventListener(Event.KEY_DOWN, function(d, e)
		if e.key == 10 then self:exit() end
	end)

	while not self.exiting do
		local e = Event.pump()
		if e ~= nil then
			self:dispatch(e)
		end

		local elapsed = stopwatch:elapsed()
		if elapsed > 0 then
			stopwatch:reset()
			self:schedTimer(elapsed)
		end
	end
end

return M
);

int luaopen_stage(lua_State * L)
{
	if(luaL_loadbuffer(L, stage_lua, sizeof(stage_lua) - 1, "Stage.lua") == LUA_OK)
		lua_call(L, 0, 1);
	return 1;
}
