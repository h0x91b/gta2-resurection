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

return CPed
