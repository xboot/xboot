---
-- Builtin module
--
Json = require "builtin.json"
pump = require("builtin.event").pump
Stopwatch = require "builtin.stopwatch"
Base64 = require "builtin.base64"
Matrix = require "builtin.matrix"
Easing = require "builtin.easing"
Object = require "builtin.object"
Pattern = require "builtin.pattern"
Texture = require "builtin.texture"
Ninepatch = require "builtin.ninepatch"
Shape = require "builtin.shape"
Font = require "builtin.font"
Display = require "builtin.display"

---
-- External event module
--
Event = require "org.xboot.event.Event"
EventDispatcher = require "org.xboot.event.EventDispatcher"

---
-- External display module
--
DisplayObject = require "org.xboot.display.DisplayObject"
DisplayImage = require "org.xboot.display.DisplayImage"
DisplayImageMask = require "org.xboot.display.DisplayImageMask"
DisplayNinepatch = require "org.xboot.display.DisplayNinepatch"
DisplayShape = require "org.xboot.display.DisplayShape"
DisplayText = require "org.xboot.display.DisplayText"

---
-- External core module
--
Assets = require "org.xboot.core.Assets"
TexturePacker = require "org.xboot.core.TexturePacker"
Application = require "org.xboot.core.Application"
Stage = require "org.xboot.core.Stage"

---
-- External timer module
--
Timer = require "org.xboot.timer.Timer"
TimerManager = require "org.xboot.timer.TimerManager"

---
-- External widget module
--
Widget = {
	Button = require "org.xboot.widget.Button",
	CheckBox = require "org.xboot.widget.CheckBox",
	RadioButton = require "org.xboot.widget.RadioButton",
	Stepper = require "org.xboot.widget.Stepper",
	Slider = require "org.xboot.widget.Slider",
}

application = Application.new()

require("main")
return true

