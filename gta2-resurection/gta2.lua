local ffi = require("ffi")
local inspect = require 'inspect'
local defs = require 'gta2-def'
local CPed = require 'cped'

print("gta2.lua")

local API = {}

function API.test( abc )
   print("API.test", abc)	
end

function API.getSettings( )
	local settings = ffi.cast('struct UISettings*', getSettings())
    return settings
end

-- init
local availableMods = listMods()
local loadedMods = {}

-- API
function loadMod( modFile )
	print("loadMod", modFile)
    local mod = require(string.gsub(modFile, ".lua", ""))
    if type(mod) ~= "table" then
        print("Error, mod must return a table")
        return false
    end

    if mod.initMod == nil then
        print("Error, mod must return a table with 'initMod' function")
        return false
    end

    mod.initMod(API)

    table.insert(loadedMods, mod)

    return true
end

-- Load mods

print("listMods", inspect(availableMods))

for i = 1, #availableMods do
    loadMod(availableMods[i])
end

local health = 10

-- static GetPedById fnGetPedByID = (GetPedById)0x0043ae10;
local fnGetPedByID = ffi.cast('GetPedById', 0x0043ae10)
local pGame = nil
print("UISettings")
local settings = ffi.cast('struct UISettings*', getSettings())
print("Ready")

function gameTickPre(dt)
    print("gameTickPre")

    for i = 1, #loadedMods do
        local mod = loadedMods[i]
        if mod.tick ~= nil then
            mod.tickPre(dt, API)
        end
    end

    -- local p = ffi.cast('int*', 0x005eb4fc)
    -- if p[0] == 0 then
    --     return
    -- end
    -- 
    -- pGame = ffi.cast('struct Game*', 0x005eb4fc)
    -- 
    -- local pp = CPed:new(1)
    -- print("pp ", inspect(pp))
    -- print("pp pp:getHealth()", pp:getHealth())
    -- print("pp pp.health", pp.health)
    -- 
	-- -- health = health + 10
	-- -- if health > 100 then
	-- -- 	health = 10
	-- -- end
	-- -- print("tick1", dt, health)
    -- local p = fnGetPedByID(1)
    -- if p == nil then
    --     return
    -- end
	-- local ped = ffi.cast('struct Ped*', p)
    -- ped.health = 100
	-- print("health: ", ped.health)
	-- if ped.car ~= nil then
	-- 	print("car: id ", ped.car.id, ped.car.type)
	-- 	-- ped.car.type = 53
	-- end
    -- 
    -- -- ped.copLevel = 0
    -- 
    -- -- print("settings.do_show_cycles", tostring(settings.do_show_cycles))
    -- ffi.cast('bool*', 0x005eada7)[0] =  settings.do_show_cycles
    -- ffi.cast('bool*', 0x005ead85)[0] =  settings.do_show_physics
    -- ffi.cast('bool*', 0x005eada1)[0] =  settings.do_show_ids
    -- -- COP_STAR_6=12000,     COP_STAR_3=3000,     COP_STAR_2=1600,     COP_STAR_5=8000,     COP_STAR_4=5000,     COP_STAR_1=600,     COP_STAR_0=0
    -- 
    -- print("copLevel", settings.copLevel, ped.copLevel)
    -- if settings.copLevel == 0 then
    --     ped.copLevel = 0
    -- elseif settings.copLevel == 1 then
    --     ped.copLevel = 600
    -- elseif settings.copLevel == 2 then
    --     ped.copLevel = 1600
    -- elseif settings.copLevel == 3 then
    --     ped.copLevel = 3000
    -- elseif settings.copLevel == 4 then
    --     ped.copLevel = 5000
    -- elseif settings.copLevel == 5 then
    --     ped.copLevel = 8000
    -- elseif settings.copLevel == 6 then
    --     ped.copLevel = 12000
    -- end
    -- 
    -- -- 1)
    -- -- setCopLevel(pedId, level)
    -- -- 
    -- -- 2)
    -- -- ped = getPedById(1)
    -- -- ped.setCopLevel(level)
    -- 
    -- --ffi.cast('bool*', 0x005ead84)[0] =  settings.do_free_shopping
    -- --ffi.cast('bool*', 0x005e4ca4 + 0x69)[0] =  settings.do_free_shopping
    -- local s9 = ffi.cast('struct S9_cars**', 0x005e4ca4)
    -- --if s9 ~= nil then
    --     print("s9.do_free_shopping", tostring(s9[0].do_free_shopping))
    --     s9[0].do_free_shopping = settings.do_free_shopping 
    --     
    --     -- but below works well...
    --     --print("s9.do_free_shopping", tostring(ffi.cast('bool*', ffi.cast('uint*', 0x005e4ca4)[0]+0x69)[0]))
    --     --ffi.cast('bool*', ffi.cast('uint*', 0x005e4ca4)[0]+0x69)[0] = settings.do_free_shopping
    -- --end
end

function gameTick(dt)
    print("gameTick")

    for i = 1, #loadedMods do
        local mod = loadedMods[i]
        if mod.tick ~= nil then
            mod.tick(dt, API)
        end
    end
end