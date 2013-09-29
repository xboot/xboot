local Led = require "widget.Led"
local M = Class(Led)

function M:init(on, off, state)
	local ion = DisplayImage.new(Texture.new(assert(on)))
	local ioff = DisplayImage.new(Texture.new(assert(off)))
	self.super:init(ion, ioff, state)
end

return M
