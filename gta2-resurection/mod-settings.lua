local inspect = require 'inspect'
local CPed = require 'cped'
local ffi = require 'ffi'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
	addBooleanSetting("Show cycles", false)
	addBooleanSetting("Show physics", false)
	addBooleanSetting("Show id's", false)
	addBooleanSetting("Show traffic lights info", false)
	addBooleanSetting("Free shoping", false)
	addBooleanSetting("Get flamethrower", false)
	addBooleanSetting("Get SMG", false)
	addBooleanSetting("Get pistol", false)
	addBooleanSetting("Get RPG", false)
	addBooleanSetting("Get molotov", false)
	addBooleanSetting("Get grenade", false)
end

function Mod.tick( dt, api )
    -- print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end

	ffi.cast('bool*', 0x005eada7)[0] = getSetting("Show cycles")
    ffi.cast('bool*', 0x005ead85)[0] = getSetting("Show physics")
    ffi.cast('bool*', 0x005eada1)[0] = getSetting("Show id's")
    ffi.cast('bool*', 0x005ead96)[0] = getSetting("Show traffic lights info")

    local s9 = ffi.cast('struct S9_cars**', 0x005e4ca4)
    s9[0].do_free_shopping = getSetting("Free shoping")
	local pGame = ffi.cast('struct Game**', 0x005eb4fc)

	if getSetting("Get flamethrower") then
		local WEAPON_FLAMETHROWER = 8
		pGame[0].player.weapons[WEAPON_FLAMETHROWER].ammo = 100
	end

	if getSetting("Get SMG") then
		local WEAPON_SMG = 1
		pGame[0].player.weapons[WEAPON_SMG].ammo = 100
	end

	if getSetting("Get pistol") then
		local WEAPON_PISTOL = 0
		pGame[0].player.weapons[WEAPON_PISTOL].ammo = 100
	end
	
	if getSetting("Get RPG") then
		local WEAPON_RPG = 2
		pGame[0].player.weapons[WEAPON_RPG].ammo = 100
	end
	
	if getSetting("Get molotov") then
		local WEAPON_MOLOTOV = 4
		pGame[0].player.weapons[WEAPON_MOLOTOV].ammo = 100
	end
	
	if getSetting("Get grenade") then
		local WEAPON_GRENADE = 5
		pGame[0].player.weapons[WEAPON_GRENADE].ammo = 100
	end

end

return Mod
