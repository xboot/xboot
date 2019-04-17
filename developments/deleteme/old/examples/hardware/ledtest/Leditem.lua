local Toggle = require("toggle")
local Led = require("Led")

local M = Class(DisplayObject)

function M:init(led)
	DisplayObject.init(self)

	local lon = DisplayImage.new(Texture.new("amber-led-on.png"))
	local loff = DisplayImage.new(Texture.new("amber-led-off.png"))
	self.uiled = Led.new(lon, loff)
	self.uiled:setPosition(25, 0)

	local ton = DisplayImage.new(Texture.new("toggle-on.png"))
	local toff = DisplayImage.new(Texture.new("toggle-off.png"))
	self.uitoggle = Toggle.new(ton, toff)
	self.uitoggle:setPosition(100, 0)

	self.led = led

	self:addChild(self.uiled)
	self:addChild(self.uitoggle)
	self.uitoggle:addEventListener("toggled", self.onToggled, self)
end

function M:onToggled(e)
	if e.info.on then
		self.uiled:setState(true)
		self.led:set(0xffffff)
	else
		self.uiled:setState(false)
		self.led:set(0x000000)
	end
end

return M
