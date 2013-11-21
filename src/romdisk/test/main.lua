local stage = application:getStage()
local assets = application:getAssets()
local timermanager = application:getTimerManager()

local background = DisplayImage.new(assets:loadTexture("assets/images/bg.png"))
stage:addChild(background)

-- Create the button
local btn1 = Widget.Button.new({x = 100, y = 100, width = 100, height = 100})
btn1:setAnchor(0.5, 0.5):setPosition(100, 100)
stage:addChild(btn1)

-- Create the button
local btn2 = Widget.Button.new({x = 300, y = 100, width = 100, height = 100})
btn2:setAnchor(0, 0):setPosition(300, 100)
stage:addChild(btn2)


btn2:setContentSize(300, 300)

timermanager:addTimer(Timer.new(1, 0, function(t, e)
	btn1:animate({rotation = btn1:getRotation() + 20}, 2/3, "outBounce")
	btn2:animate({rotation = btn2:getRotation() + 20}, 2/3, "outBounce")
end))
