local Pattern = Pattern

local function D(name)
	return ("assets/themes/default/" .. name)
end

local common = {
	fontFamily = "roboto-regular",
	fontSize = 24,
	textPatternNormal = { red = 0, green = 0, blue = 0, alpha = 1 },
	textPatternPressed = { red = 0.5, green = 0.5, blue = 0.1, alpha = 1 },
	textPatternDisabled = { red = 0.4, green = 0.4, blue = 0.4, alpha = 1 },
}

return {
	button = {
		image = {
			normal = D("button-normal.9.png"),
			pressed = D("button-pressed.9.png"),
			disabled = D("button-disabled.9.png"),
		},
		text = {
			fontFamily = "roboto-regular",
			fontSize = 24,
			color = {
				normal = { red = 0, green = 0, blue = 0, alpha = 1 },
				pressed = { red = 0.5, green = 0.5, blue = 0.1, alpha = 1 },
				disabled = { red = 0.4, green = 0.4, blue = 0.4, alpha = 1 },
			},
			margin = {
				left = 0,
				top = 0,
				right = 0,
				bottom = 0,
			},
		},
	},

	checkbox = {
		image = {
			onNormal = D("checkbox-on-normal.png"),
			onPressed = D("checkbox-on-pressed.png"),
			onDisabled = D("checkbox-on-disabled.png"),
			offNormal = D("checkbox-off-normal.png"),
			offPressed = D("checkbox-off-pressed.png"),
			offDisabled = D("checkbox-off-disabled.png"),
		},
		text = {
			fontFamily = "roboto-regular",
			fontSize = 24,
			color = {
				normal = { red = 0, green = 0, blue = 0, alpha = 1 },
				pressed = { red = 0.5, green = 0.5, blue = 0.1, alpha = 1 },
				disabled = { red = 0.4, green = 0.4, blue = 0.4, alpha = 1 },
			},
			margin = {
				left = 6,
				top = 2,
				right = 6,
				bottom = 2,
			},
		},
	},

	radiobutton = {
		image = {
			onNormal = D("radiobutton-on-normal.png"),
			onPressed = D("radiobutton-on-pressed.png"),
			onDisabled = D("radiobutton-on-disabled.png"),
			offNormal = D("radiobutton-off-normal.png"),
			offPressed = D("radiobutton-off-pressed.png"),
			offDisabled = D("radiobutton-off-disabled.png"),
		},
		text = {
			fontFamily = "roboto-regular",
			fontSize = 24,
			color = {
				normal = { red = 0, green = 0, blue = 0, alpha = 1 },
				pressed = { red = 0.5, green = 0.5, blue = 0.1, alpha = 1 },
				disabled = { red = 0.4, green = 0.4, blue = 0.4, alpha = 1 },
			},
			margin = {
				left = 6,
				top = 2,
				right = 6,
				bottom = 2,
			},
		},
	},

	stepper = {
		imageMinusNormal = D("stepper-minus-normal.png"),
		imageMinusPressed = D("stepper-minus-pressed.png"),
		imageMinusDisabled = D("stepper-minus-disabled.png"),
		imagePlusNormal = D("stepper-plus-normal.png"),
		imagePlusPressed = D("stepper-plus-pressed.png"),
		imagePlusDisabled = D("stepper-plus-disabled.png"),
	},

	slider = {
		imageTrack = D("slider-track.9.png"),
		imageBackground = D("slider-background.9.png"),
		imageThumbNormal = D("slider-thumb-normal.png"),
		imageThumbPressed = D("slider-thumb-pressed.png"),
		imageThumbDisabled = D("slider-thumb-disabled.png"),
	},
}
