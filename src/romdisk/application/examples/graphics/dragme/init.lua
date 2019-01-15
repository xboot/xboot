local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init()

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("graphics/dragme/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	for i = 1, 5 do
		local shape = DisplayShape.new(100, 50)
			:setLineWidth(6)
			:rectangle(0, 0, 100, 50)
			:setSourceColor(1, 0, 0, 0.5)
			:fillPreserve()
			:setSourceColor(0, 0, 0)
			:stroke()
			:setPosition(math.random(0, w - 100), math.random(0, h - 50))
	
		shape:addEventListener(Event.MOUSE_DOWN, self.onMouseDown)
		shape:addEventListener(Event.MOUSE_MOVE, self.onMouseMove)
		shape:addEventListener(Event.MOUSE_UP, self.onMouseUp)
		shape:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin)
		shape:addEventListener(Event.TOUCH_MOVE, self.onTouchMove)
		shape:addEventListener(Event.TOUCH_END, self.onTouchEnd)
	
		self:addChild(shape)
	end
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.touchid == -1 then	
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.touchid == -1 then
		self.touchid = nil
		e.stop = true
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.touchid == e.id then
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

return M
