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
-- @return #DisplayShape
function M:init(width, height)
	self.super:init()
	self:setShape(Shape.new(width, height))
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
		self:setSize(w, h)
	end
	return self
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
	return self
end

function M:restore()
	self.shape:restore()
	return self
end

function M:pushGroup()
	self.shape:pushGroup()
	return self
end

function M:popGroup()
	self.shape:popGroup()
	return self
end

function M:popGroupToSource()
	self.shape:popGroupToSource()
	return self
end

function M:newPath()
	self.shape:newPath()
	return self
end

function M:newSubPath()
	self.shape:newSubPath()
	return self
end

function M:closePath()
	self.shape:closePath()
	return self
end

function M:setOperator(op)
	self.shape:setOperator(op)
	return self
end

function M:setSource(pattern)
	self.shape:setSource(pattern)
	return self
end

function M:setSourceColor(r, g, b, a)
	self.shape:setSourceColor(r, g, b, a)
	return self
end

function M:setTolerance(tolerance)
	self.shape:setTolerance(tolerance)
	return self
end

function M:setMiterLimit(limit)
	self.shape:setMiterLimit(limit)
	return self
end

function M:setAntialias(antialias)
	self.shape:setAntialias(antialias)
	return self
end

function M:setFillRule(rule)
	self.shape:setFillRule(rule)
	return self
end

function M:setLineWidth(width)
	self.shape:setLineWidth(width)
	return self
end

function M:setLineCap(cap)
	self.shape:setLineCap(cap)
	return self
end

function M:setLineJoin(join)
	self.shape:setLineJoin(join)
	return self
end

function M:setDash(dashes, offset)
	self.shape:setDash(dashes, offset)
	return self
end

function M:moveTo(x, y)
	self.shape:moveTo(x, y)
	return self
end

function M:relMoveTo(dx, dy)
	self.shape:relMoveTo(dx, dy)
	return self
end

function M:lineTo(x, y)
	self.shape:lineTo(x, y)
	return self
end

function M:relLineTo(dx, dy)
	self.shape:relLineTo(dx, dy)
	return self
end

function M:curveTo(x1, y1, x2, y2, x3, y3)
	self.shape:curveTo(x1, y1, x2, y2, x3, y3)
	return self
end

function M:relCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
	self.shape:relCurveTo(dx1, dy1, dx2, dy2, dx3, dy3)
	return self
end

function M:rectangle(x, y, width, height)
	self.shape:rectangle(x, y, width, height)
	return self
end

function M:arc(xc, yc, radius, angle1, angle2)
	self.shape:arc(xc, yc, radius, angle1, angle2)
	return self
end

function M:arcNegative(xc, yc, radius, angle1, angle2)
	self.shape:arcNegative(xc, yc, radius, angle1, angle2)
	return self
end

function M:stroke()
	self.shape:stroke()
	return self
end

function M:strokePreserve()
	self.shape:strokePreserve()
	return self
end

function M:fill()
	self.shape:fill()
	return self
end

function M:fillPreserve()
	self.shape:fillPreserve()
	return self
end

function M:clip()
	self.shape:clip()
	return self
end

function M:clipPreserve()
	self.shape:clipPreserve()
	return self
end

function M:paint(alpha)
	self.shape:paint(alpha)
	return self
end

---
-- Draw display shape to the screen. (subclasses method)
--
-- @function [parent=#DisplayShape] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
  self:updateTransformMatrix()
  display:drawShape(self.object, self.shape)
end

return M
