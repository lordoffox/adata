local adata = require("adata_core")
local player_adl = require("player_adl")
local quest_adl = require("quest_adl")
local tablen = require("adata").tablen

-- vec3 comparer
local vec3_equals = function(lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  return 
  	lhs.x == rhs.x and
  	lhs.y == rhs.y and
  	lhs.z == rhs.z
end

-- item comparer
local item_equals = function(lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  return 
  	lhs.id == rhs.id and
  	lhs.level == rhs.level and
  	lhs.type == rhs.type
end

-- quest comparer
quest_equals = function(lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  return 
  	lhs.id == rhs.id and
  	lhs.name == rhs.name and
  	lhs.description == rhs.description
end

-- player comparer
local player_cmp = {
	equals_list,
	equals,
	equals_v2
}

local array_equals = function(cmp, lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  local res = true
  if tablen(lhs) == tablen(rhs) then
  	for i,v in ipairs(lhs) do
  	  if not cmp(rhs[i], v) then 
  	  	res = false 
  	  	break 
  	  end
  	end
  else
  	res = false
  end
  return res
end

local player_equals = function(lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  return 
  	lhs.id == rhs.id and
  	lhs.name == rhs.name and 
  	lhs.age == rhs.age and
  	lhs.factor == rhs.factor and
  	vec3_equals(lhs.pos, rhs.pos) and
  	array_equals(
  	  item_equals, lhs.inventory, rhs.inventory
  	  ) and
  	array_equals(
  	  quest_equals, lhs.quests, rhs.quests
  	  )
end

local player_equals_v2 = function(lhs, rhs)
  if lhs == rhs then return true end
  if lhs == nil or rhs == nil then return false end

  return 
  	lhs.id == rhs.id and
  	lhs.name == rhs.name and 
  	vec3_equals(lhs.pos, rhs.pos) and
  	array_equals(
  	  item_equals, lhs.inventory, rhs.inventory
  	  ) and
  	array_equals(
  	  quest_equals, lhs.quests, rhs.quests
  	  )
end

local dump_player_v1 = function(pv1)
  print(pv1.id, pv1.name, pv1.age, pv1.factor)
  print(pv1.pos.x, pv1.pos.y, pv1.pos.z)
  print("inventory: ")
  for i,v in ipairs(pv1.inventory) do
    print("  ", v.id, v.type, v.level)
  end
end

local dump_player_v2 = function(pv2)
  print(pv2.id, pv2.name)
  print(pv2.pos.x, pv2.pos.y, pv2.pos.z)
  print("inventory: ")
  for i,v in ipairs(pv2.inventory) do
    print("  ", v.id, v.type, v.level)
  end
end

local ec = 0
local pv1 = player_adl.player_v1()
pv1.id = 1
pv1.age = 22
pv1.factor = 2.0
pv1.name = "pv1"

local itm = player_adl.item()
itm.id = 11
itm.level = 321110
itm.type = 3
table.insert(pv1.inventory, itm)
itm = player_adl.item()
itm.id = 12
table.insert(pv1.inventory, itm)

local qst = quest_adl.quest()
qst.id = 50
qst.name = "quest1"
qst.description = "There are something unusual..."
table.insert(pv1.quests, qst)

local buf_len = 4096
local stream = adata.new_buf(buf_len)

ec = pv1:write(stream)
if ec > 0 then 
  -- some error, print it
  error(adata.trace_info(stream))
end

local data = adata.get_write_data(stream)

-- deserialize
adata.set_read_data(stream, data)

local pv1_other = player_adl.player_v1()

ec = pv1_other:read(stream)
if ec > 0 then 
  -- some error, print it
  error(adata.trace_info(stream))
end

assert(player_equals(pv1, pv1_other))

adata.clear_buf(stream)

local pv2 = player_adl.player_v2()

-- forward compat (old data, new struct)
ec = pv2:read(stream)
if ec > 0 then 
  -- some error, print it
  error(adata.trace_info(stream))
end

--dump_player_v1(pv1)
--dump_player_v2(pv2)
assert(player_equals_v2(pv1, pv2))

adata.clear_buf(stream)
pv1 = player_adl.player_v1()

-- backward compat (new data, old struct)
pv2.id = 6543;
pv2.name = "pv2";
table.insert(pv2.friends, 2)
table.insert(pv2.friends, 100)

ec = pv2:write(stream)
if ec > 0 then 
  -- some error, print it
  error(adata.trace_info(stream))
end

data = adata.get_write_data(stream)
adata.set_read_data(stream, data)
ec = pv1:read(stream)
if ec > 0 then 
  -- some error, print it
  error(adata.trace_info(stream))
end

assert(player_equals_v2(pv1, pv2))

print("done.")
