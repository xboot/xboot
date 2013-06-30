local class = require("org.xboot.lang.class")

---
-- The objects of 'event' class contains information about an event that has occurred. 'event' objects
-- are passed to event listeners when an event occurs.
-- 
-- @module event
local M = class()

---
-- @field [parent=#event] KEY_DOWN keydown
M.KEY_DOWN = "keydown"

---
-- @field [parent=#event] KEY_UP keyup
M.KEY_UP = "keyup"

---
-- @field [parent=#event] MOUSE_DOWN mousedown
M.MOUSE_DOWN = "mousedown"

---
-- @field [parent=#event] MOUSE_MOVE mousemove
M.MOUSE_MOVE = "mousemove"

---
-- @field [parent=#event] MOUSE_UP mouseup
M.MOUSE_UP = "mouseup"

---
-- @field [parent=#event] MOUSE_WHEEL mousewheel
M.MOUSE_WHEEL = "mousewheel"

---
-- @field [parent=#event] TOUCHES_BEGAN touchesbegan
M.TOUCHES_BEGAN = "touchesbegan"

---
-- @field [parent=#event] TOUCHES_MOVE touchesmove
M.TOUCHES_MOVE = "touchesmove"

---
-- @field [parent=#event] TOUCHES_END touchesend
M.TOUCHES_END = "touchesend"

---
-- @field [parent=#event] TOUCHES_CANCEL touchescancel
M.TOUCHES_CANCEL = "touchescancel"

---
-- Creates a new 'event' object to be dispatched from an 'event_dispatcher'.
-- 
-- @function [parent=#event] new
-- @param type (string)
-- @return New 'event' object.
function M:init(type)
	self.__type = type
	self.__target = nil
	self.__stoped = false
end

---
-- Returns a string that represents the type of the event.
-- 
-- @function [parent=#event] get_type
-- @param self
-- @return Type of event.
function M:get_type()
  return self.__type
end

---
-- Returns the element on which the event listener was registered.
-- 
-- @function [parent=#event] get_target
-- @param self
-- @return Target of event.
function M:get_target()
  return self.__target
end

---
-- Disables the propagation of the current event.
-- 
-- @function [parent=#event] stop_propagation
-- @param self
function M:stop_propagation()
	self.__stoped = true
end

return M
