local Tile = require("games.2048.Tile")
local Grid = require("games.2048.Grid")

local M = Class(DisplayObject)

function M:init()
	self.super:init()
	
	
	local w, h = application:getScreenSize()
	local assets = application:getAssets()
	local bg = assets:loadDisplay("games/2048/gamebg.png")
	
	self:addChild(bg)
	self:addChild(Tile.new(2))
	self:addChild(Tile.new(4))
	self:addChild(Tile.new(8))
	self:addChild(Tile.new(16))
	self:addChild(Tile.new(32))
	self:addChild(Tile.new(64))
	self:addChild(Tile.new(128))
	self:addChild(Tile.new(256))
	self:addChild(Tile.new(512))
	self:addChild(Tile.new(1024))
	self:addChild(Tile.new(2048))
	self:addChild(Tile.new(4096))
	self:addChild(Tile.new(8192))
	
	local width, height = bg:getSize()
	self:setSize(width, height)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

return M
