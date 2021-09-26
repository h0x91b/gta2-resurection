local inspect = require 'inspect'
local CPed = require 'cped'
local ffi = require 'ffi'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
	addBooleanSetting("Fast Cars", false)
end

function Mod.tick( dt, api )
    -- print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end
	if ped.car ~= nil and getSetting("Fast Cars") then
	 	print("car: id ", ped.car.id, ped.car.type)
		ped.obj.car.type = 19
	end
end

return Mod
