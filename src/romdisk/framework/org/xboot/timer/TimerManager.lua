---
-- The 'TimerManager' class is used to manager timer.
-- 
-- @module TimerManager
local M = class()

---
-- Creates a new 'TimerManager' object.
-- 
-- @function [parent=#TimerManager] new
-- @return New 'TimerManager' object.
function M:init()
	self.timerList = {}
end

---
-- Checks if the 'TimerManager' object has a timer.
-- 
-- @function [parent=#TimerManager] hasTimer
-- @param self
-- @param timer (Timer) The timer was registered.
-- @return A value of 'true' if a timer is registered; 'false' otherwise.
function M:hasTimer(timer)
	local tl = self.timerList

	if not tl or #tl == 0 then
		return false
	end

	if not timer then
		return false
	end

	for i, v in ipairs(tl) do
		if v == timer then
			return true
		end
	end

	return false
end

--- 
-- Registers a timer
-- 
-- @function [parent=#TimerManager] addTimer
-- @param self
-- @param timer (Timer) The timer will be registered.
-- @return A value of 'true' or 'false'.
function M:addTimer(timer)
	if self:hasTimer(timer) then
		return false
	end

	table.insert(self.timerList, timer)
	return true
end

---
-- Removes a timer from the 'TimerManager'.
-- 
-- @function [parent=#TimerManager] removeTimer
-- @param self
-- @param timer (Timer) The timer will be removed.
-- @return A value of 'true' or 'false'.
function M:removeTimer(timer)
	local tl = self.timerList

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

---
-- Schedule timers according to time interval.
-- 
-- @function [parent=#TimerManager] schedule
-- @param self
-- @param dt (number) The time delta in seconds.
function M:schedule(dt)
	for i, v in ipairs(self.timerList) do
		if v.running then
			v.__time = v.__time + dt

			if v.__time >= v.delay then
				v.__count = v.__count + 1
				v.listener(v, {time = v.__time, count = v.__count})

				v.__time = 0
				if v.iteration ~= 0 and v.__count >= v.iteration then
					self:removeTimer(v)
				end
			end
		end
	end
end

return M
