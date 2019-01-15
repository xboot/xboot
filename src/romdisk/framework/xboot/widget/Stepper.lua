local Button = require "xboot.widget.Button"
local Dobject = Dobject

local M = Class(DisplayObject)

function M:init(option, name)
	self.super:init()

	local assets = assets
	local option = option or {}
	local theme = assets:loadTheme(name)

	self.opt = {}
	self.opt.x = option.x or 0
	self.opt.y = option.y or 0
	self.opt.width = option.width
	self.opt.height = option.height
	self.opt.visible = option.visible or true
	self.opt.touchable = option.touchable or true
	self.opt.enable = option.enable or true
	self.opt.min = option.min or 0
	self.opt.max = option.max or 100
	self.opt.step = option.step or 1
	self.opt.value = option.value or 0

	self.opt.imageMinusNormal = assert(option.imageMinusNormal or theme.stepper.imageMinusNormal)
	self.opt.imageMinusPressed = assert(option.imageMinusPressed or theme.stepper.imageMinusPressed)
	self.opt.imageMinusDisabled = assert(option.imageMinusDisabled or theme.stepper.imageMinusDisabled)
	self.opt.imagePlusNormal = assert(option.imagePlusNormal or theme.stepper.imagePlusNormal)
	self.opt.imagePlusPressed = assert(option.imagePlusPressed or theme.stepper.imagePlusPressed)
	self.opt.imagePlusDisabled = assert(option.imagePlusDisabled or theme.stepper.imagePlusDisabled)

	self.buttonMinus = Button.new({
		imageNormal = self.opt.imageMinusNormal,
		imagePressed = self.opt.imageMinusPressed,
		imageDisabled = self.opt.imageMinusDisabled,})
		:setAlignment(Dobject.ALIGN_LEFT)
	self.buttonPlus = Button.new({
		imageNormal = self.opt.imagePlusNormal,
		imagePressed = self.opt.imagePlusPressed,
		imageDisabled = self.opt.imagePlusDisabled,})
		:setAlignment(Dobject.ALIGN_RIGHT)

	self:addChild(self.buttonMinus)
	self:addChild(self.buttonPlus)

	local mw, mh = self.buttonMinus:getSize()
	local pw, ph = self.buttonPlus:getSize()
	self.opt.width = self.opt.width or (mw + pw)
	self.opt.height = self.opt.height or ph

	self.touchid = nil
	self.enable = self.opt.enable
	self.min = self.opt.min
	self.max = self.opt.max
	self.step = self.opt.step
	self.value = self.opt.value

	self:setPosition(self.opt.x, self.opt.y)
	self:setSize(self.opt.width, self.opt.height)
	self:setVisible(self.opt.visible)
	self:setTouchable(self.opt.touchable)
	self:setEnable(self.opt.enable)
	self:setValue(self.opt.value)
	self:updateVisualState()

	self.buttonMinus:addEventListener("Click", function(d, e)
		self:setValue(self:getValue() - 1)
		self:dispatchEvent(Event.new("Change", {value = self.value}))
	end)
	
	self.buttonPlus:addEventListener("Click", function(d, e)
		self:setValue(self:getValue() + 1)
		self:dispatchEvent(Event.new("Change", {value = self.value}))
	end)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	self.buttonMinus:setSize(width / 2, height)
	self.buttonPlus:setSize(width / 2, height)
	self:updateVisualState()
	return self
end

function M:setEnable(enable)
	self.enable = enable
	self:updateVisualState()
	return self
end

function M:getEnable()
	return self.enable
end

function M:setParam(min, max, step)
	self.min = min or self.min
	self.max = max or self.max
	self.step = step or self.step
	self:setValue(self:getValue())
end

function M:getParam()
	return self.min, self.max, self.step
end

function M:setValue(value)
	if value < self.min then
		value = self.min
	elseif value > self.max then
		value = self.max
	end
	self.value = value
	self:updateVisualState()
	return self
end

function M:getValue()
	return self.value
end

function M:enable()
	return self:setEnable(true)
end

function M:disable()
	return self:setEnable(false)
end

function M:updateVisualState()
	if self.enable then
		if self.value <= self.min then
			self.buttonMinus:disable()
			self.buttonPlus:enable()
		elseif self.value >= self.max then
			self.buttonMinus:enable()
			self.buttonPlus:disable()
		else
			self.buttonMinus:enable()
			self.buttonPlus:enable()
		end
	else
		self.buttonMinus:disable()
		self.buttonPlus:disable()
	end
	self:layout()
end

return M
