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
return 
[[<dl class="function">
<dt>
# --
# -- Resolve item type definition
# --
# local typedef = _item:resolvetype()

# --
# -- Show item type for internal type
# --
#if _item.type and (not typedef or typedef.tag ~= 'functiontypedef') then
#	--Show link only when available 
#	local link = fulllinkto(_item.type)
#	if link then
		<em>$( link )</em>
#	else
		<em>$(prettyname(_item.type))</em>
#	end
#end
<a id="$(anchor(_item))" >
<strong>$( prettyname(_item) )</strong>
</a>
</dt>
<dd>
# if _item.shortdescription then
	$( format(_item.shortdescription) )
# end
# if _item.description and #_item.description > 0 then
	$( format(_item.description) )
# end
#
# --
# -- For function definitions, describe parameters and return values
# --
#if typedef and typedef.tag == 'functiontypedef' then
#	--
#	-- Describe parameters
#	--
#	local fdef = typedef
#
#	-- Adjust parameter count if first one is 'self'
#	local paramcount
#	if #fdef.params > 0 and fdef.params[1].name == 'self' then
#		paramcount = #fdef.params - 1
#	else
#		paramcount = #fdef.params
#	end
#
#	-- List parameters
#	if paramcount > 0 then
		<h$(i)>Parameter$( paramcount > 1 and 's' )</h$(i)>
		<ul>
#		for position, param in ipairs( fdef.params ) do
#			if not (position == 1 and param.name == 'self') then
				<li>
#				local paramline = "<code><em>"
#				if param.type then
#					local link = linkto( param.type )
#					local name = prettyname( param.type )
#					if link then
#						paramline = paramline .. '<a href=\"' .. link .. '\">' .. name .. "</a>"
#					else
#						paramline = paramline .. name
#					end
#				end
#
#				paramline = paramline .. " " .. param.name ..  " "
#
#				if param.optional then
#					paramline = paramline .. "optional" .. " "
#				end
#				if param.hidden then 
#					paramline = paramline .. "hidden"
#				end
#
# 				paramline = paramline .. "</em></code>: "
#
#				if param.description and #param.description > 0 then
#					paramline = paramline .. "\n" .. param.description
#				end
#
				$( format (paramline))
				</li>
#			end
#		end
		</ul>
#	end
#
#	--
#	-- Describe returns types
#	--
#	if fdef and #fdef.returns > 0 then
		<h$(i)>Return value$(#fdef.returns > 1 and 's')</h$(i)>
#		--
#		-- Format nice type list
#		--
#		local function niceparmlist( parlist )
#			local typelist = {}
#			for position, type in ipairs(parlist) do
#				local link = linkto( type )
#				local name = prettyname( type )
#				if link then
#					typelist[#typelist + 1] = '<a href="'..link..'">'..name..'</a>'
#				else
#					typelist[#typelist + 1] = name
#				end
#				-- Append end separator or separating comma 
#				typelist[#typelist + 1] = position == #parlist and ':' or ', '
#			end
#			return table.concat( typelist )
#		end
#		--
#		-- Generate a list if they are several return clauses
#		--
#		if #fdef.returns > 1 then
			<ol>
#			for position, ret in ipairs(fdef.returns) do
				<li>
#				local returnline = "";
#
#				local paramlist = niceparmlist(ret.types)
#				if #ret.types > 0 and #paramlist > 0 then
#					returnline = "<em>" .. paramlist .. "</em>"
#				end
#				returnline = returnline .. "\n" .. ret.description
				$( format (returnline))
				</li>
#			end
			</ol>
#		else
#			local paramlist = niceparmlist(fdef.returns[1].types)
#			local isreturn = fdef.returns and #fdef.returns > 0 and #paramlist > 0
#			local isdescription = fdef.returns and fdef.returns[1].description and #format(fdef.returns[1].description) > 0
#
# 			local returnline = "";
#			-- Show return type if provided
#			if isreturn then
#				returnline = "<em>"..paramlist.."</em>"
#			end
#			if isdescription then
#				returnline = returnline .. "\n" .. fdef.returns[1].description
#			end
				$( format(returnline))
#		end
#	end
#end
#
#--
#-- Show usage samples
#--
#if _item.metadata and _item.metadata.usage then
	$( applytemplate(_item.metadata.usage, i) )
#end
</dd>
</dl>]]
