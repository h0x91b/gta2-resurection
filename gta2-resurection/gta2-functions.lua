local ffi = require("ffi")
local inspect = require 'inspect'

local funcs = {}

funcs.fnGetPedByID = ffi.cast('GetPedById', 0x0043ae10)

return funcs
