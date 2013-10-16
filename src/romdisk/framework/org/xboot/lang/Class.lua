local function rethack(t, bbak, mbak, ...)
	t.base = bbak
	setmetatable(t.self, mbak)

	return ...
end

local function super(t, k)
	return function(self, ...)
		local bbak = t.base
		local mbak = getmetatable(t.self)

		setmetatable(t.self, t.base)
		t.base = t.base.base

		return rethack(t, bbak, mbak, bbak[k](t.self, ...))
	end
end

return function(b)
	local o = {}
	o.__index = o

	function o.new(...)
		local self = {}
		setmetatable (self, o)
		self.super = setmetatable({self = self, base = b}, {__index = super})

		if self.init then
			self:init(...)
		end

		return self
	end

	if b then
		o.base = b
		setmetatable(o, {__index = b})
	end

	return o
end
