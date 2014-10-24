local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local width, height = application:getScreenSize()
	local assets = application:getAssets()
	self:addChild(assets:loadDisplay("scene1.png"))

	for i=1,5 do
		local shape = DisplayShape.new(100, 50)
			:setLineWidth(6)
			:rectangle(0, 0, 100, 50)
			:setSourceColor(1, 0, 0, 0.5)
			:fillPreserve()
			:setSourceColor(0, 0, 0)
			:stroke()
			:setPosition(math.random(0, width - 100), math.random(0, height - 50))
	
		shape:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, shape)
		shape:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, shape)
		shape:addEventListener(Event.MOUSE_UP, self.onMouseUp, shape)
		shape:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, shape)
		shape:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, shape)
		shape:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, shape)
		shape:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, shape)
	
		self:addChild(shape)
	end
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = -1
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == -1 then	
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == -1 then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

return M
