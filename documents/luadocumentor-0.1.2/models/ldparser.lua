-------------------------------------------------------------------------------
-- Copyright (c) 2011-2013 Sierra Wireless and others.
-- All rights reserved. This program and the accompanying materials
-- are made available under the terms of the Eclipse Public License v1.0
-- which accompanies this distribution, and is available at
-- http://www.eclipse.org/legal/epl-v10.html
--
-- Contributors:
--     Sierra Wireless - initial API and implementation
-------------------------------------------------------------------------------
local mlc = require ('metalua.compiler').new()
local gg = require 'metalua.grammar.generator'
local lexer = require 'metalua.grammar.lexer'
local mlp = mlc.parser

local M = {} -- module
local lx -- lexer used to parse tag
local registeredparsers -- table {tagname => {list de parsers}}

-- raise an error if result contains a node error
local function raiserror(result)
	for i, node in ipairs(result) do
		assert(not node or node.tag ~= "Error")
	end
end


-- copy key and value from one table to an other
local function copykey(tablefrom, tableto)
	for key, value in pairs(tablefrom) do
		if key ~= "lineinfos" then
			tableto[key] = value
		end
	end
end

------------------------------------------------------
-- Handle keyword and identifiers as word
local function parseword(lx)
	local word = lx :peek()
	local tag = word.tag
	
	if tag=='Keyword' or tag=='Id' then
		lx:next()
		return  {tag='Word', lineinfo=word.lineinfo, word[1]}
	else
		return gg.parse_error(lx,'Id or Keyword expected')
	end
end

------------------------------------------------------
-- parse an id
-- return a table {name, lineinfo)
local idparser = gg.sequence({
	builder =	function (result)
		raiserror(result)
		return { name = result[1][1] }
	end,
	parseword
})

------------------------------------------------------
-- parse a modulename  (id.)?id
-- return a table {name, lineinfo)
local modulenameparser = gg.list({
	builder =	function (result)
		raiserror(result)
		local ids = {}
		for i, id in ipairs(result) do
			table.insert(ids,id.name)
		end
		return {name = table.concat(ids,".")}
	end,
	primary  = idparser,
	separators = '.'
})
------------------------------------------------------
-- parse a typename  (id.)?id
-- return a table {name, lineinfo)
local typenameparser= modulenameparser

------------------------------------------------------
-- parse an internaltype ref
-- return a table {name, lineinfo)
local internaltyperefparser = gg.sequence({
	builder =	function(result)
		raiserror(result)
		return {tag = "typeref",type=result[1].name}
	end,
	"#", typenameparser
})

------------------------------------------------------
-- parse en external type ref
-- return a table {name, lineinfo)
local externaltyperefparser = gg.sequence({
	builder =	function(result)
		raiserror(result)
		return {tag = "typeref",module=result[1].name,type=result[2].name}
	end,
	modulenameparser,"#", typenameparser
})


------------------------------------------------------
-- parse a typeref
-- return a table {name, lineinfo)
local typerefparser =	gg.multisequence{
	internaltyperefparser,
	externaltyperefparser}

------------------------------------------------------
-- parse a list of typeref
-- return a list of table {name, lineinfo)
local typereflistparser = gg.list({
	primary  = typerefparser,
	separators = ','
})

------------------------------------------------------
-- TODO use a more generic way to parse (modifier if not always a typeref)
-- TODO support more than one modifier
local modifiersparser = gg.sequence({
	builder =	function(result)
		raiserror(result)
		return {[result[1].name]=result[2]}
	end,
	"[", idparser ,  "=" , internaltyperefparser , "]"
})

------------------------------------------------------
-- parse a return tag
local returnparsers = {
	-- full parser
	gg.sequence({
		builder =	function (result)
			raiserror(result)
			return { types= result[1]}
		end,
		'@','return', typereflistparser
	}),
	-- parser without typerefs
	gg.sequence({
		builder =	function (result)
			raiserror(result)
			return { types = {}}
		end,
		'@','return'
	})
}

------------------------------------------------------
-- parse a param tag
local paramparsers = {
	-- full parser
	gg.sequence({
		builder =	function (result)
			raiserror(result)
			return { name = result[2].name, type = result[1]}
		end,
		'@','param', typerefparser, idparser
	}),

	-- full parser without type
	gg.sequence({
		builder = function (result)
			raiserror(result)
			return { name = result[1].name}
		end,
		'@','param', idparser
	}),

	-- Parser for `Dots
	gg.sequence({
		builder = function (result)
			raiserror(result)
			return { name = '...' }
		end,
		'@','param', '...'
	}),
}
------------------------------------------------------
-- parse a field tag
local fieldparsers = {
	-- full parser
	gg.sequence({
		builder =	function (result)
			raiserror(result)
			local tag = {}
			copykey(result[1],tag)
			tag.type = result[2]
			tag.name = result[3].name
			return tag
		end,
		'@','field', modifiersparser, typerefparser, idparser
	}),


	-- parser  without modifiers
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			return { name = result[2].name, type = result[1]}
		end,
		'@','field', typerefparser, idparser
	}),

	-- parser without type
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			local tag = {}
			copykey(result[1],tag)
			tag.name = result[2].name
			return tag
		end,
		'@','field', modifiersparser, idparser
	}),

	-- parser without type without modifiers
	gg.sequence({
		builder = function (result)
			raiserror(result)
			return { name = result[1].name}
		end,
		'@','field', idparser
	})
}

------------------------------------------------------
-- parse a function tag
-- TODO use a more generic way to parse modifier !
local functionparsers = {
	-- full parser
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			local tag = {}
			copykey(result[1],tag)
			tag.name = result[2].name
			return tag
		end,
		'@','function', modifiersparser, idparser
	})
}

------------------------------------------------------
-- parse a type tag
local typeparsers = {
	-- full parser
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			return { name = result[1].name}
		end,
		'@','type',typenameparser
	})
}

------------------------------------------------------
-- parse a module tag
local moduleparsers = {
	-- full parser
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			return { name = result[1].name }
		end,
		'@','module', modulenameparser
	}),
	-- parser without name
	gg.sequence({
		builder = 	function (result)
			raiserror(result)
			return {}
		end,
		'@','module'
	})
}

------------------------------------------------------
-- parse a third tag
local thirdtagsparser = gg.sequence({
	builder = 	function (result)
		raiserror(result)
		return { name = result[1][1] }
	end,
	'@', mlp.id
})
------------------------------------------------------------
-- init parser
local function initparser()
	-- register parsers
	-- each tag name has several parsers
	registeredparsers  = {
		["module"]   = moduleparsers,
		["return"]   = returnparsers,
		["type"]     = typeparsers,
		["field"]    = fieldparsers,
		["function"] = functionparsers,
		["param"]    = paramparsers
	}

	-- create lexer used for parsing
	lx = lexer.lexer:clone()
	lx.extractors = {
		-- "extract_long_comment",
		-- "extract_short_comment",
		-- "extract_long_string",
		"extract_short_string",
		"extract_word",
		"extract_number",
		"extract_symbol"
	}

	-- Add dots as keyword
	local tagnames = { '...' }

	-- Add tag names as key word
	for tagname, _ in pairs(registeredparsers) do
		table.insert(tagnames,tagname)
	end
	lx:add(tagnames)

	return lx, parsers
end

initparser()

------------------------------------------------------------
-- get the string pattern to remove for each line of description
-- the goal is to fix the indentation problems
local function getstringtoremove (stringcomment,commentstart)
	local _,_,capture = string.find(stringcomment,"\n?([ \t]*)@[^{]+",commentstart)
	if not capture then
		_,_,capture = string.find(stringcomment,"^([ \t]*)",commentstart)
	end
	capture = string.gsub(capture,"(.)","%1?")
	return capture
end

------------------------------------------------------------
-- parse comment tag partition and return table structure
local function parsetag(part)
	if part.comment:find("^@") then
		-- check if the part start by a supported tag
		for tagname,parsers in pairs(registeredparsers) do
			if (part.comment:find("^@"..tagname)) then
				-- try the registered parsers for this tag
				local result
				for i, parser in ipairs(parsers) do
					local valid, tag = pcall(parser, lx:newstream(part.comment, tagname .. 'tag lexer'))
					if valid then
						-- add tagname
						tag.tagname = tagname

						-- add description
						local endoffset = tag.lineinfo.last.offset
						tag.description = part.comment:sub(endoffset+2,-1)
						return tag
					end
				end
			end
		end
	end
	return nil
end

------------------------------------------------------------
-- Parse third party tags.
--
-- Enable to parse a tag not defined in language.
-- So for, accepted format is: @sometagname adescription
local function parsethirdtag( part )

	-- Check it there is someting to process
	if not part.comment:find("^@") then
		return nil, 'No tag to parse'
	end

	-- Apply parser
	local status, parsedtag = pcall(thirdtagsparser, lx:newstream(part.comment, 'Third party tag lexer'))
	if not status then
		return nil, "Unable to parse given string."
	end

	-- Retrieve description
	local endoffset = parsedtag.lineinfo.last.offset
	local tag = {
		description = part.comment:sub(endoffset+2,-1)
	}
	return parsedtag.name, tag
end

------------------------------------------------------------
-- split string comment in several part
-- return list of {comment = string, offset = number}
-- the first part is the part before the first tag
-- the others are the part from a tag to the next one
local function split(stringcomment,commentstart)
	local partstart = commentstart
	local result = {}

	-- manage case where the comment start by @
	-- (we must ignore the inline see tag @{..})
	local at_startoffset, at_endoffset = stringcomment:find("^[ \t]*@[^{]",partstart)
	if at_endoffset then
		partstart = at_endoffset-1 -- we start before the @ and the non '{' character
	end

	-- split comment
	-- (we must ignore the inline see tag @{..})
	repeat
		at_startoffset, at_endoffset = stringcomment:find("\n[ \t]*@[^{]",partstart)
		local partend
		if at_startoffset then
			partend= at_startoffset-1 -- the end is before the separator pattern (just before the \n)
		else
			partend = #stringcomment -- we don't find any pattern so the end is the end of the string
		end
		table.insert(result, { comment = stringcomment:sub (partstart,partend) ,
			offset = partstart})
		if at_endoffset then
			partstart = at_endoffset-1 -- the new start is befire the @ and the non { char
		end
	until not at_endoffset
	return result
end


------------------------------------------------------------
-- parse a comment block and return a table
function M.parse(stringcomment)

	local _comment = {description="", shortdescription=""}

	-- clean windows carriage return
	stringcomment = string.gsub(stringcomment,"\r\n","\n")

	-- check if it's a ld comment
	-- get the begin of the comment
	-- ============================
	if not stringcomment:find("^-") then
		-- if this comment don't start by -, we will not handle it.
		return nil
	end

	-- retrieve the real start
	local commentstart = 2 --after the first hyphen
	-- if the first line is an empty comment line with at least 3 hyphens we ignore it
	local  _ , endoffset = stringcomment:find("^-+[ \t]*\n")
	if endoffset then
		commentstart = endoffset+1
	end

	-- clean comments
	-- ===================
	-- remove line of "-"
	stringcomment = string.sub(stringcomment,commentstart)
	-- clean indentation
	local pattern = getstringtoremove (stringcomment,1)
	stringcomment = string.gsub(stringcomment,"^"..pattern,"")
	stringcomment = string.gsub(stringcomment,"\n"..pattern,"\n")

	-- split comment part
	-- ====================
	local commentparts = split(stringcomment, 1)

	-- Extract descriptions
	-- ====================
	local firstpart = commentparts[1].comment
	if firstpart:find("^[^@]") or firstpart:find("^@{") then
		-- if the comment part don't start by @
		-- it's the part which contains descriptions
		-- (there are an exception for the in-line see tag @{..})
		local shortdescription, description = string.match(firstpart,'^(.-[.?])(%s.+)')
		-- store description
		if shortdescription then
			_comment.shortdescription = shortdescription
			-- clean description
			-- remove always the first space character
			-- (this manage the case short and long description is on the same line)
			description = string.gsub(description, "^[ \t]","")
			-- if first line is only an empty string remove it
			description = string.gsub(description, "^[ \t]*\n","")
			_comment.description = description
		else
			_comment.shortdescription = firstpart
			_comment.description = ""
		end
	end

	-- Extract tags
	-- ===================
	-- Parse regular tags
	local tag
	for i, part in ipairs(commentparts) do
		tag = parsetag(part)
		--if it's a supported tag (so tag is not nil, it's a table)
		if tag then
			if not _comment.tags then _comment.tags = {} end
			if not _comment.tags[tag.tagname] then
				_comment.tags[tag.tagname] = {}
			end
			table.insert(_comment.tags[tag.tagname], tag)
		else

			-- Try user defined tags, so far they will look like
			-- @identifier description
			local tagname, thirdtag = parsethirdtag( part )
			if tagname then
				--
				-- Append found tag
				--
				local reservedname = 'unknowntags'
				if not _comment.unknowntags then
					_comment.unknowntags = {}
				end

				-- Create specific section for parsed tag
				if not _comment.unknowntags[tagname] then
					_comment.unknowntags[tagname] = {}
				end
				-- Append to specific section
				table.insert(_comment.unknowntags[tagname], thirdtag)
			end
		end
	end
	return _comment
end


function M.parseinlinecomment(stringcomment)
	--TODO this code is use to activate typage only on --- comments. (deactivate for now)
	--	if not stringcomment or not stringcomment:find("^-") then
	--		-- if this comment don't start by -, we will not handle it.
	--		return nil
	--	end
	--	-- remove the first '-'
	--	stringcomment = string.sub(stringcomment,2)
	--	print (stringcomment)
	--	io.flush()
	local valid, parsedtag = pcall(typerefparser, lx:newstream(stringcomment, 'typeref parser'))
	if valid then
		local endoffset = parsedtag.lineinfo.last.offset
		parsedtag.description = stringcomment:sub(endoffset+2,-1)
		return parsedtag
	end
end

return M
