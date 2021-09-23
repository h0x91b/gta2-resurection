local inspect = require 'inspect'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
	print("initMod", inspect(api))
	api.test()
end

return Mod
