local background = DisplayImage.new(Texture.new("background.png"))
runtime:addChild(background)

local packer = TexturePacker.new("cnum")

local img1 = DisplayImage.new(packer:getTexture("1.png"))
img1:setPosition(0,0)
runtime:addChild(img1)

local img2 = DisplayImage.new(packer:getTexture("2.png"))
img2:setPosition(109,0)
runtime:addChild(img2)

local img3 = DisplayImage.new(packer:getTexture("3.png"))
img3:setPosition(0,109)
runtime:addChild(img3)

local img4 = DisplayImage.new(packer:getTexture("4.png"))
img4:setPosition(109,109)
runtime:addChild(img4)

local img5 = DisplayImage.new(packer:getTexture("5.png"))
img5:setPosition(109+109,0)
runtime:addChild(img5)
