local background = display_image:new("background.png")
runtime:add_child(background)

dot1tex = "dot-1.png"
dot2tex = "dot-2.png"
group1tex = "group-1.png"
group2tex = "group-2.png"

group1 = display_object:new()
group1:add_child(display_image:new(group1tex))

for i=0,3 do
	for j=0,3 do
		local dot = display_image:new(dot1tex)
		dot:setxy(i * 45 + 10, j * 45 + 60)
		group1:add_child(dot)
	end
end

group1:setxy(10, 10)
runtime:add_child(group1)


group2 = display_object:new()
group2:add_child(display_image:new(group2tex))

for i=0,3 do
	for j=0,3 do
		local dot = display_image:new(dot2tex)
		dot:setxy(i * 45 + 10, j * 45 + 60)
		group2:add_child(dot)
	end
end

group2:setxy(100, 200)
runtime:add_child(group2)
