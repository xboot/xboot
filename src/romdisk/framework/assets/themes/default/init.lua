local function D(name)
	return ("assets/themes/default/" .. name)
end

return {
	checkbox = {
		imageChecked = D("checkbox-checked.png"),
		imageUnchecked = D("checkbox-unchecked.png"),
	},
	
	radiobutton = {
		imageChecked = D("radiobutton-checked.png"),
		imageUnchecked = D("radiobutton-unchecked.png"),
	},
}
