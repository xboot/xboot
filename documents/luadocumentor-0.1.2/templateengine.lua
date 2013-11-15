--------------------------------------------------------------------------------
--  Copyright (c) 2011-2012 Sierra Wireless.
--  All rights reserved. This program and the accompanying materials
--  are made available under the terms of the Eclipse Public License v1.0
--  which accompanies this distribution, and is available at
--  http://www.eclipse.org/legal/epl-v10.html
--
--  Contributors:
--       Kevin KIN-FOO <kkinfoo@sierrawireless.com>
--           - initial API and implementation and initial documentation
--------------------------------------------------------------------------------
---
-- This library provide html description of elements from the externalapi
local M = {}

-- Load template engine
local pltemplate = require 'pl.template'

-- Markdown handling
local markdown = require 'markdown'

-- apply template to the given element
function M.applytemplate(elem, ident, templatetype)
	-- define environment
	local env = M.getenv(elem, ident)

	-- load template
	local template = M.gettemplate(elem,templatetype)
	if not template then
		templatetype = templatetype and string.format(' "%s"', templatetype) or ''
		local elementname = string.format(' for %s', elem.tag or 'untagged element')
		error(string.format('Unable to load %s template %s', templatetype, elementname))
	end

	-- apply template
	local str, err = pltemplate.substitute(template, env)

	--manage errors
	if not str then
		local templateerror = templatetype and string.format(' parsing "%s" template ', templatetype) or ''
		error(string.format('An error occured%s for "%s"\n%s',templateerror, elem.tag, err))
	end
	return str
end

-- get the a new environment for this element
function M.getenv(elem, ident)
	local currentenv ={}
	for k,v in pairs(M.env) do currentenv[k] = v end
	if elem and elem.tag then
		currentenv['_'..elem.tag]= elem
	end
	currentenv['i']= ident or 1
	return currentenv
end

-- get the template for this element
function M.gettemplate(elem,templatetype)
	local tag = elem and elem.tag
	if tag then
		if templatetype then
			return require ("template." .. templatetype.. "." .. tag)
		else
			return require ("template." .. tag)
		end
	end
end


---
-- Allow user to format text in descriptions.
-- Default implementation replaces @{---} tags with links and apply markdown.
-- @return #string 
local function format(string)
	-- Allow to replace encountered tags with valid links
	local replace = function(found)
		local apiobj = M.env.getelement(found)
		if apiobj then
			return M.env.fulllinkto(apiobj)
		end
		return found
	end
	string = string:gsub('@{%s*(.-)%s*}', replace)
	return M.env.markdown( string )
end
---
-- Provide a full link to an element using `prettyname` and `linkto`.
-- Default implementation is for HTML.
local function fulllinkto(o)
	local ref   = M.env.linkto(o)
	local name	= M.env.prettyname(o)
	if not ref then
		return name
	end
	return string.format('<a href="%s">%s</a>', ref, name)
end
--
-- Define default template environnement
--
local defaultenv = {
	table         = table,
	ipairs        = ipairs,
	pairs         = pairs,
	markdown      = markdown,
	applytemplate = M.applytemplate,
	format        = format,
	linkto        = function(str) return str end,
	fulllinkto    = fulllinkto,
	prettyname    = function(s) return s end,
	getelement    = function(s)	return nil end
}

-- this is the global env accessible in the templates
-- env should be redefine by docgenerator user to add functions or redefine it.
M.env = defaultenv
return M
