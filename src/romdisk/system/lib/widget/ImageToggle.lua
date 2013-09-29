local Toggle = require "widget.Toggle"
local M = Class(Toggle)

function M:init(on, off, state)
	local ion = DisplayImage.new(Texture.new(assert(on)))
	local ioff = DisplayImage.new(Texture.new(assert(off)))
	self.super:init(ion, ioff, state)
end

return M
