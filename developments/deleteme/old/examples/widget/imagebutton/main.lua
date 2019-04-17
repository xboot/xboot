-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
stage:addChild(bg)

-- Create the button
local btn = Widget.ImageButton.new("images/up.png", "images/down.png")
btn:setPosition(100, 100)
stage:addChild(btn)

-- Register to "click" event
local count = 0
btn:addEventListener("click", function(d, e)
	count = count + 1
	print("Image button clicked " .. count .. " times")
end, btn)
