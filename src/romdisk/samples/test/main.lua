local class = require "org.xboot.lang.class"

local a = class()

function a:init(i)
	self.i = i or 0
	print("a:init", self, "i = ", self.i)
end

function a:show()
	print("a:show", "i = ", self.i)
end

local b = class(a)

function b:init(j)
	self.j = j or 0
	print("b:init", self, "j = ", self.j)
end

local newa = a(10)
b(24)
a:new(2)
b:new(3)

newa:show()
