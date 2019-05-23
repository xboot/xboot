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
	self._exiting = false
	self._timerlist = {}
	self._window = Window.new()
	self.super:init(self._window:getSize())
	self:markDirty()
end

function M:exit()
	self._exiting = true
	return self
end

function M:hasTimer(timer)
	local tl = self._timerlist

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
	table.insert(self._timerlist, timer)
	return true
end

function M:removeTimer(timer)
	local tl = self._timerlist

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
	for i, v in ipairs(self._timerlist) do
		if v._running then
			v._runtime = v._runtime + dt

			if v._runtime >= v._delay then
				v._runcount = v._runcount + 1
				v._listener(v)
				v._runtime = 0

				if v._iteration ~= 0 and v._runcount >= v._iteration then
					self:removeTimer(v)
				end
			end
		end
	end
end

function M:getDotsPerInch()
	local w, h = self._window:getSize()
	local pw, ph = self._window:getPhysicalSize()
	return w * 25.4 / pw, h * 25.4 / ph
end

function M:getBytesPerPixel()
	return self._window:getBytesPerPixel()
end

function M:setBacklight(brightness)
	return self._window:setBacklight(brightness)
end

function M:getBacklight()
	return self._window:getBacklight()
end

function M:showobj(show)
	self._window:showobj(show)
	return self
end

function M:snapshot()
	return self._window:snapshot()
end

function M:loop()
	local Event = Event
	local window = self._window
	local stopwatch = Stopwatch.new()

	self:addTimer(Timer.new(1 / 60, 0, function(t)
		self:dispatch(Event.new("enter-frame"))
		self:render(window)
		collectgarbage("step")
	end))

	self:addEventListener("key-down", function(d, e)
		if e.key == 10 then self:exit() end
	end)

	while not self._exiting do
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
