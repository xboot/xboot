--------------------------------------------------------------------------------
-- Copyright (c) 2006-2013 Fabien Fleutot and others.
--
-- All rights reserved.
--
-- This program and the accompanying materials are made available
-- under the terms of the Eclipse Public License v1.0 which
-- accompanies this distribution, and is available at
-- http://www.eclipse.org/legal/epl-v10.html
--
-- This program and the accompanying materials are also made available
-- under the terms of the MIT public license which accompanies this
-- distribution, and is available at http://www.lua.org/license.html
--
-- Contributors:
--     Fabien Fleutot - API and implementation
--
--------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Copyright (c) 2011, 2012 Sierra Wireless and others.
-- All rights reserved. This program and the accompanying materials
-- are made available under the terms of the Eclipse Public License v1.0
-- which accompanies this distribution, and is available at
-- http://www.eclipse.org/legal/epl-v10.html
--
-- Contributors:
--     Sierra Wireless - initial API and implementation
-------------------------------------------------------------------------------

---
-- Uses Metalua capabilities to indent code and provide source code offset
-- semantic depth
--
-- @module luaformatter
local M = {}
local mlc = require 'metalua.compiler'
local math = require 'math'
local walk = require 'metalua.walk'

---
--  calculate all ident level
-- @param Source code to analyze
-- @return #table {linenumber = identationlevel}
-- @usage local depth = format.indentLevel("local var")
local function getindentlevel(source,indenttable)

	local function getfirstline(node)
		-- Regular node
		local offsets = node[1].lineinfo
		local first
		local offset
		-- Consider previous comments as part of current chunk
		-- WARNING: This is NOT the default in Metalua
		if offsets.first.comments then
			first = offsets.first.comments.lineinfo.first.line
			offset = offsets.first.comments.lineinfo.first.offset
		else
			first = offsets.first.line
			offset = offsets.first.offset
		end
		return first, offset
	end

	local function getlastline(node)
		-- Regular node
		local offsets = node[#node].lineinfo
		local last
		-- Same for block end comments
		if offsets.last.comments then
			last = offsets.last.comments.lineinfo.last.line
		else
			last = offsets.last.line
		end
		return last
	end

	--
	-- Define AST walker
	--
	local linetodepth = { 0 }
	local walker = {
		block = { },
		expr  = { },
		depth = 0,     -- Current depth while walking
	}

	function walker.block.down(node, parent,...)
		--ignore empty node
		if #node == 0 then
			return end
		-- get first line of the block
		local startline,startoffset = getfirstline(node)
		local endline = getlastline(node)
		-- If the block doesn't start with a new line, don't indent the first line
		if not source:sub(1,startoffset-1):find("[\r\n]%s*$") then
			startline = startline + 1
		end
		for i=startline, endline do
			linetodepth[i]=walker.depth
		end
		walker.depth = walker.depth + 1
	end

	function walker.block.up(node, ...)
		if #node == 0 then
			return end
		walker.depth = walker.depth - 1
	end

	function walker.expr.down(node, parent, ...)
		if indenttable and node.tag == 'Table' then
			if #node == 0 then
				return end
			local startline,startoffset = getfirstline(node)
			local endline = getlastline(node)
			if source:sub(1,startoffset-1):find("[\r\n]%s*$") then
				for i=startline, endline do
					linetodepth[i]=walker.depth
				end
			else
				for i=startline+1, endline do
					linetodepth[i]=walker.depth
				end
			end
			walker.depth = walker.depth + 1
		elseif node.tag =='String' then
			local firstline = node.lineinfo.first.line
			local lastline = node.lineinfo.last.line
			for i=firstline+1, lastline do
				linetodepth[i]=false
			end
		end
	end

	function walker.expr.up(node, parent, ...)
		if indenttable and node.tag == 'Table' then
			if #node == 0 then
				return end
			walker.depth = walker.depth - 1
		end
	end

	-- Walk through AST to build linetodepth
	local ast = mlc.src_to_ast(source)
	mlc.check_ast(ast)
    walk.block(walker, ast)
	return linetodepth
end

---
-- Trim white spaces before and after given string
--
-- @usage local trimmedstr = trim('          foo')
-- @param #string string to trim
-- @return #string string trimmed
local function trim(string)
	local pattern = "^(%s*)(.*)"
	local _, strip =  string:match(pattern)
	if not strip then return string end
	local restrip
	_, restrip = strip:reverse():match(pattern)
	return restrip and restrip:reverse() or strip
end

---
-- Indent Lua Source Code.
-- @function [parent=#luaformatter] indentCode
-- @param source source code to format
-- @param delimiter line delimiter to use, usually '\n' or '\r\n'
-- @param indentTable boolean: whether table content must be indented
-- @param tab either a string representing a number of indentation, or the number
--   of spaces taken by a tab (often 8 or 4)
-- @param indentationSize if given, an indentation of depth `n` shifts the code
--   `indentationSize * n` chars to the right, with a mix of chars and spaces.
--   `tab` must then be a number
-- @return #string formatted code
-- @usage indentCode('local var', '\n', true, '\t')
-- @usage indentCode('local var', '\n', true, --[[tabulationSize]]4, --[[indentationSize]]2)
function M.indentcode(source, delimiter, indenttable, tab, indentationSize)
    checks('string', 'string', '?', 'number|string', '?numer')

    -- function: generates a string which moves `depth` indentation levels from the left.
	local tabulation
    if indentationSize then
        local tabSize = assert(tonumber(tab))
		-- When tabulation size and indentation size are given,
        -- tabulate with a mix of tabs and spaces
		tabulation = function(depth)
			local range      = depth * indentationSize
			local tabCount   = math.floor(range / tabSize)
			local spaceCount = range % tabSize
			return string.rep('\t', tabCount) .. string.rep(' ', spaceCount)
		end
    else
        if type(tab)=='number' then tab = string.rep(' ', tab) end
		tabulation = function (depth) return tab :rep (depth) end
	end

	-- Delimiter position table: positions[x] is the offset of the first character
    -- of the n-th delimiter in the source
	local positions = { 1-#delimiter }
	local a, b = nil, 0
	repeat
        a, b = source :find (delimiter, b+1, true)
        if a then table.insert (positions, a) end
    until not a

	-- Don't try to indent a single line!
	if #positions < 2 then return source end

	-- calculate the line number -> indentation correspondence table
	local linetodepth = getindentlevel(source,indenttable)

	-- Concatenate string with right identation
	local indented = { }
	for  position=1, #positions do
		-- Extract source code line
		local offset = positions[position]
		-- Get the interval between two positions
		local rawline
		if positions[position + 1] then
			rawline = source:sub(offset + delimiterLength, positions[position + 1] -1)
		else
			-- From current prosition to end of line
			rawline = source:sub(offset + delimiterLength)
		end

		-- Trim white spaces
		local indentcount = linetodepth[position]
		if not indentcount then
			indented[#indented+1] = rawline
		else
			local line = trim(rawline)
			-- Append right indentation
			-- Indent only when there is code on the line
			if line:len() > 0 then
				-- Compute next real depth related offset
				-- As is offset is pointing a white space before first statement of block,
				-- We will work with parent node depth
				indented[#indented+1] = tabulation( indentcount)
				-- Append timmed source code
				indented[#indented+1] = line
			end
		end
		-- Append carriage return
		-- While on last character append carriage return only if at end of original source
		if position < #positions or source:sub(source:len()-delimiterLength, source:len()) == delimiter then
			indented[#indented+1] = delimiter
		end
	end
	return table.concat(indented)
end

return M
