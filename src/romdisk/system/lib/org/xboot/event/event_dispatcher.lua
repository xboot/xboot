local class = require("org.xboot.lang.class")

---
-- The objects of xxxx
-- 
-- @module event_dispatcher
local M = class()

function M:init()
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
function M:add_event_listener(type, listener, data)
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
function M:remove_event_listener(type, listener, data)
end

---
-- Dispatches an event to this 'event_dispatcher' instance.
-- 
-- @function [parent=#event_dispatcher] dispatch_event
-- @param self
-- @param event (event) The 'event' object to be dispatched.
function M:dispatch_event(event)
end

---
-- Checks if the 'event_dispatcher' object has a event listener registered for the specified type of event.
-- 
-- @function [parent=#event_dispatcher] has_event_listener
-- @param self
-- @param type (string) The type of event.
-- @return A value of 'true' if a listener of the specified type is registered; 'false' otherwise.
function M:has_event_listener(type)
	return false
end

return M
