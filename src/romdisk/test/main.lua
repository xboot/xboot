local stage = application:getStage()
local assets = application:getAssets()
local timermanager = application:getTimerManager()
local width, height = application:getScreenSize()

stage:addChild(DisplayShape.new(width, height):setSourceColor(0.8, 0.8, 0.8):paint())

-- Create the RadioButton
local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
stage:addChild(radiobutton)

-- Create the CheckBox
local checkbox = Widget.CheckBox.new({x = 300, y = 100})
stage:addChild(checkbox)

checkbox:addEventListener("Change", function(d, e)
	print("Checkbox changed:", e.info.checked)
	radiobutton:setEnable(e.info.checked)
end)