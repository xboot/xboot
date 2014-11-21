local Game = require("games.2048.Game")

local M = Class(DisplayObject)

function M:init()
	self.super:init()
	
	local game = Game.new()
	game:addEventListener(Event.MOUSE_DOWN, self.onMouseDown)
	game:addEventListener(Event.MOUSE_MOVE, self.onMouseMove)
	game:addEventListener(Event.MOUSE_UP, self.onMouseUp)
	game:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin)
	game:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove)
	game:addEventListener(Event.TOUCHES_END, self.onTouchesEnd)
	game:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel)

	self:addChild(game)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = -1
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == -1 then	
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == -1 then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

return M
