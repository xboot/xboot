--------------------------------------------------------------------------------
--  Copyright (c) 2012 Sierra Wireless.
--  All rights reserved. This program and the accompanying materials
--  are made available under the terms of the Eclipse Public License v1.0
--  which accompanies this distribution, and is available at
--  http://www.eclipse.org/legal/epl-v10.html
--
--  Contributors:
--       Marc AUBRY <maubry@sierrawireless.com>
--           - initial API and implementation
--------------------------------------------------------------------------------
return[[#
#--
#-- Show usage samples
#--
#if _usage then
#	if #_usage > 1 then
#		-- Show all usages
		<h$(i)>Usages:</h$(i)>
		<ul>
#		-- Loop over several usage description
#		for _, usage in ipairs(_usage) do
			<li><pre class="example"><code>$( securechevrons(usage.description) )</code></pre></li>
#		end
		</ul>
#	elseif #_usage == 1 then
#		-- Show unique usage sample
		<h$(i)>Usage:</h$(i)>
#		local usage = _usage[1]
		<pre class="example"><code>$( securechevrons(usage.description) )</code></pre>
#	end
#end
#]]
