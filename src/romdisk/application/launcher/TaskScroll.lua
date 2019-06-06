local TaskItem = require "TaskItem"

local M = Class(DisplayScroll)

function M:init(width, height)
	self.super:init(width, height, false)
	self:reload()
end

function M:setWidth(width)
	return self
end

function M:setHeight(height)
	return self
end

function M:setSize(width, height)
	return self
end

function M:reload()
	local sw, sh = self:getSize()
	local launcher = Application.new()

	self:clear()
    for k, v in pairs(Window.list()) do
        if launcher:getPath() ~= k then
            local task = TaskItem.new(sw / 2, sh, v)
				:addEventListener("click", function(d, e)
					self:dispatchEvent(Event.new("execute"))
					d:execute()
				end)
            self:addItem(task)
        end
    end
end

return M
