---
-- All classes that dispatch events inherit from 'EventDispatcher'. The target of
-- an event is a listener function and an optional data value.
-- 
-- @module EventDispatcher
local M = class()

---
-- Creates a new 'EventDispatcher' object.
-- 
-- @function [parent=#EventDispatcher] new
-- @return New 'EventDispatcher' object.
function M:init()
	self.eventListenersMap = {}
end

---
-- Checks if the 'EventDispatcher' object has a event listener registered for the specified type of event.
-- 
-- @function [parent=#EventDispatcher] hasEventListener
-- @param self
-- @param type (string) The type of event.
-- @param listener (listener) The listener function that processes the event.
-- @param data (data) An optional data parameter that is passed to the listener function.
-- @return A value of 'true' if a listener of the specified type is registered; 'false' otherwise.
function M:hasEventListener(type, listener, data)
	local els = self.eventListenersMap[type]

	if not els or #els == 0 then
		return false
	end

	for i, v in ipairs(els) do
		if v.listener == listener and v.data == data then
			return true
		end
	end

	return false
end

--- 
-- Registers a listener function and an optional data value so that the listener function is called when an event
-- of a particular type occurs.
-- 
-- @function [parent=#EventDispatcher] addEventListener
-- @param self
-- @param type (string) The type of event.
-- @param listener (function) The listener function that processes the event.
-- @param data (optional) An optional data parameter that is passed as a first argument to the listener function.
-- @return A value of 'true' or 'false'.
function M:addEventListener(type, listener, data)
	local data = data or self

	if self:hasEventListener(type, listener, data) then
		return self
	end

	if not self.eventListenersMap[type] then
		self.eventListenersMap[type] = {}
	end

	local els = self.eventListenersMap[type]
	local el = {type = type, listener = listener, data = data}
	table.insert(els, el)

	return self
end

---
-- Removes a listener from the 'EventDispatcher' object. 'removeEventListener()' function expects
-- the same arguments with 'addEventListener()' to remove the event. If there is no matching listener
-- registered, a call to this function has no effect.
-- 
-- @function [parent=#EventDispatcher] removeEventListener
-- @param self
-- @param type (string) The type of event.
-- @param listener (function) The listener object to remove.
-- @param data The data parameter that is used while registering the event.
-- @return A value of 'true' or 'false'.
function M:removeEventListener(type, listener, data)
	local data = data or self
	local els = self.eventListenersMap[type]

	if not els or #els == 0 then
		return self
	end

	for i, v in ipairs(els) do
		if v.type == type and v.listener == listener and v.data == data then
			table.remove(els, i)
			break
		end
	end

	return self
end

---
-- Dispatches an event to this 'EventDispatcher' instance.
-- 
-- @function [parent=#EventDispatcher] dispatchEvent
-- @param self
-- @param event (event) The 'event' object to be dispatched.
function M:dispatchEvent(event)
	if event.stoped == true then
		return self
	end

	local els = self.eventListenersMap[event.type]

	if not els or #els == 0 then
		return self
	end

	for i, v in ipairs(els) do
		if v.type == event.type then
			v.listener(v.data, event)
		end
	end

	return self
end

return M
