local function D(name)
	return ("assets/themes/default/" .. name)
end

return {
	button = {
		imageUp = D("button-up.png"),
		imageDown = D("button-down.png"),
	},

	checkbox = {
		imageChecked = D("checkbox-checked.png"),
		imageUnchecked = D("checkbox-unchecked.png"),
	},

	radiobutton = {
		imageChecked = D("radiobutton-checked.png"),
		imageUnchecked = D("radiobutton-unchecked.png"),
	},
}
