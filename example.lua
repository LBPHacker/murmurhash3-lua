local mmh3 = require "murmurhash3"

local seed = 0 -- (optional)

local data = "payload"

local hash = mmh3.hash32(data, seed)

print(hash)

local arr = {}
for ix = 1, #data do
	table.insert(arr, data:byte(ix))
end

local hash_array = mmh3.hash32_array(arr, seed)

print(hash_array)


