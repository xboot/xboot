---
-- All classes that dispatch events inherit from 'event_dispatcher'. The target of
-- an event is a listener function and an optional data value.
-- 
-- @module event_dispatcher
local M = class()

---
-- Creates a new 'event_dispatcher' object.
-- 
-- @function [parent=#event_dispatcher] new
-- @return New 'event_dispatcher' object.
function M:init()
	self.event_listeners_map = {}
end

---
-- Checks if the 'event_dispatcher' object has a event listener registered for the specified type of event.
-- 
-- @function [parent=#event_dispatcher] has_event_listener
-- @param self
-- @param type (string) The type of event.
-- @param listener (optional) The listener function that processes the event.
-- @param data (optional) An optional data parameter that is passed to the listener function.
-- @return A value of 'true' if a listener of the specified type is registered; 'false' otherwise.
function M:has_event_listener(type, listener, data)
	local els = self.event_listeners_map[type]

	if not els or #els == 0 then
		return false
	end

	if listener == nil and data == nil then
		return true
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
-- @function [parent=#event_dispatcher] add_event_listener
-- @param self
-- @param type (string) The type of event.
-- @param listener (function) The listener function that processes the event.
-- @param data (optional) An optional data parameter that is passed as a first argument to the listener function.
-- @return A value of 'true' or 'false'.
function M:add_event_listener(type, listener, data)
	if self:has_event_listener(type, listener, data) then
		return false
	end

	if not self.event_listeners_map[type] then
		self.event_listeners_map[type] = {}
	end

	local els = self.event_listeners_map[type]
	local el = {type = type, listener = listener, data = data}
	table.insert(els, el)

	return true
end

---
-- Removes a listener from the 'event_dispatcher' object. 'remove_event_listener()' function expects
-- the same arguments with 'add_event_listener()' to remove the event. If there is no matching listener
-- registered, a call to this function has no effect.
-- 
-- @function [parent=#event_dispatcher] remove_event_listener
-- @param self
-- @param type (string) The type of event.
-- @param listener (function) The listener object to remove.
-- @param data The data parameter that is used while registering the event.
-- @return A value of 'true' or 'false'.
function M:remove_event_listener(type, listener, data)
	local els = self.event_listeners_map[type]

	if not els or #els == 0 then
		return false
	end

	for i, v in ipairs(els) do
		if v.type == type and v.listener == listener and v.data == data then
			table.remove(els, i)
			return true
		end
	end

	return false
end

---
-- Dispatches an event to this 'event_dispatcher' instance.
-- 
-- @function [parent=#event_dispatcher] dispatch_event
-- @param self
-- @param event (event) The 'event' object to be dispatched.
function M:dispatch_event(event)
	if event.stoped == true then
		return
	end

	local els = self.event_listeners_map[event.type]

	if not els or #els == 0 then
		return
	end

	for i, v in ipairs(els) do
		if v.type == event.type then
			v.listener(v.data, event)
		end
	end
end

return M
