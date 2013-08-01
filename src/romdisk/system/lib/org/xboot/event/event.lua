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
-- @field [parent=#event] TOUCHES_BEGIN touchesbegan
M.TOUCHES_BEGIN = "touchesbegin"

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
-- @field [parent=#event] ENTER_FRAME enterframe
M.ENTER_FRAME = "enterframe"

---
-- Creates a new 'event' object to be dispatched from an 'event_dispatcher'.
-- 
-- @function [parent=#event] new
-- @param type (string)
-- @param info (optional) An optional table of information attach to event object.
-- @return New 'event' object.
function M:init(type, info)
	self.type = type
	self.info = info or {}
	self.stoped = false
end

---
-- Disables the propagation of the current event.
-- 
-- @function [parent=#event] stop_propagation
-- @param self
function M:stop_propagation()
	self.stoped = true
end

return M
