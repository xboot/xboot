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
local apimodel = require 'models.apimodel'

---
-- @module docutils
-- Handles link generation, node quick description.
--
-- Provides:
--	* link generation
--	* anchor generation
--	* node quick description
local M = {}

function M.isempty(map)
	local f = pairs(map)
	return f(map) == nil
end

---
-- Provide a handling function for all supported anchor types
--                 recordtypedef => #(typename)
-- item (field of recordtypedef) => #(typename).itemname
--                 item (global) => itemname
M.anchortypes = {
	recordtypedef = function (o) return string.format('#(%s)', o.name) end,
	item = function(o)
		if not o.parent or o.parent.tag == 'file' then
			-- Handle items referencing globals
			return o.name
		elseif o.parent and o.parent.tag == 'recordtypedef' then
			-- Handle items included in recordtypedef
			local recordtypedef = o.parent
			local recordtypedefanchor = M.anchor(recordtypedef)
			if not recordtypedefanchor then
				return nil, 'Unable to generate anchor for `recordtypedef parent.'
			end
			return string.format('%s.%s', recordtypedefanchor, o.name)
		end
		return nil, 'Unable to generate anchor for `item'
	end
}

---
-- Provides anchor string for an object of API mode
--
-- @function [parent = #docutils] anchor
-- @param modelobject Object form API model
-- @result #string Anchor for an API model object, this function __may rise an error__
-- @usage # -- In a template
-- # local anchorname = anchor(someobject)
-- <a id="$(anchorname)" />
function M.anchor( modelobject )
	local tag = modelobject.tag
	if M.anchortypes[ tag ] then
		return M.anchortypes[ tag ](modelobject)
	end
	return nil, string.format('No anchor available for `%s', tag)
end

local function getexternalmodule( item )
	-- Get file which contains this item
	local file
	if item.parent then
		if item.parent.tag =='recordtypedef' then
			local recordtypedefparent = item.parent.parent
			if recordtypedefparent and recordtypedefparent.tag =='file'then
				file = recordtypedefparent
			end
		elseif item.parent.tag =='file' then
			file = item.parent
		else
			return nil, 'Unable to fetch item parent'
		end
	end
	return file
end

---
-- Provide a handling function for all supported link types
--
--                        internaltyperef => ##(typename)
--                                        => #anchor(recordtyperef)
--                        externaltyperef => modulename.html##(typename)
--                                        => linkto(file)#anchor(recordtyperef)
--                           file(module) => modulename.html
--                                  index => index.html
--                          recordtypedef => ##(typename)
--                                        => #anchor(recordtyperef)
-- item (internal field of recordtypedef) => ##(typename).itemname
--                                        => #anchor(item)
--                 item (internal global) => #itemname
--                                        => #anchor(item)
-- item (external field of recordtypedef) => modulename.html##(typename).itemname
--                                        => linkto(file)#anchor(item)
--                  item (externalglobal) => modulename.html#itemname
--                                        => linkto(file)#anchor(item)
M.linktypes = {
	internaltyperef	= function(o) return string.format('##(%s)', o.typename) end,
	externaltyperef	= function(o) return string.format('%s.html##(%s)', o.modulename, o.typename) end,
	file            = function(o) return string.format('%s.html', o.name) end,
	index           = function() return 'index.html' end,
	recordtypedef   = function(o)
		local anchor = M.anchor(o)
		if not anchor then
			return nil, 'Unable to generate anchor for `recordtypedef.'
		end
		return string.format('#%s', anchor)
	end,
	item = function(o)

		-- For every item get anchor
		local anchor = M.anchor(o)
		if not anchor then
			return nil, 'Unable to generate anchor for `item.'
		end

		-- Built local link to item
		local linktoitem = string.format('#%s', anchor)

		--
		-- For external item, prefix with the link to the module.
		--
		-- The "external item" concept is used only here for short/embedded
		-- notation purposed. This concept and the `.external` field SHALL NOT
		-- be used elsewhere.
		--
		if o.external then

			-- Get link to file which contains this item
			local file = getexternalmodule( o )
			local linktofile = file and M.linkto( file )
			if not linktofile then
				return nil, 'Unable to generate link for external `item.'
			end

			-- Built external link to item
			linktoitem = string.format("%s%s", linktofile, linktoitem)
		end

		return linktoitem
	end
}

---
-- Generates text for HTML links from API model element
--
-- @function [parent = #docutils]
-- @param modelobject Object form API model
-- @result #string Links text for an API model element, this function __may rise an error__.
-- @usage # -- In a template
-- <a href="$( linkto(api) )">Some text</a>
function M.linkto( apiobject )
	local tag = apiobject.tag
	if M.linktypes[ tag ] then
		return M.linktypes[tag](apiobject)
	end
	if not tag then
		return nil, 'Link generation is impossible as no tag has been provided.'
	end
	return nil, string.format('No link generation available for `%s.', tag)
end

---
-- Provide a handling function for all supported pretty name types
--                                 primitivetyperef => #typename
--                                  internaltyperef => #typename
--                                  externaltyperef => modulename#typename
--                                     file(module) => modulename
--                                            index => index
--                                    recordtypedef => typename
--        item (internal function of recordtypedef) => typename.itemname(param1, param2,...)
--  item (internal func with self of recordtypedef) => typename:itemname(param2)
--  item (internal non func field of recordtypedef) => typename.itemname
--                      item (internal func global) => functionname(param1, param2,...)
--                  item (internal non func global) => itemname
--        item (external function of recordtypedef) => modulename#typename.itemname(param1, param2,...)
--  item (external func with self of recordtypedef) => modulename#typename:itemname(param2)
--  item (external non func field of recordtypedef) => modulename#typename.itemname
--                      item (external func global) => functionname(param1, param2,...)
--                  item (external non func global) => itemname
M.prettynametypes = {
	primitivetyperef = function(o) return string.format('#%s', o.typename) end,
	externaltyperef = function(o) return string.format('%s#%s', o.modulename, o.typename) end,
	index = function(o) return "index" end,
	file = function(o) return o.name end,
	recordtypedef = function(o) return o.name end,
	item = function( o )

		-- Determine item name
		-- ----------------------
		local itemname = o.name
		
		-- Determine scope
		-- ----------------------
		local parent = o.parent
		local isglobal = parent and parent.tag == 'file'
		local isfield = parent and parent.tag == 'recordtypedef'

		-- Determine type name
		-- ----------------------
		 
		local typename = isfield and parent.name

		-- Fetch item definition
		-- ----------------------
		-- Get file object
		local file
		if isglobal then
			file = parent
		elseif isfield then
			file = parent.parent
		end
		-- Get definition
		local definition
		if file and o.type and o.type.typename then
			definition = file.types[ o.type.typename ]
		end				
		
		
		
		-- Build prettyname 
		-- ----------------------
		local prettyname
		if not definition or definition.tag ~= 'functiontypedef' then
			-- Fields
			if isglobal or not typename then			   
				prettyname = itemname
			else
				prettyname = string.format('%s.%s', typename, itemname)
			end
		else
			-- Functions
			-- Build parameter list
			local paramlist = {}
			local hasfirstself = false
			for position, param in ipairs(definition.params) do
				-- For non global function, when first parameter is 'self',
				-- it will not be part of listed parameters
				if position == 1 and param.name == 'self' and isfield then
					hasfirstself = true
				else
					table.insert(paramlist, param.name)
					if position ~= #definition.params then
						table.insert(paramlist, ', ')
					end
				end
			end

			if isglobal or not typename then
				prettyname = string.format('%s(%s)',itemname, table.concat(paramlist))
			else
				-- Determine function prefix operator,
				-- ':' if 'self' is first parameter, '.' else way
				local operator = hasfirstself and ':' or '.'
	
				-- Append function parameters
				prettyname = string.format('%s%s%s(%s)',typename, operator, itemname, table.concat(paramlist))
			end
		end
		
		-- Manage external Item prettyname 
		-- ----------------------
		local externalmodule = o.external and getexternalmodule( o )
		local externalmodulename = externalmodule and externalmodule.name
		
		if externalmodulename then
			return string.format('%s#%s',externalmodulename,prettyname)
		else
			return prettyname
		end
	end
}
M.prettynametypes.internaltyperef = M.prettynametypes.primitivetyperef

---
-- Provide human readable overview from an API model element
--
-- Resolve all element needed to summurize nicely an element form API model.
-- @usage $ print( prettyname(item) )
--	module:somefunction(secondparameter)
-- @function [parent = #docutils]
-- @param apiobject Object form API model
-- @result #string Human readable description of given element.
-- @result #nil, #string In case of error.
function M.prettyname( apiobject )
	local tag = apiobject.tag
	if M.prettynametypes[tag] then
		return M.prettynametypes[tag](apiobject)
	elseif not tag then
		return nil, 'No pretty name available as no tag has been provided.'
	end
	return nil, string.format('No pretty name for `%s.', tag)
end

---
-- Just make a string print table in HTML.
-- @function [parent = #docutils] securechevrons
-- @param #string String to convert.
-- @usage securechevrons('<markup>') => '&lt;markup&gt;'
-- @return #string Converted string.
function M.securechevrons( str )
	if not str then return nil, 'String expected.' end
	return string.gsub(str:gsub('<', '&lt;'), '>', '&gt;')
end

-------------------------------------------------------------------------------
-- Handling format of @{some#type} tag.
-- Following functions enable to recognize several type of references between
-- "{}".
-------------------------------------------------------------------------------

---
-- Provide API Model elements from string describing global elements
-- such as:
-- * `global#foo`
-- * `foo#global.bar`
local globals = function(str)
	-- Handling globals from modules
	for modulename, fieldname in str:gmatch('([%a%.%d_]+)#global%.([%a%.%d_]+)') do
		local item = apimodel._item(fieldname)
		local file = apimodel._file()
		file.name = modulename
		file:addglobalvar( item )
		return item
	end
	-- Handling other globals
	for name in str:gmatch('global#([%a%.%d_]+)') do
		--	print("globale", name)
		return apimodel._externaltypref('global', name)
	end
	return nil
end

---
-- Transform a string like `module#(type).field` in an API Model item
local field = function( str )

	-- Match `module#type.field`
	local mod, typename, fieldname = str:gmatch('([%a%.%d_]*)#([%a%.%d_]+)%.([%a%.%d_]+)')()

	-- Try matching `module#(type).field`
	if not mod then
		mod, typename, fieldname = str:gmatch('([%a%.%d_]*)#%(([%a%.%d_]+)%)%.([%a%.%d_]+)')()
		if not mod then
			-- No match
			return nil
		end
	end

	-- Build according `item
	local modulefielditem = apimodel._item( fieldname )
	local moduletype = apimodel._recordtypedef(typename)
	moduletype:addfield( modulefielditem )
	local typeref
	if #mod > 0 then
		local modulefile = apimodel._file()
		modulefile.name = mod
		modulefile:addtype( moduletype )
		typeref = apimodel._externaltypref(mod, typename)
		modulefielditem.external = true
	else
		typeref = apimodel._internaltyperef(typename)
	end
	modulefielditem.type = typeref
	return modulefielditem
end

---
-- Build an API internal reference from a string like: `#typeref`
local internal = function ( typestring )
	for name in typestring:gmatch('#([%a%.%d_]+)') do
		-- Do not handle this name is it starts with reserved name "global"
		if name:find("global.") == 1 then return nil end
		return apimodel._internaltyperef(name)
	end
	return nil
end

---
-- Build an API external reference from a string like: `mod.ule#type`
local extern = function (type)

	-- Match `mod.ule#ty.pe`
	local modulename, typename = type:gmatch('([%a%.%d_]+)#([%a%.%d_]+)')()

	-- Trying  `mod.ule#(ty.pe)`
	if not modulename then
		modulename, typename = type:gmatch('([%a%.%d_]+)#%(([%a%.%d_]+)%)')()

		-- No match at all
		if not modulename then
			return nil
		end
	end
	return apimodel._externaltypref(modulename, typename)
end

---
-- Build an API external reference from a string like: `mod.ule`
local file = function (type)
	for modulename in type:gmatch('([%a%.%d_]+)') do
		local file = apimodel._file()
		file.name = modulename
		return file
	end
	return nil
end


---
-- Provide API Model element from a string
-- @usage local externaltyperef = getelement("somemodule#somefield")
function M.getelement( str )

	-- Order matters, more restrictive are at begin of table
	local extractors = {
		globals,
		field,
		extern,
		internal,
		file
	}
	-- Loop over extractors.
	-- First valid result is used
	for _, extractor in ipairs( extractors ) do
		local result = extractor( str )
		if result then return result end
	end
	return nil
end

--------------------------------------------------------------------------------
-- Iterator that iterates on the table in key ascending order.
--
-- @function [parent=#utils.table] sortedPairs
-- @param t table to iterate.
-- @return iterator function.
function M.sortedpairs(t)
	local a = {}
	local insert = table.insert
	for n in pairs(t) do insert(a, n) end
	table.sort(a)
	local i = 0
	return function()
		i = i + 1
		return a[i], t[a[i]]
	end
end
return M
