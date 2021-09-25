local ffi = require("ffi")
local inspect = require 'inspect'
local defs = require 'gta2-def'
local CPed = require 'cped'

print("gta2.lua")

ffi.cdef[[

struct UISettings {
    bool open;
    bool do_show_cycles; 
    bool do_show_physics; 
    bool do_show_ids; 
    bool do_free_shopping;
    bool fast_cars;
    bool flamethrower;
    bool set_cop_level;
    int copLevel;
};

]]

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
        if mod.tickPre ~= nil then
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
	
    -- 
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