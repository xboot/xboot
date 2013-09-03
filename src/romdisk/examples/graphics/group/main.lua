local background = DisplayImage:new(Texture:new("background.png"))
runtime:addChild(background)

dot1tex = Texture:new("dot-1.png")
dot2tex = Texture:new("dot-2.png")
group1tex = Texture:new("group-1.png")
group2tex = Texture:new("group-2.png")

group1 = DisplayObject:new()
group1:addChild(DisplayImage:new(group1tex))

for i=0,3 do
	for j=0,3 do
		local dot = DisplayImage:new(dot1tex)
		dot:setXY(i * 45 + 10, j * 45 + 60)
		group1:addChild(dot)
	end
end

group1:setXY(10, 10)
runtime:addChild(group1)


group2 = DisplayObject:new()
group2:addChild(DisplayImage:new(group2tex))

for i=0,3 do
	for j=0,3 do
		local dot = DisplayImage:new(dot2tex)
		dot:setXY(i * 45 + 10, j * 45 + 60)
		group2:addChild(dot)
	end
end

group2:setXY(100, 200)
runtime:addChild(group2)
