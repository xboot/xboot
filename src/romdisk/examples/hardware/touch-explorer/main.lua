local background = DisplayImage.new(Texture.new("background.png"))
stage:addChild(background)

local dots = {
	DisplayImage.new(Texture.new("1.png")),
	DisplayImage.new(Texture.new("2.png")),
	DisplayImage.new(Texture.new("3.png")),
	DisplayImage.new(Texture.new("4.png")),
	DisplayImage.new(Texture.new("5.png")),
}

local function onTouchesBegin(d, e)
	local dot = dots[e.info.id]
	if dot then
		stage:addChild(dot)
		dot:setPosition(e.info.x - dot:getWidth() / 2 , e.info.y - dot:getHeight() / 2)
	end
end

local function onTouchesMove(d, e)
	local dot = dots[e.info.id]
	if dot then
		dot:setPosition(e.info.x - dot:getWidth() / 2 , e.info.y - dot:getHeight() / 2)
	end
end

local function onTouchesEnd(d, e)
	local dot = dots[e.info.id]
	if dot and stage:contains(dot) then
		stage:removeChild(dot)
	end
end

local function onTouchesCancel(d, e)
	local dot = dots[e.info.id]
	if dot and stage:contains(dot) then
		stage:removeChild(dot)
	end
end

stage:addEventListener(Event.TOUCHES_BEGIN, onTouchesBegin)
stage:addEventListener(Event.TOUCHES_MOVE, onTouchesMove)
stage:addEventListener(Event.TOUCHES_END, onTouchesEnd)
stage:addEventListener(Event.TOUCHES_CANCEL, onTouchesCancel)
