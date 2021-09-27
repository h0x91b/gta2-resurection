local inspect = require 'inspect'
local CPed = require 'cped'

local Mod = {}

function Mod.initMod( api )
	addBooleanSetting("Mouse rotation", true)
end

function Mod.tick( dt, api )
    -- print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end

    if not getSetting("Mouse rotation") or ped.car ~= nil then
        return
    end

	local rotation, originalRotation = ped:getRotation()
    print("rotation", rotation, originalRotation)
	rotation = rotation - 5
	ped:setRotation(rotation)
end

return Mod
