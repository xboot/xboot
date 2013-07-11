local class = require("org.xboot.lang.class")

---
-- The 'timer' class is used to execute a code at specified intervals.
-- 
-- @module timer
local M = class()
local __timer_list = {}

---
-- Creates a new 'timer' object with the specified delay and iteration.
-- 
-- @function [parent=#timer] new
-- @param delay (number) The delay in seconds
-- @param iteration (number) The number of times listener is to be invoked. pass 0 if you want it to loop forever.
-- @param listener (function) The listener to invoke after the delay.
-- @param data (optional) An optional data parameter that is passed to the listener function.
-- @return New 'timer' object.
function M:init(delay, iteration, listener, data)
	self.__delay = delay or 1
	self.__iteration = iteration or 1
	self.__listener = listener
	self.__data = data or nil

	self.__time = 0
	self.__count = 0
	self.__running = true
	
	table.insert(__timer_list, self)
end

---
-- Returns the current running status of timer.
-- 
-- @function [parent=#timer] isrunning
-- @param self
-- @return 'true' if the timer is running, 'false' otherwise.
function M:isrunning()
	return self.__running
end

---
-- Resumes the timer that was paused.
--
-- @function [parent=#timer] resume
-- @param self
function M:resume()
	self.__running = true
end

---
-- Pauses the timer that was resumed.
--
-- @function [parent=#timer] pause
-- @param self
function M:pause()
	self.__running = false
end

---
-- Cancels the timer operation initiated with timer:new().
-- 
-- @function [parent=#timer] cancel
-- @param self
-- @return the number of iterations.
function M:cancel()
	for i, v in ipairs(__timer_list) do
		if v.__delay == self.__delay and v.__iteration == self.__iteration and v.__listener == self.__listener and v.__data == self.__data then
			v:pause()
			table.remove(__timer_list, i)
			return v.__count
		end
	end
end

---
-- Schedule all timers in list.
-- 
-- @function [parent=#timer] schedule
-- @param self
-- @param delta (number) The time interval in seconds.
function M:schedule(delta)
	for i, v in ipairs(__timer_list) do
		if v.__running then
			v.__time = v.__time + delta
			
			if v.__time > v.__delay then
				v.__count = v.__count + 1
				v.__listener(v, {time = v.__time, count = v.__count, data = v.__data})

				v.__time = 0
				if v.__iteration ~= 0 and v.__count >= v.__iteration then
					v:cancel()
				end
			end
		end
	end
end

return M
