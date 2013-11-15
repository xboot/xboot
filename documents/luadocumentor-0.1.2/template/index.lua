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
[[#
#if _index.modules then
<div id="content">
<h2>Module$( #_index.modules > 1 and 's' )</h2>
	<table class="module_list">
#	for _, module in sortedpairs( _index.modules ) do
#		if module.tag ~= 'index' then
			<tr>
			<td class="name" nowrap="nowrap">$( fulllinkto(module) )</td>
			<td class="summary">$( module.description and format(module.shortdescription) )</td>
			</tr>
#		end
#	end
	</table>
</div>
#end ]]
