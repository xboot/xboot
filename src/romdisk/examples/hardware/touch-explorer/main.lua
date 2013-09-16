local background = DisplayImage:new(Texture.new("background.png"))
runtime:addChild(background)

local dots = {
	DisplayImage:new(Texture.new("1.png")),
	DisplayImage:new(Texture.new("2.png")),
	DisplayImage:new(Texture.new("3.png")),
	DisplayImage:new(Texture.new("4.png")),
	DisplayImage:new(Texture.new("5.png")),
}

local function onTouchesBegin(d, e)
	local dot = dots[e.info.id]
	if dot then
		runtime:addChild(dot)
		dot:setXY(e.info.x - dot.width / 2 , e.info.y - dot.height / 2)
	end
end

local function onTouchesMove(d, e)
	local dot = dots[e.info.id]
	if dot then
		dot:setXY(e.info.x - dot.width / 2 , e.info.y - dot.height / 2)
	end
end

local function onTouchesEnd(d, e)
	local dot = dots[e.info.id]
	if dot and runtime:contains(dot) then
		runtime:removeChild(dot)
	end
end

local function onTouchesCancel(d, e)
	local dot = dots[e.info.id]
	if dot and runtime:contains(dot) then
		runtime:removeChild(dot)
	end
end

runtime:addEventListener(Event.TOUCHES_BEGIN, onTouchesBegin)
runtime:addEventListener(Event.TOUCHES_MOVE, onTouchesMove)
runtime:addEventListener(Event.TOUCHES_END, onTouchesEnd)
runtime:addEventListener(Event.TOUCHES_CANCEL, onTouchesCancel)
