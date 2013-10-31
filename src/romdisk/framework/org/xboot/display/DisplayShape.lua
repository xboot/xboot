---
-- The 'DisplayShape' class is used to display shape related objects that can
-- be placed on the screen.
--
-- @module DisplayShape
local M = Class(DisplayObject)

---
-- Creates a new object of display shape.
--
-- @function [parent=#DisplayShape] new
-- @param width (number) The width of drawing area in pixels.
-- @param height (number) The height of drawing area in pixels.
-- @param x (optional) The x coordinate of the display shape.
-- @param y (optional) The y coordinate of the display shape.
-- @return #DisplayShape
function M:init(width, height, x, y)
	self.super:init()

	self:setShape(Shape.new(width, height))
	self:setPosition(x or 0, y or 0)
end

---
-- Attach shape to display shape.
--
-- @function [parent=#DisplayShape] setShape
-- @param self
-- @param shape (Shape) The shape object
function M:setShape(shape)
	if shape then
		local w, h = shape:size()
		self.shape = shape
		self:setInnerSize(w, h)
	end
end

---
-- Get shape of display shape.
--
-- @function [parent=#DisplayShape] getShape
-- @param self
-- @return The shape object of display shape.
function M:getShape()
	return self.shape
end

function M:save()
	self.shape:save()
end

function M:restore()
	self.shape:restore()
end

function M:pushGroup()
	self.shape:pushGroup()
end

function M:popGroup()
	self.shape:popGroup()
end

function M:popGroupToSource()
	self.shape:popGroupToSource()
end

function M:newPath()
	self.shape:newPath()
end

function M:newSubPath()
	self.shape:newSubPath()
end

function M:closePath()
	self.shape:closePath()
end

function M:setOperator(op)
	self.shape:setOperator(op)
end

function M:setSource(pattern)
	self.shape:setSource(pattern)
end

function M:setSourceColor(r, g, b, a)
	self.shape:setSourceColor(r, g, b, a)
end

function M:setTolerance(tolerance)
	self.shape:setTolerance(tolerance)
end

function M:setMiterLimit(limit)
	self.shape:setMiterLimit(limit)
end

function M:setAntialias(antialias)
	self.shape:setAntialias(antialias)
end

function M:setFillRule(rule)
	self.shape:setFillRule(rule)
end

function M:setLineWidth(width)
	self.shape:setLineWidth(width)
end

function M:setLineCap(cap)
	self.shape:setLineCap(cap)
end

function M:setLineJoin(join)
	self.shape:setLineJoin(join)
end

function M:setDash(dashes, offset)
	self.shape:setDash(dashes, offset)
end

function M:moveTo(x, y)
	self.shape:moveTo(x, y)
end

function M:relMoveTo(dx, dy)
	self.shape:relMoveTo(dx, dy)
end

function M:lineTo(x, y)
	self.shape:lineTo(x, y)
end

function M:relLineTo(dx, dy)
	self.shape:relLineTo(dx, dy)
end

function M:curveTo(x1, y1, x2, y2, x3, y3)
	self.shape:curveTo(x1, y1, x2, y2, x3, y3)
end

function M:relCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
	self.shape:relCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
end

function M:rectangle(x, y, width, height)
	self.shape:rectangle(x, y, width, height)
end

function M:arc(xc, yc, radius, angle1, angle2)
	self.shape:arc(xc, yc, radius, angle1, angle2)
end

function M:arcNegative(xc, yc, radius, angle1, angle2)
	self.shape:arcNegative(xc, yc, radius, angle1, angle2)
end

function M:stroke()
	self.shape:stroke()
end

function M:strokePreserve()
	self.shape:strokePreserve()
end

function M:fill()
	self.shape:fill()
end

function M:fillPreserve()
	self.shape:fillPreserve()
end

function M:clip()
	self.shape:clip()
end

function M:clipPreserve()
	self.shape:clipPreserve()
end

function M:paint(alpha)
	self.shape:paint(alpha)
end

---
-- Draw display shape to the screen. (subclasses method)
--
-- @function [parent=#DisplayShape] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	display:drawShape(self.shape, self:getTransformMatrix(), self:getAlpha())
end

return M
