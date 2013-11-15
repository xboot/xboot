--------------------------------------------------------------------------------
--  Copyright (c) 2011-2012 Sierra Wireless.
--  All rights reserved. This program and the accompanying materials
--  are made available under the terms of the Eclipse Public License v1.0
--  which accompanies this distribution, and is available at
--  http://www.eclipse.org/legal/epl-v10.html
--
--  Contributors:
--       Simon BERNARD <sbernard@sierrawireless.com>
--           - initial API and implementation and initial documentation
--------------------------------------------------------------------------------
local M = {}

--------------------------------------------------------------------------------
-- API MODEL
--------------------------------------------------------------------------------

function M._file()
	local file = {
		-- FIELDS
		tag              = "file",
		name             = nil,    -- string
		shortdescription = nil,    -- string
		description      = nil,    -- string
		types            = {},     -- map from typename to type
		globalvars       = {},     -- map from varname to item
		returns          = {},     -- list of return

		-- FUNCTIONS
		addtype =  function (self,type)
			self.types[type.name] = type
			type.parent = self
		end,
		
		addglobalvar =  function (self,item)
			self.globalvars[item.name] = item
			item.parent = self
		end,
		
		moduletyperef = function (self)
			if self and self.returns[1] and self.returns[1].types[1] then
				local typeref = self.returns[1].types[1]
				return typeref
			end
		end		
	}
	return file
end

function M._recordtypedef(name)
	local recordtype = {
		-- FIELDS
		tag              = "recordtypedef",
		name             = name,            -- string (mandatory)
		shortdescription = nil,             -- string
		description      = nil,             -- string
		fields           = {},              -- map from fieldname to field
		sourcerange      = {min=0,max=0},

		-- FUNCTIONS
		addfield = function (self,field)
			self.fields[field.name] = field
			field.parent = self
		end
	}
	return recordtype
end

function M._functiontypedef(name)
	return {
		tag              = "functiontypedef",
		name             = name,              -- string (mandatory)
		shortdescription = nil,               -- string
		description      = nil,               -- string
		params           = {},                -- list of parameter
		returns          = {}                 -- list of return
	}
end

function M._parameter(name)
	return {
		tag         = "parameter",
		name        = name, -- string (mandatory)
		description = "",   -- string
		type        = nil   -- typeref (external or internal or primitive typeref)
	}
end

function M._item(name)
	return {
		-- FIELDS
		tag              = "item",
		name             = name,   -- string (mandatory)
		shortdescription = "",     -- string
		description      = "",     -- string
		type             = nil,    -- typeref (external or internal or primitive typeref)
		occurrences      = {},     -- list of identifier (see internalmodel)
		sourcerange      = {min=0, max=0},

		-- This is A TRICK
		-- This value is ALWAYS nil, except for internal purposes (short references).
		external         = nil,

		-- FUNCTIONS
		addoccurence = function (self,occ)
			table.insert(self.occurrences,occ)
			occ.definition = self
		end,
		
		resolvetype = function (self)
			if self and self.type and self.type.tag =="internaltyperef" then
				if self.parent.tag == 'recordtypedef' then
					local file = self.parent.parent
					return file.types[ self.type.typename ]
				elseif self.parent.tag == 'file' then
					return self.parent.types[ self.type.typename ]
				end
			end	
		end
	}
end

function M._externaltypref(modulename, typename)
	return {
		tag        = "externaltyperef",
		modulename = modulename,        -- string
		typename   =  typename          -- string
	}
end

function M._internaltyperef(typename)
	return {
		tag      = "internaltyperef",
		typename =  typename          -- string
	}
end

function M._primitivetyperef(typename)
	return {
		tag      = "primitivetyperef",
		typename =  typename           -- string
	}
end

function M._moduletyperef(modulename,returnposition)
	return {
		tag            = "moduletyperef",
		modulename     = modulename,      -- string
		returnposition = returnposition   -- number
	}
end

function M._exprtyperef(expression,returnposition)
	return {
		tag            = "exprtyperef",
		expression     =  expression,   -- expression (see internal model)
		returnposition = returnposition -- number
	}
end

function M._return(description)
	return {
		tag         = "return",
		description =  description or "", -- string
		types       = {}                  -- list of typref (external or internal or primitive typeref)
	}
end
return M
