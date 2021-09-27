local inspect = require 'inspect'
local CPed = require 'cped'

local Mod = {}

function Mod.initMod( api )
	addBooleanSetting("Mouse rotation", true)
end

function Mod.tickPre( dt, api )
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
	
	local x, y, width, height = getCursorPos()
	print("getCursorPos", x, y, width, height)

	-- fix aspect ratio
	local aspect = width / height
	print("apect ratio", aspect)
	local mouseAngle = math.deg(math.atan2(x-0.5, (y-0.5) / aspect)) - 90.0

	print("mouseAngle", mouseAngle)
	ped:setRotation(mouseAngle)
end

return Mod
