local button = require("button")
local toggle = require("toggle")

local M = Class(DisplayObject)

function M:init()
	DisplayObject.init(self)

	self:addChild(DisplayImage:new("/romdisk/samples/images/background.png"))

-- button
	local normal = DisplayImage:new("/romdisk/samples/images/off.png")
	local active = DisplayImage:new("/romdisk/samples/images/on.png")
	local btn = button:new(normal, active)
	btn:addEventListener("click", function(d, e)
		print("power click...")
	end, btn)
	btn:setXY(200, 150)
	self:addChild(btn)
	
-- toggle
	local on = DisplayImage:new("/romdisk/samples/images/music-ui-elements-on.png")
	local off = DisplayImage:new("/romdisk/samples/images/music-ui-elements-off.png")
	local tog = toggle:new(on, off)
	tog:addEventListener("toggled", function(d, e)
		print("toggle click...")
	end, toggle)
	tog:setXY(350, 300)
	self:addChild(tog)
	
	self:addEventListener("enterBegin", self.on_transition_in_begin, self)
	self:addEventListener("enterEnd", self.on_transition_in_end, self)
	self:addEventListener("exitBegin", self.on_transition_out_begin, self)
	self:addEventListener("exitEnd", self.on_transition_out_end, self)
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
