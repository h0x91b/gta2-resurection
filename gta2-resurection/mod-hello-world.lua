local inspect = require 'inspect'
local CPed = require 'cped'
local cjson = require "cjson"
local pb = require "pb"
local protoc = require "protoc"

print("mod hello world")

local json = cjson.encode({
    foo = "bar",
    some_object = {},
    some_array = cjson.empty_array
})

print("json" .. json)


-- load schema from text (just for demo, use protoc.new() in real world)
assert(protoc:load [[
   message Phone {
      optional string name        = 1;
      optional int64  phonenumber = 2;
   }
   message Person {
      optional string name     = 1;
      optional int32  age      = 2;
      optional string address  = 3;
      repeated Phone  contacts = 4;
   } ]])

print("assert ok")

-- lua table data
local data = {
   name = "huy",
   age  = 13,
   contacts = {
      { name = "alice", phonenumber = 12312341234 },
      { name = "bob",   phonenumber = 45645674567 }
   }
}

-- encode lua table data into binary format in lua string and return
local bytes = assert(pb.encode("Person", data))
print(pb.tohex(bytes))

-- and decode the binary data back into lua table
local data2 = assert(pb.decode("Person", bytes))
print(inspect(data2))

local Mod = {}

function Mod.initMod( api )
	print("initMod", inspect(api))
	api.test()
end

function Mod.tick( dt, api )
    -- print("Mod.tick", dt, api)
	local ped = CPed:new(1)
	if ped == nil then
		return 
	end
	-- print('ped.health', ped.health)
	ped.obj.health = 100
end

return Mod
