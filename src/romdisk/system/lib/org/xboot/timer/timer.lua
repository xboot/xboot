---
-- The 'timer' class is used to execute a code at specified intervals.
-- 
-- @module timer
local M = class()
local timer_list = {}

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
	self.delay = delay or 1
	self.iteration = iteration or 1
	self.listener = listener
	self.data = data or nil

	self.time = 0
	self.count = 0
	self.running = true

	table.insert(timer_list, self)
end

---
-- Returns the current running status of timer.
-- 
-- @function [parent=#timer] isrunning
-- @param self
-- @return 'true' if the timer is running, 'false' otherwise.
function M:isrunning()
	return self.running
end

---
-- Resumes the timer that was paused.
--
-- @function [parent=#timer] resume
-- @param self
function M:resume()
	self.running = true
end

---
-- Pauses the timer that was running.
--
-- @function [parent=#timer] pause
-- @param self
function M:pause()
	self.running = false
end

---
-- Cancels the timer operation initiated with timer:new().
-- 
-- @function [parent=#timer] cancel
-- @param self
-- @return the number of iterations.
function M:cancel()
	for i, v in ipairs(timer_list) do
		if v.delay == self.delay and v.iteration == self.iteration and v.listener == self.listener and v.data == self.data then
			v:pause()
			table.remove(timer_list, i)
			return v.count
		end
	end
end

---
-- Schedule all timers according to time interval.
-- 
-- @function [parent=#timer] schedule
-- @param self
-- @param dt (number) The time delta in seconds.
function M:schedule(dt)
	for i, v in ipairs(timer_list) do
		if v.running then
			v.time = v.time + dt

			if v.time > v.delay then
				v.count = v.count + 1
				v.listener(v, {time = v.time, count = v.count, data = v.data})

				v.time = 0
				if v.iteration ~= 0 and v.count >= v.iteration then
					v:cancel()
				end
			end
		end
	end
end

return M
