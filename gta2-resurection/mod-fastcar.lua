local inspect = require 'inspect'
local CPed = require 'cped'
local ffi = require 'ffi'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
end

function Mod.tick( dt, api )
    print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end
	local settings = api.getSettings()
	if ped.car ~= nil and settings.fast_cars then
	 	print("car: id ", ped.car.id, ped.car.type)
		ped.obj.car.type = 53
	end
end

return Mod
