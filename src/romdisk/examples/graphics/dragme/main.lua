local stage = application:getStage()
local width, height = application:getScreenSize()

local function onMouseDown(self, e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = -1
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onMouseMove(self, e)
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

local function onMouseUp(self, e)
	if self.touchid == -1 then
		self.touchid = nil
		e:stopPropagation()
	end
end

local function onTouchesBegin(self, e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

local function onTouchesMove(self, e)
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

local function onTouchesEnd(self, e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

local function onTouchesCancel(self, e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

for i=1,5 do
	local shape = DisplayShape.new(100, 50)
		:setLineWidth(6)
		:rectangle(0, 0, 100, 50)
		:setSourceColor(1, 0, 0, 0.5)
		:fillPreserve()
		:setSourceColor(0, 0, 0)
		:stroke()
		:setPosition(math.random(0, width - 100), math.random(0, height - 50))
	
	shape:addEventListener(Event.MOUSE_DOWN, onMouseDown, shape)
	shape:addEventListener(Event.MOUSE_MOVE, onMouseMove, shape)
	shape:addEventListener(Event.MOUSE_UP, onMouseUp, shape)
	shape:addEventListener(Event.TOUCHES_BEGIN, onTouchesBegin, shape)
	shape:addEventListener(Event.TOUCHES_MOVE, onTouchesMove, shape)
	shape:addEventListener(Event.TOUCHES_END, onTouchesEnd, shape)
	shape:addEventListener(Event.TOUCHES_CANCEL, onTouchesCancel, shape)
	
	stage:addChild(shape)
end
