local Pattern = Pattern

local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
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
		self.touchid = nil
		e.stop = true
	end
end

local function onTouchBegin(self, e)
	if self:hitTestPoint(e.x, e.y) then
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
		self.touchid = nil
		e.stop = true
	end
end

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new(sw, sh):extend(Image.new("bg.png"), "repeat")))

local panel = Image.new(200, 200)
			:save()
			:setLineWidth(5)
			:rectangle(0, 0, 200, 200)
			:setSourceColor(0.5, 0.7, 0.9, 1)
			:stroke()
			:restore()

local function DrawLine(p, x1, y1, x2, y2, c)
	if math.abs(x2 - x1) > math.abs(y2 - y1) then
		steps = math.abs(x2 - x1)
	else
		steps = math.abs(y2 - y1)
	end
		increx = (x2 - x1) / steps
		increy = (y2 - y1) / steps
		x = x1
		y = y1
	for i = 1, steps do
		p:setPixel(x - x % 1, y - y % 1, c)
		x = x + increx
		y = y + increy
	end
end

local function DrawRectFull(p, x, y, w, h, c)
	for i = 0, w do
		for o = 0, h do
			p:setPixel(x + i,y + o, c)
		end
	end
end

function DrawPutdot(p, x0,y0,x,y,c)
	p:setPixel(x0+x,y0+y,c)
	p:setPixel(x0+x,y0-y,c)
	p:setPixel(x0-x,y0+y,c)
	p:setPixel(x0-x,y0-y,c)
	p:setPixel(x0+y,y0+x,c)
	p:setPixel(x0+y,y0-x,c)
	p:setPixel(x0-y,y0+x,c)
	p:setPixel(x0-y,y0-x,c)
end

local function DrawCircle(p, x0, y0, l, c)
	x=0;y=l;f=5/4-y
	while x<=y do
		DrawPutdot(p,x0 - x0 % 1,y0 - y0 % 1,x - x % 1,y - y % 1,c)
		if f<0 then
			f=f+2*x+3
		else
			f=f+2*(x-y)+5
			y=y-1
		end
		x=x+1
	end
end

DrawLine(panel, 50, 30, 100, 150, Color.new("red"))
DrawCircle(panel, 100, 100, 50, Color.new("blue"))

local img = DisplayImage.new(panel)
	:setPosition(math.random(0, sw - 64), math.random(0, sh - 64))
	:setAnchor(0.5, 0.5)

img:addEventListener("mouse-down", onMouseDown)
img:addEventListener("mouse-move", onMouseMove)
img:addEventListener("mouse-up", onMouseUp)
img:addEventListener("touch-begin", onTouchBegin)
img:addEventListener("touch-move", onTouchMove)
img:addEventListener("touch-end", onTouchEnd)

stage:addChild(img)
