local inspect = require 'inspect'
local CPed = require 'cped'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
	print("initMod", inspect(api))
	api.test()
end

function Mod.tick( dt, api )
    print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end
	print('ped.health', ped.health)
	ped.obj.health = 100
end

return Mod
