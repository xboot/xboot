local stage = application:getStage()
local width, height = application:getScreenSize()

local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
	:addEventListener("Change", function(d, e) print("RadioButton changed:", e.info.checked) end)
stage:addChild(radiobutton)
