local Button = require "widget.Button"
local M = Class(Button)

function M:init(up, down)
	local iup = DisplayImage.new(Texture.new(assert(up)))
	local idn = DisplayImage.new(Texture.new(assert(down)))
	self.super:init(iup, idn)
end

return M
