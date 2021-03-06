local inspect = require 'inspect'
local CPed = require 'cped'

print("mod hello world")

local Mod = {}

function Mod.initMod( api )
	-- print("initMod", inspect(api))
	addBooleanSetting("Set cop level", true)
    addSliderSetting("Cop level", 0, 0, 6)
end

-- local copLevel = 0
-- 
-- function Mod.tickPre( dt, api )
--     print("Mod.tickPre", dt, api)
-- 	local ped = CPed:new(1)
-- 	if ped == nil then
-- 		return 
-- 	end
-- 	local settings = api.getSettings()
-- 
--     if ped.obj.copLevel == copLevel then
--         return
--     end
-- 
-- 	if ped.obj.copLevel == 0 then
--         settings.copLevel = 0
--     elseif ped.obj.copLevel == 600 then
--         settings.copLevel = 1
--     elseif ped.obj.copLevel == 1600 then
--         settings.copLevel = 2
--     elseif ped.obj.copLevel == 3000 then
--         settings.copLevel = 3
--     elseif ped.obj.copLevel == 5000 then
--         settings.copLevel = 4
--     elseif ped.obj.copLevel == 8000 then
--         settings.copLevel = 5
--     elseif ped.obj.copLevel == 12000 then
--         settings.copLevel = 6
--     end
-- 
--     copLevel = settings.copLevel
-- end

function Mod.tick( dt, api )
    -- print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end
	local settings = api.getSettings()

    if not getSetting("Set cop level") then
        return
    end

    local copLevel = getSetting("Cop level")
	if copLevel == 0 then
        ped.obj.copLevel = 0
    elseif copLevel == 1 then
        ped.obj.copLevel = 600
    elseif copLevel == 2 then
        ped.obj.copLevel = 1600
    elseif copLevel == 3 then
        ped.obj.copLevel = 3000
    elseif copLevel == 4 then
        ped.obj.copLevel = 5000
    elseif copLevel == 5 then
        ped.obj.copLevel = 8000
    elseif copLevel == 6 then
        ped.obj.copLevel = 12000
    end

end

return Mod
