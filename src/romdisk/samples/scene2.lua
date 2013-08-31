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
