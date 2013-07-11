local class = require("org.xboot.lang.class")

local M = class()

local __timer_list = {}

function M:init(delay, listener, iteration, data)
	assert(delay)
	assert(listener)
	
	self.__delay = delay
	self.__listener = listener
	self.__iteration = iteration or 0
	self.__data = data or nil

	self.__time = 0
	self.__count = 0
	self.__running = false
	
	table.insert(__timer_list, self)
end

function M:start()
	self.__running = true
end

function M:pause()
	self.__running = false
end

function M:cancel()
	for i, v in ipairs(__timer_list) do
		if v.__delay == self.__delay and v.__listener == self.__listener and v.__iteration == self.__iteration and v.__data == self.__data then
			table.remove(__timer_list, i)
			break
		end
	end
end

function M:schedule(delta)
	for i, v in ipairs(__timer_list) do
		if v.__running then
			v.__time = v.__time + delta
			
			if v.__time > v.__delay then
				v.__count = v.__count + 1
				v.__listener({time = v.__time, count = v.__count}, v.__data)
						
				if v.__iteration ~= 0 and not v.__count < v.__iteration then
					v.__running = false
				end
				v.__time = 0
			end
		end
	end
end

return M
