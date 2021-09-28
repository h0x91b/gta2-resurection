local inspect = require 'inspect'
local CPed = require 'cped'

local Mod = {}

function Mod.initMod( api )
	addBooleanSetting("Mouse rotation", true)
end

local prevAttack = false

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
    -- print("rotation", rotation, originalRotation)
	
	local x, y, width, height = getCursorPos()
	print("getCursorPos", x, y, width, height)

	-- fix aspect ratio
	local aspect = width / height
	-- print("apect ratio", aspect)
	local mouseAngle = math.deg(math.atan2(x-0.5, (y-0.5) / aspect)) - 90.0

	print("mouseAngle", mouseAngle)
	ped:setRotation(mouseAngle)

end

function bitand(a, b)
    local result = 0
    local bitval = 1
    while a > 0 and b > 0 do
      if a % 2 == 1 and b % 2 == 1 then -- test the rightmost bits
          result = result + bitval      -- set the current bit
      end
      bitval = bitval * 2 -- shift left
      a = math.floor(a/2) -- shift right
      b = math.floor(b/2)
    end
    return result
end

function Mod.setKeyState( keys, api )
	local ped = CPed:new(1)
	if ped == nil or not getSetting("Mouse rotation") or ped.car ~= nil then
        return keys
    end
	if api.IsLeftMouseDown then
		if bitand(keys, 0x10) == 0 then
			keys = keys + 0x10
		end
	end
	if api.IsRightMouseDown then
		if bitand(keys, 0x80) == 0 then
			keys = keys + 0x80
		end
	end
	return keys
end


return Mod
