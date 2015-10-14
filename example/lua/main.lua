local adata = require('adata')
adata.load('game.adt')

local tablen = require('adata').tablen
local set_eq = function (o, f)
  local mt = getmetatable(o)
  if rawequal(mt, nil) then error('obj must have a metatable!') end
  mt.__eq = f
end

-- vec3 comparer
local vec3_equals = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  return 
    lhs.x == rhs.x and
    lhs.y == rhs.y and
    lhs.z == rhs.z
end

-- item comparer
local item_equals = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  return 
    lhs.id == rhs.id and
    lhs.level == rhs.level and
    lhs.type == rhs.type
end

-- quest comparer
quest_equals = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  return 
    lhs.id == rhs.id and
    lhs.name == rhs.name and
    lhs.description == rhs.description
end

local tabeq;
tabeq = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  local res = true
  if tablen(lhs) == tablen(rhs) then
    for k,v in pairs(lhs) do
      local ty = type(v)
      if ty ~= type(rhs[k]) then 
        res = false 
        break
      end

      if ty == 'table' or ty == 'userdata' then
        local mt = getmetatable(v)
        if mt == nil or mt.__eq == nil then
          if ty == 'table' then
            if not tabeq(v, rhs[k]) then
              res = false
              break
            end
          else
            if not (v == rhs[k]) then 
              res = false
              break
            end
          end
        else
          if ty == 'table' then
            if not tabeq(v, rhs[k]) then
              res = false
              break
            end
          elseif not (v == rhs[k]) then 
            res = false
            break 
          end
        end
      else
        if not (v == rhs[k]) then 
          res = false 
          break 
        end
      end
    end
  else
    res = false
  end
  return res
end

local player_equals = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  return 
    lhs.id == rhs.id and
    lhs.name == rhs.name and 
    lhs.age == rhs.age and
    lhs.factor == rhs.factor and
    tabeq(
      lhs.pos , rhs.pos
      ) and
    tabeq(
      lhs.inventory, rhs.inventory
      ) and
    tabeq(
      lhs.quests, rhs.quests
      )
end

local player_equals_v2 = function(lhs, rhs)
  if rawequal(lhs, nil) or rawequal(rhs, nil) then return false end
  if rawequal(lhs, rhs) then return true end

  return 
    lhs.id == rhs.id and
    lhs.name == rhs.name and 
    tabeq(
      lhs.pos , rhs.pos
      ) and
    tabeq(
      lhs.inventory, rhs.inventory
      ) and
    tabeq(
      lhs.quests, rhs.quests
      )
end
-- set all adl obj __eq
local tmp_vec3 = adata.types['util.vec3']()
set_eq(tmp_vec3, vec3_equals)

local tmp_item = adata.types['my.game.item']()
set_eq(tmp_item, item_equals)

local tmp_quest = adata.types['my.game.quest']()
set_eq(tmp_quest, quest_equals)

local tmp_player_v1 = adata.types['my.game.player_v1']()
set_eq(tmp_player_v1, player_equals)

local dump_player_v1 = function(pv1)
  print(pv1.id, pv1.name, pv1.age, pv1.factor)
  print(pv1.pos.x, pv1.pos.y, pv1.pos.z)
  print('inventory: ')
  for i,v in ipairs(pv1.inventory) do
    print("  ", v.id, v.type, v.level)
  end
end

local dump_player_v2 = function(pv2)
  print(pv2.id, pv2.name)
  print(pv2.pos.x, pv2.pos.y, pv2.pos.z)
  print('inventory: ')
  for i,v in ipairs(pv2.inventory) do
    print("  ", v.id, v.type, v.level)
  end
end

local ec = 0
local pv1 = adata.types['my.game.player_v1']()
pv1.id = 1
pv1.age = 22
pv1.factor = 2.0
pv1.name = 'pv1'

local itm = adata.types['my.game.item']()
itm.id = 11
itm.level = 321110
itm.type = 3
table.insert(pv1.inventory, itm)
itm = adata.types['my.game.item']()
itm.id = 12
table.insert(pv1.inventory, itm)

local qst = adata.types['my.game.quest']()
qst.id = 50
qst.name = 'quest1'
qst.description = 'There are something unusual...'
table.insert(pv1.quests, qst)

local buf_len = 4096
local stream = adata.new(buf_len)

ec = pv1:write(stream)
if ec > 0 then 
  -- some error, print it
  error(stream:trace_info())
end

local data = stream:get_write_data()

-- deserialize
stream:set_read_data(data)

local pv1_other = adata.types['my.game.player_v1']()

ec = pv1_other:read(stream)
if ec > 0 then 
  -- some error, print it
  error(stream:trace_info())
end

assert(player_equals(pv1, pv1_other))

stream:clear()

local pv2 = adata.types['my.game.player_v2']()

-- forward compat (old data, new struct)
ec = pv2:read(stream)
if ec > 0 then 
  -- some error, print it
  error(stream:trace_info())
end

assert(player_equals_v2(pv1, pv2))

stream:clear()
pv1 = adata.types['my.game.player_v1']()

-- backward compat (new data, old struct)
pv2.id = 6543;
pv2.name = 'pv2';
table.insert(pv2.friends, 2)
table.insert(pv2.friends, 100)

ec = pv2:write(stream)
if ec > 0 then 
  -- some error, print it
  error(stream:trace_info())
end

data = stream:get_write_data()
stream:set_read_data( data)
ec = pv1:read(stream)
if ec > 0 then 
  -- some error, print it
  error(stream:trace_info())
end

assert(player_equals_v2(pv1, pv2))

print('done.')

cpp2lua = function (pv1)
  local buf_len = 4096
  local stream = adata.new(buf_len)

  -- serialize
  ec = pv1:write(stream)
  if ec > 0 then 
    -- some error, print it
    error(stream:trace_info())
  end

  local data = stream:get_write_data()

  -- deserialize
  stream:set_read_data(data)

  local pv1_other = adata.types['my.game.player_v1']()

  ec = pv1_other:read(stream)
  if ec > 0 then 
    -- some error, print it
    error(stream:trace_info())
  end

  assert(pv1 == pv1_other)
  return pv1_other
end
