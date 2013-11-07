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

	self.tl = DisplayImage.new(asset:loadTexture("1/tl.png"))
	self.tm = DisplayImage.new(asset:loadTexture("1/tm.png"))
	self.tr = DisplayImage.new(asset:loadTexture("1/tr.png"))
	
	self.ml = DisplayImage.new(asset:loadTexture("1/ml.png"))
	self.mm = DisplayImage.new(asset:loadTexture("1/mm.png"))
	self.mr = DisplayImage.new(asset:loadTexture("1/mr.png"))
	
	self.bl = DisplayImage.new(asset:loadTexture("1/bl.png"))
	self.bm = DisplayImage.new(asset:loadTexture("1/bm.png"))
	self.br = DisplayImage.new(asset:loadTexture("1/br.png"))
	
	local w, h = self.tl:getInnerSize()
	self.lw = w
	local w, h = self.tm:getInnerSize()
	self.mw = w
	local w, h = self.tr:getInnerSize()
	self.rw = w
	local w, h = self.tl:getInnerSize()
	self.th = h
	local w, h = self.ml:getInnerSize()
	self.mh = h
	local w, h = self.bl:getInnerSize()
	self.bh = h

	self:addChild(self.tl)
	self:addChild(self.tm)
	self:addChild(self.tr)
	
	self:addChild(self.ml)
	self:addChild(self.mm)
	self:addChild(self.mr)
	
	self:addChild(self.bl)
	self:addChild(self.bm)
	self:addChild(self.br)
	
	self:setPosition(x or 0, y or 0)
end

function M:setSize(width, height)
	
end

return M
