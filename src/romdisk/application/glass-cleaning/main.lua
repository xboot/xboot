local sw, sh = stage:getSize()

local bgimg = Image.new("assets/images/background.png")
stage:addChild(DisplayImage.new(bgimg))

local maskimg = DisplayShape.new(sw, sh)
	:setSource(Pattern.image(bgimg:clone():blur(200)))
	:paint()
	:setSource(Pattern.image(Image.new("assets/images/water.png")))
	:paint()
	:snapshot()
local mask = DisplayImage.new(maskimg)
stage:addChild(mask)

local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		e.stop = true
		local m = DisplayShape.new(sw, sh)
			:setSource(Pattern.image(maskimg))
			:paint()
			:arc(e.x, e.y, 30, 0, 2 * math.pi)
			:setOperator("clear")
			:fill()
			:snapshot()
		maskimg:clear():mask(m)
		mask:markDirty()
		collectgarbage("collect")
	end
end

local function onMouseMove(self, e)
	if self.touchid == -1 then
		e.stop = true
		local m = DisplayShape.new(sw, sh)
			:setSource(Pattern.image(maskimg))
			:paint()
			:arc(e.x, e.y, 30, 0, 2 * math.pi)
			:setOperator("clear")
			:fill()
			:snapshot()
		maskimg:clear():mask(m)
		mask:markDirty()
		collectgarbage("collect")
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
		e.stop = true
		local m = DisplayShape.new(sw, sh)
			:setSource(Pattern.image(maskimg))
			:paint()
			:arc(e.x, e.y, 30, 0, 2 * math.pi)
			:setOperator("clear")
			:fill()
			:snapshot()
		maskimg:clear():mask(m)
		mask:markDirty()
		collectgarbage("collect")
	end
end

local function onTouchMove(self, e)
	if self.touchid == e.id then
		e.stop = true
		local m = DisplayShape.new(sw, sh)
			:setSource(Pattern.image(maskimg))
			:paint()
			:arc(e.x, e.y, 30, 0, 2 * math.pi)
			:setOperator("clear")
			:fill()
			:snapshot()
		maskimg:clear():mask(m)
		mask:markDirty()
		collectgarbage("collect")
	end
end

local function onTouchEnd(self, e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

mask:addEventListener("mouse-down", onMouseDown)
mask:addEventListener("mouse-move", onMouseMove)
mask:addEventListener("mouse-up", onMouseUp)
mask:addEventListener("touch-begin", onTouchBegin)
mask:addEventListener("touch-move", onTouchMove)
mask:addEventListener("touch-end", onTouchEnd)

stage:showfps(false)
stage:showobj(false)