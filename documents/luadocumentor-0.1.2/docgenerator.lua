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

--
-- Load documentation generator and update its path
--
local templateengine  = require 'templateengine'
for name, def in pairs( require 'template.utils' ) do
	templateengine.env [ name ] = def
end

-- Load documentation extractor and set handled languages
local lddextractor = require 'lddextractor'

local M = {}
M.defaultsitemainpagename = 'index'

function M.generatedocforfiles(filenames, cssname)
	if not filenames then return nil, 'No files provided.' end
	--
	-- Generate API model elements for all files
	--
	local generatedfiles = {}
	local wrongfiles = {}
	for _, filename in pairs( filenames ) do
		-- Load file content
		local file, error = io.open(filename, 'r')
		if not file then return nil, 'Unable to read "'..filename..'"\n'..err end
		local code = file:read('*all')
		file:close()
		-- Get module for current file
		local apimodule, err = lddextractor.generateapimodule(filename, code)
		
		-- Handle modules with module name
		if  apimodule and apimodule.name then
			generatedfiles[ apimodule.name ] = apimodule
		elseif not apimodule then
			-- Track faulty files
			table.insert(wrongfiles, 'Unable to extract comments from "'..filename..'".\n'..err)
		elseif not apimodule.name then
			-- Do not generate documentation for unnamed modules
			table.insert(wrongfiles, 'Unable to create documentation for "'..filename..'", no module name provided.')
		end
	end
	--
	-- Defining index, which will summarize all modules
	--
	local index = {
		modules = generatedfiles,
		name = M.defaultsitemainpagename,
		tag='index'
	}
	generatedfiles[ M.defaultsitemainpagename ] = index

	--
	-- Define page cursor
	--
	local page = {
		currentmodule = nil,
		headers = { [[<link rel="stylesheet" href="]].. cssname ..[[" type="text/css"/>]] },
		modules = generatedfiles,
		tag = 'page'
	}

	--
	-- Iterate over modules, generating complete doc pages
	--
	for _, module in pairs( generatedfiles ) do
		-- Update current cursor page
		page.currentmodule = module
		-- Generate page
		local content, error = templateengine.applytemplate(page)
		if not content then return nil, error end
		module.body = content
	end
	return generatedfiles, wrongfiles
end
return M
