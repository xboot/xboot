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

local button = require("button")
local toggle = require("toggle")

local M = class(display_object)

function M:init()
	display_object.init(self)

	self:add_child(display_image:new("/romdisk/samples/images/background.png"))

-- button
	local normal = display_image:new("/romdisk/samples/images/off.png")
	local active = display_image:new("/romdisk/samples/images/on.png")
	local btn = button:new(normal, active)
	btn:add_event_listener("click", function(d, e)
		print("power click...")
	end, btn)
	btn:setxy(200, 150)
	self:add_child(btn)
	
-- toggle
	local on = display_image:new("/romdisk/samples/images/music-ui-elements-on.png")
	local off = display_image:new("/romdisk/samples/images/music-ui-elements-off.png")
	local tog = toggle:new(on, off)
	tog:add_event_listener("toggled", function(d, e)
		print("toggle click...")
	end, toggle)
	tog:setxy(350, 300)
	self:add_child(tog)
	
	self:add_event_listener("enterBegin", self.on_transition_in_begin, self)
	self:add_event_listener("enterEnd", self.on_transition_in_end, self)
	self:add_event_listener("exitBegin", self.on_transition_out_begin, self)
	self:add_event_listener("exitEnd", self.on_transition_out_end, self)
end

function M:on_transition_in_begin()
	print("scene2 - enter begin")
end

function M:on_transition_in_end()
	print("scene2 - enter end")
end

function M:on_transition_out_begin()
	print("scene2 - exit begin")
end

function M:on_transition_out_end()
	print("scene2 - exit end")
end

return M
