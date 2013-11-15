--------------------------------------------------------------------------------
--  Copyright (c) 2012 Sierra Wireless.
--  All rights reserved. This program and the accompanying materials
--  are made available under the terms of the Eclipse Public License v1.0
--  which accompanies this distribution, and is available at
--  http://www.eclipse.org/legal/epl-v10.html
--
--  Contributors:
--       Kevin KIN-FOO <kkinfoo@sierrawireless.com>
--           - initial API and implementation and initial documentation
--------------------------------------------------------------------------------
local lfs = require 'lfs'
local M = {}
local function iswindows()
	local p = io.popen("echo %os%")
	if not p then
		return false
	end
	local result =p:read("*l")
	p:close()
	return result == "Windows_NT"
end
M.separator = iswindows() and [[\]] or [[/]]
---
-- Will recursively browse given directories and list files encountered
-- @param tab Table, list where files will be added
-- @param dirorfiles list of path to browse in order to build list.
--	Files from this list will be added to <code>tab</code> list.
-- @return <code>tab</code> list, table containing all files from directories
--	and files contained in <code>dirorfile</code>
local function appendfiles(tab, dirorfile)

	-- Nothing to process
	if #dirorfile < 1 then return tab end

	-- Append all files to list
	local dirs = {}
	for _, path in ipairs( dirorfile ) do
		-- Determine element nature
		local elementnature = lfs.attributes (path, "mode")

		-- Handle files
		if elementnature == 'file' then
			table.insert(tab, path)
		else if elementnature == 'directory' then

				-- Check if folder is accessible
				local status, error = pcall(lfs.dir, path)
				if not status then return nil, error end

				--
				-- Handle folders
				--
				for diskelement in lfs.dir(path) do

					-- Format current file name
					local currentfilename
					if path:sub(#path) == M.separator then
						currentfilename = path  .. diskelement
					else
						currentfilename = path  .. M.separator .. diskelement
					end

					-- Handle folder elements
					local nature, err = lfs.attributes (currentfilename, "mode")
					-- Append file to current list
					if nature == 'file' then
						table.insert(tab, currentfilename)
					elseif nature == 'directory' then
						-- Avoid current and parent directory in order to avoid
						-- endless recursion
						if diskelement ~= '.' and diskelement ~= '..' then
							-- Handle subfolders
							table.insert(dirs, currentfilename)
						end
					end
				end
			end
		end
	end
	-- If we only encountered files, going deeper is useless
	if #dirs == 0 then return tab end
	-- Append files from encountered directories
	return appendfiles(tab, dirs)
end
---
-- Provide a list of files from a directory
-- @param list Table of directories to browse
-- @return table of string, path to files contained in given directories
function M.filelist(list)
	if not list	then return nil, 'No directory list provided' end
	return appendfiles({}, list)
end
function M.checkdirectory( dirlist )
	if not dirlist then return false end
	local missingdirs = {}
	for _, filename in ipairs( dirlist ) do
		if not lfs.attributes(filename, 'mode') then
			table.insert(missingdirs, filename)
		end
	end
	if #missingdirs > 0 then
		return false, missingdirs
	end
	return true
end
function M.fill(filename, content)
	--
	-- Ensure parent directory exists
	--
	local parent = filename:gmatch([[(.*)]] .. M.separator ..[[(.+)]])()
	local parentnature = lfs.attributes(parent, 'mode')
	-- Create parent directory while absent
	if not parentnature then
		lfs.mkdir( parent )
	elseif parentnature ~= 'directory' then
		-- Notify that disk element already exists
		return nil, parent..' is a '..parentnature..'.'
	end

	-- Create actual file
	local file, error = io.open(filename, 'w')
	if not file then
		return nil, error
	end
	file:write( content )
	file:close()
	return true
end
return M
