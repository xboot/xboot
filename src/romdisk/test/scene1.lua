local buildin_logger = require("org.xboot.buildin.logger")
local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")
local buildin_cairo = require("org.xboot.buildin.cairo")

local class = require("org.xboot.lang.class")
local timer = require("org.xboot.timer.timer")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local display_image = require("org.xboot.display.display_image")

local M = class(display_object)

function M:init()
	display_object.init(self)

	self:add_child(display_image:new("/romdisk/test/images/scene1.png"))
	
	self:add_event_listener("enterBegin", self.on_transition_in_begin, self)
	self:add_event_listener("enterEnd", self.on_transition_in_end, self)
	self:add_event_listener("exitBegin", self.on_transition_out_begin, self)
	self:add_event_listener("exitEnd", self.on_transition_out_end, self)
end

function M:on_transition_in_begin()
	print("scene1 - enter begin")
end

function M:on_transition_in_end()
	print("scene1 - enter end")
end

function M:on_transition_out_begin()
	print("scene1 - exit begin")
end

function M:on_transition_out_end()
	print("scene1 - exit end")
end

return M
