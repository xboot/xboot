#!/usr/bin/lua
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

-- try to define the right lua path.
-- use the debug api to find the path of luadocumentor.lua file
local luadocumentordirpath
local debugpath = debug.getinfo(1).source;
if debugpath then
	-- extract the directory path of luadocumentor.lua
	luadocumentordirpath = string.match(debugpath,"^@(.*)luadocumentor.lua$")
	if luadocumentordirpath then
		if luadocumentordirpath == "" then luadocumentordirpath = "./" end
		-- change lua path and mpath to not load system version of metalua
		package.path = luadocumentordirpath.."?.lua;"..luadocumentordirpath.."?.luac;"
		require "metalua.package"
		package.mpath = luadocumentordirpath.."?.mlua;"
		-- do not change cpath to have access to lfs.
		-- (it must be already installed)
	end
end

--
-- Defining help message.
--

-- This message is compliant to 'lapp', which will match options and arguments
-- from commande line.
local help = [[luadocumentor: tool for Lua Documentation Language
	-f, --format (default doc) Define output format :
		* doc: Will produce HTML documentation from specified file(s) or directories.
		* api: Will produce API file(s) from specified file(s) or directories.
	-d, --dir (default docs) Define an output directory. If the given directory doesn't exist, it will be created.
	-h, --help Display the help.
	-s, --style (default !) The path of your own css file, if you don't want to use the default one. (usefull only for the doc format)
	[directories|files]  Define the paths or the directories of inputs files. Only Lua or C files containing a @module tag will be considered.
]]
local docgenerator = require 'docgenerator'
local lddextractor = require 'lddextractor'
local lapp = require 'pl.lapp'
local args = lapp( help )


if not args or #args < 1 then
	print('No directory provided')
	return
elseif args.help then
	-- Just print help
	print( help )
	return
end

--
-- define css file name
--
local cssfilename = "stylesheet.css"

--
-- Parse files from given folders
--

-- Check if all folders exist
local fs = require 'fs.lfs'
local allpresent, missing = fs.checkdirectory(args)

-- Some of given directories are absent
if missing then
	-- List missing directories
	print 'Unable to open'
	for _, file in ipairs( missing ) do
		print('\t'.. file)
	end
	return
end

-- Get files from given directories
local filestoparse, error = fs.filelist( args )
if not filestoparse then
	print ( error )
	return
end

--
-- Generate documentation only files
--
if args.format == 'api' then
	for _, filename in ipairs( filestoparse ) do

		-- Loading file content
		print('Dealing with "'..filename..'".')
		local file, error = io.open(filename, 'r')
		if not file then
			print ('Unable to open "'..filename.."'.\n"..error)
		else
			local code = file:read('*all')
			file:close()

			--
			-- Creating comment file
			--
			local commentfile, error = lddextractor.generatecommentfile(filename, code)

			-- Getting module name
			-- Optimize me
			local module, moduleerror = lddextractor.generateapimodule(filename, code)
			if not commentfile then
				print('Unable to create documentation file for "'..filename..'"\n'..error)
			elseif not module or not module.name then
				local error = moduleerror and '\n'..moduleerror or ''
				print('Unable to compute module name for "'..filename..'".'..error)
			else
				--
				-- Flush documentation file on disk
				--
				local path = args.dir..fs.separator..module.name..'.lua'
				local status, err = fs.fill(path, commentfile)
				if not status then
					print(err)
				end
			end
		end
	end
	print('Done')
	return
end

-- Deal only supported output types
if args.format ~= 'doc' then
	print ('"'..args.format..'" format is not handled.')
	return
end
-- Generate html form files
local parsedfiles, unparsed = docgenerator.generatedocforfiles(filestoparse, cssfilename)

-- Show warnings on unparsed files
if #unparsed > 0 then
	for _, faultyfile in ipairs( unparsed ) do
		print( faultyfile )
	end
end
-- This loop is just for counting parsed files
-- TODO: Find a more elegant way to do it
local parsedfilescount = 0
for _, p in pairs( parsedfiles) do
	parsedfilescount = parsedfilescount + 1
end
print (parsedfilescount .. ' file(s) parsed.')

-- Create html files
local generated = 0
for _, apifile in pairs ( parsedfiles ) do
	local status, err = fs.fill(args.dir..fs.separator..apifile.name..'.html', apifile.body)
	if status then
		generated = generated + 1
	else
		print( 'Unable to create '..apifile.name..'.html on disk.')
	end
end
print (generated .. ' file(s) generated.')

-- Copying css
local csscontent
if args.style == '!' then
	csscontent = require 'defaultcss'
else
	local css, error = io.open(args.style, 'r')
	if not css then
		print('Unable to open "'..args.style .. '".\n'..error)
		return
	end
	csscontent = css:read("*all")
	css:close()
end

local status, error = fs.fill(args.dir..fs.separator..cssfilename, csscontent)
if not status then
	print(error)
	return
end
print('Adding css')
print('Done')
