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

	ffi.cast('bool*', 0x005eada7)[0] =  settings.do_show_cycles
    ffi.cast('bool*', 0x005ead85)[0] =  settings.do_show_physics
    ffi.cast('bool*', 0x005eada1)[0] =  settings.do_show_ids

    local s9 = ffi.cast('struct S9_cars**', 0x005e4ca4)
    s9[0].do_free_shopping = settings.do_free_shopping 
end

return Mod
