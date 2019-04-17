-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
stage:addChild(bg)

-- Create up and down display object for the button
local up = DisplayImage.new(Texture.new("images/up.png"))
local down = DisplayImage.new(Texture.new("images/down.png"))

-- Create the button
local btn = Widget.Button.new(up, down)
btn:setPosition(100, 100)
stage:addChild(btn)

-- Register to "click" event
local count = 0
btn:addEventListener("click", function(d, e)
	count = count + 1
	print("Button clicked " .. count .. " times")
end, btn)
