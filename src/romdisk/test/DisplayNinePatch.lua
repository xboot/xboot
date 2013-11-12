---
-- The 'DisplayNinePatch' class is used to display texture related objects that can
-- be placed on the screen.
--
-- @module DisplayNinePatch
local M = Class(DisplayObject)

---
-- Creates a new object of display image.
--
-- @function [parent=#DisplayNinePatch] new
-- @param texture (Texture) The texture object
-- @param x (optional) The x coordinate of the display image.
-- @param y (optional) The y coordinate of the display image.
-- @return #DisplayNinePatch
function M:init(x, y)
	self.super:init()
	self:setPosition(x or 0, y or 0)

	self.lt = DisplayImage.new(asset:loadTexture("2/lt.png"))
	self.mt = DisplayImage.new(asset:loadTexture("2/mt.png"))
	self.rt = DisplayImage.new(asset:loadTexture("2/rt.png"))
	self.lm = DisplayImage.new(asset:loadTexture("2/lm.png"))
	self.mm = DisplayImage.new(asset:loadTexture("2/mm.png"))
	self.rm = DisplayImage.new(asset:loadTexture("2/rm.png"))
	self.lb = DisplayImage.new(asset:loadTexture("2/lb.png"))
	self.mb = DisplayImage.new(asset:loadTexture("2/mb.png"))
	self.rb = DisplayImage.new(asset:loadTexture("2/rb.png"))

	self.left, self.top = self.lt:getInnerSize()
	self.right, self.bottom = self.rb:getInnerSize()

	self:addChild(self.lt)
	self:addChild(self.mt)
	self:addChild(self.rt)
	self:addChild(self.lm)
	self:addChild(self.mm)
	self:addChild(self.rm)
	self:addChild(self.lb)
	self:addChild(self.mb)
	self:addChild(self.rb)
end

function M:fitSize(width, height)
	local w = width - self.left - self.right
	local h = height - self.top - self.bottom
	
	self.lt:setPosition(0, 0)
	self.lt:fitSize(self.left, self.top)
	self.mt:setPosition(self.left, 0)
	self.mt:fitSize(w, self.top)
	self.rt:setPosition(self.left + w, 0)
	self.rt:fitSize(self.right, self.top)
	
	self.lm:setPosition(0, self.top)
	self.lm:fitSize(self.left, h)
	self.mm:setPosition(self.left, self.top)
	self.mm:fitSize(w, h)
	self.rm:setPosition(self.left + w, self.top)
	self.rm:fitSize(self.right, h)
	
	self.lb:setPosition(0, self.top + h)
	self.lb:fitSize(self.left, self.bottom)
	self.mb:setPosition(self.left, self.top + h)
	self.mb:fitSize(w, self.bottom)
	self.rb:setPosition(self.left + w, self.top + h)
	self.rb:fitSize(self.right, self.bottom)
end

return M
