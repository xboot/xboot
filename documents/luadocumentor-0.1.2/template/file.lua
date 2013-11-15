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
return[[#
<div id="content">
# --
# -- Module name
# --
# if _file.name then
   <h$(i)>Module <code>$(_file.name)</code></h$(i)>
# end
# --
# -- Descriptions
# --
# if _file.shortdescription then
   $( format(_file.shortdescription) )
# end
# if _file.description and #_file.description > 0 then
   $( format(_file.description) )
# end
# --
# -- Handle "@usage" special tag
# --
#if _file.metadata and _file.metadata.usage then
	$( applytemplate(_file.metadata.usage, i+1) )
#end
# --
# -- Show quick description of current type
# --
# 
# -- show quick description for globals
# if not isempty(_file.globalvars) then
	<h$(i+1)>Global(s)</h$(i+1)>
	<table class="function_list">
#	for _, item in sortedpairs(_file.globalvars) do
		<tr>
		<td class="name" nowrap="nowrap">$( fulllinkto(item) )</td>
		<td class="summary">$( format(item.shortdescription) )</td>
		</tr>
# 	end
	</table>
# end
#
# -- get type corresponding to this file (module)
# local currenttype 
# local typeref = _file:moduletyperef()
# if typeref and typeref.tag == "internaltyperef" then 
#	local typedef = _file.types[typeref.typename]
#	if typedef and typedef.tag == "recordtypedef" then
#		currenttype = typedef 
#	end
# end
#
# -- show quick description type exposed by module
# if currenttype and not isempty(currenttype.fields) then
	<h$(i+1)><a id="$(anchor(currenttype))" >Type <code>$(currenttype.name)</code></a></h$(i+1)>
	$( applytemplate(currenttype, i+2, 'index') )
# end
# --
# -- Show quick description of other types
# --
# if _file.types then
#	for name, type in sortedpairs( _file.types ) do
#		if type ~= currenttype and type.tag == 'recordtypedef' and not isempty(type.fields) then
			<h$(i+1)><a id="$(anchor(type))">Type <code>$(name)</code></a></h$(i+1)>
			$( applytemplate(type, i+2, 'index') )
#		end
#	end
# end
# --
# -- Long description of globals
# --
# if not isempty(_file.globalvars) then
	<h$(i+1)>Global(s)</h$(i+1)>
#	for name, item in sortedpairs(_file.globalvars) do
		$( applytemplate(item, i+2) )
#	end
# end
# --
# -- Long description of current type
# --
# if currenttype then
	<h$(i+1)><a id="$(anchor(currenttype))" >Type <code>$(currenttype.name)</code></a></h$(i+1)>
	$( applytemplate(currenttype, i+2) )
# end
# --
# -- Long description of other types
# --
# if not isempty( _file.types ) then
#	for name, type in sortedpairs( _file.types ) do
#		if type ~= currenttype  and type.tag == 'recordtypedef' then
			<h$(i+1)><a id="$(anchor(type))" >Type <code>$(name)</code></a></h$(i+1)>
			$( applytemplate(type, i+2) )
#		end
#	end
# end
</div>
]]
