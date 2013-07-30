local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local display_object = require("org.xboot.display.display_object")

local M = class(display_object)

function M:init(on, off)
	display_object.init(self)

	self.on = on
	self.off = off
	self.ison = false
	self.focus = false

	self:add_child(self.on)
	self:add_child(self.off)
	self:update_visual_state(self.ison)

	self:add_event_listener(event.MOUSE_DOWN, self.on_mouse_down, self)
	self:add_event_listener(event.MOUSE_MOVE, self.on_mouse_move, self)
	self:add_event_listener(event.MOUSE_UP, self.on_mouse_up, self)

	self:add_event_listener(event.TOUCHES_BEGIN, self.on_touches_begin, self)
	self:add_event_listener(event.TOUCHES_MOVE, self.on_touches_move, self)
	self:add_event_listener(event.TOUCHES_END, self.on_touches_end, self)
	self:add_event_listener(event.TOUCHES_CANCEL, self.on_touches_cancel, self)
end

function M:on_mouse_down(e)
	if self:hit_test_point(e.msg.x, e.msg.y) then
		self.focus = true
		e:stop_propagation()
	end
end

function M:on_mouse_move(e)
	if self.focus then
		if not self:hit_test_point(e.msg.x, e.msg.y) then	
			self.focus = false
		end
		e:stop_propagation()
	end
end

function M:on_mouse_up(e)
	if self.focus then
		self.focus = false
		self.ison = not self.ison
		self:update_visual_state(self.ison)
		self:dispatch_event(event:new("toggled"))
		e:stop_propagation()
	end
end

function M:on_touches_begin(e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_move(e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_end(e)
	if self.focus then
		e:stop_propagation()
	end
end

function M:on_touches_cancel(e)
	if self.focus then
		self.focus = false;
		e:stop_propagation()
	end
end

function M:update_visual_state(state)
	if state then
		self.on:visible(true)
		self.off:visible(false)
		self.width = self.off.width
		self.height = self.off.height
	else
		self.on:visible(false)
		self.off:visible(true)
		self.width = self.on.width
		self.height = self.on.height
	end
end

return M
