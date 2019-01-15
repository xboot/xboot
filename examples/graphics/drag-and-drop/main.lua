local Pattern = Pattern

local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self:toFront():animate({scalex = self.sx * 1.2, scaley = self.sx * 1.2}, 0.1, "inOutElastic")
		self.touchid = -1
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onMouseMove(self, e)
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

local function onMouseUp(self, e)
	if self.touchid == -1 then
		self:toFront():animate({scalex = self.sx * 1, scaley = self.sx * 1}, 0.1, "inOutElastic")
		self.touchid = nil
		e.stop = true
	end
end

local function onTouchBegin(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self:toFront():animate({scalex = self.sx * 1.2, scaley = self.sx * 1.2}, 0.1, "inOutElastic")
		self.touchid = e.id
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onTouchMove(self, e)
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

local function onTouchEnd(self, e)
	if self.touchid == e.id then
		self:toFront():animate({scalex = self.sx * 1, scaley = self.sx * 1}, 0.1, "inOutElastic")
		self.touchid = nil
		e.stop = true
	end
end

local sw, sh = stage:getSize()

stage:addChild(DisplayShape.new(sw, sh)
		:setSource(Pattern.image(assets:loadImage("bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

for i = 1, 10 do
	local img = assets:loadDisplay("tomato.png")
		:setPosition(math.random(0, sw - 64), math.random(0, sh - 64))
		:setScale(math.random() * 0.4 + 0.6)
		:setRotation(math.random(0, 360))
		:setAnchor(0.5, 0.5)
	img.sx = img:getScaleX()
	img.sy = img:getScaleY()
	img.r = img:getRotation()

	img:addEventListener(Event.MOUSE_DOWN, onMouseDown)
	img:addEventListener(Event.MOUSE_MOVE, onMouseMove)
	img:addEventListener(Event.MOUSE_UP, onMouseUp)
	img:addEventListener(Event.TOUCH_BEGIN, onTouchBegin)
	img:addEventListener(Event.TOUCH_MOVE, onTouchMove)
	img:addEventListener(Event.TOUCH_END, onTouchEnd)

	stage:addChild(img)
end

stage:showfps(true)

