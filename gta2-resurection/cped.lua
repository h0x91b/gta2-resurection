local ffi = require("ffi")
local inspect = require 'inspect'
local gta = require 'gta2-functions'

CPed = {}

function CPed:new(id)
    local o = {}
    setmetatable(o, self)
    o.id = id
    o._self = self
    local p = gta.fnGetPedByID(id)
    if p == nil then
        return nil
    end
	o.obj = ffi.cast('struct Ped*', p)
    o.__index = struct_index
    return o
end

function CPed:__index( index )
	-- print ("__index", index)
    if self._self[index] ~= nil then
        return self._self[index]
    end
    return self.obj[index]
end

-- function CPed:__newindex( index, value )
--     print ("__newindex", index, value)
--     if self.ped[index] ~= nil then
--         self.ped[index] = value
--         return
--     end
--     self._self[index] = value
-- end

function CPed:getHealth()
    return self.obj.health
end

function CPed:getRotation()
    local rot = self.obj.pedS1.rotation
    local normalized = math.fmod((rot / 4 - 90), 360)
    if normalized < 0 then
        normalized = normalized + 360
    end
	return normalized,rot
end

function CPed:setRotation(angle)
    local denormalized = math.fmod(angle, 360)
    while denormalized < 0 do
        denormalized = denormalized + 360
    end
    denormalized = (denormalized + 90) * 4
    self.obj.pedS1.rotation = denormalized
end

return CPed
