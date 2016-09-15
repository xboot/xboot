local GPIO = require("hardware.gpio")
local M = Class()

function M:init(cpin, dpin, n)
	self.clk = GPIO.new(cpin)
	self.dat = GPIO.new(dpin)
	self.num = n or 1
	self.buf = {}
	for i = 1, self.num, 1 do
		self.buf[i] = {r = 0, g = 0, b = 0}
	end

	self.clk:setDir(GPIO.DIR_OUTPUT)
	self.dat:setDir(GPIO.DIR_OUTPUT)
	self.clk:setValue(1)
	self.dat:setValue(1)
end

function M:hcl2rgb(h, c, l)
	local h = h % 768
	local x = math.floor(c * (128 - math.abs((h % 256) - 128)) / 128)
	local r, g, b
	if h < 128 then
		r, g, b = c, x, 0
	elseif h < 256 then
		r, g, b = x, c, 0
	elseif h < 384 then
		r, g, b = 0, c, x
	elseif h < 512 then
		r, g, b = 0, x, c
	elseif h < 640 then
		r, g, b = x, 0, c
	else
		r, g, b = c, 0, x
	end
	local m = math.floor(l - (77 * r + 151 * g + 28 * b) / 256)
	r, g, b = r + m, g + m, b + m

	return g < 0 and 0 or g > 255 and 255 or g,
		r < 0 and 0 or r > 255 and 255 or r,
		b < 0 and 0 or b > 255 and 255 or b
end

function M:rgbColor(i, r, g, b)
	self.buf[i].r = r or self.buf[i].r
	self.buf[i].g = g or self.buf[i].g
	self.buf[i].b = b or self.buf[i].b
end

function M:hclColor(i, h, c, l)
	local r, g, b = self:hcl2rgb(h, c, l)
	self.buf[i].r = r
	self.buf[i].g = g
	self.buf[i].b = b
end

function M:send(byte)
	for i = 1, 8, 1 do
		if byte & 0x80 == 0x80 then
			self.dat:setValue(1)
		else
			self.dat:setValue(0)
		end
		self.clk:setValue(0)
		self.clk:setValue(1)
		byte = byte << 1
	end
end

function M:refresh()
	self:send(0)
	self:send(0)
	self:send(0)
	self:send(0)

	for i = 1, self.num, 1 do
		local r = self.buf[i].r
		local g = self.buf[i].g
		local b = self.buf[i].b
		local prefix = 0xc0;
		if b & 0x80 == 0 then prefix = prefix | 0x20 end
		if b & 0x40 == 0 then prefix = prefix | 0x10 end
		if g & 0x80 == 0 then prefix = prefix | 0x08 end
		if g & 0x40 == 0 then prefix = prefix | 0x04 end
		if r & 0x80 == 0 then prefix = prefix | 0x02 end
		if r & 0x40 == 0 then prefix = prefix | 0x01 end

		self:send(prefix)
		self:send(b)
		self:send(g)
		self:send(r)
	end

	self:send(0)
	self:send(0)
	self:send(0)
	self:send(0)
end

return M