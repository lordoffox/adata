local core = require'adata_core';
local load_c = core.load;
local skip_read_c = core.skip_read;
local size_of_c = core.size_of;
local read_c = core.read;
local write_c = core.write;
local raw_size_of_c = core.raw_size_of;
local raw_read_c = core.raw_read;
local raw_write_c = core.raw_write;

--global string pool
local str_pool = {};
local str_idx = {};

--global type layout map
local type_layouts = {};
--global type metatable for cpp2lua
local mts = {};
--assign layout to metatable list
local mt_list = {};
--global type constructor map
local types = {};

local make_type = function(ns,ty)
  local type_def = ty[1];
  local name = ty[2];
  local field_list = ty[3];
  local construct_list = ty[4];
  local mt = ty[5];

  mt.__index = mt;
  local field_count = #field_list;
  for i = 1,field_count do
    local val = construct_list[i];
    if type(val) == "string" and val ~= "" then
      construct_list[i] = types[val];
    end
  end
  
  local construct = function()
    local obj = {};
    for i = 1,field_count do
      local field = field_list[i];
      local val = construct_list[i];
      if val == nil then
        obj[field] = {};
      elseif val == "" then
        obj[field] = val;
      elseif type(val) == "function" then
        obj[field] = val();
      else
        obj[field] = val;
      end
    end
    setmetatable(obj,mt);
    return obj;
  end

  types[ns..'.'..name] = construct;

  local skip_read = function(o,buf)
    return skip_read_c( str_idx , mt_list , buf , type_def , o);
  end

  local size_of = function(o,buf)
    return size_of_c( str_idx , mt_list , type_def , o);
  end

  local read = function(o,buf)
    return read_c( str_idx , mt_list , buf , type_def , o);
  end

  local write = function(o,buf)
    return write_c( str_idx , mt_list , buf , type_def , o);
  end
  
  local raw_size_of = function(o,buf)
    return raw_size_of_c( str_idx , mt_list , type_def , o);
  end

  local raw_read = function(o,buf)
    return raw_read_c( str_idx , mt_list , buf , type_def , o);
  end

  local raw_write = function(o,buf)
    return raw_write_c( str_idx , mt_list , buf , type_def , o);
  end
  
  mt.adtype = construct;
  mt.skip_read = skip_read;
  mt.size_of = size_of;
  mt.read = read;
  mt.write = write;
  mt.raw_size_of = raw_size_of;
  mt.raw_read = raw_read;
  mt.raw_write = raw_write;
  
  if type(type_def) == 'table' then type_def.constructor = construct; end;
end

local make_namespace = function (ns)
  local name = ns[1];
  local ts = ns[2];
  local len = #ts;
  for i = 1 , len do
    make_type(name,ts[i])
  end
end

local tinsert = table.insert;

local load = function(file)
  local ret = load_c(file,str_pool,str_idx,type_layouts,mts,mt_list);
  local len = #ret
  for i = 1 , len do
    make_namespace(ret[i])
  end
end

local tablen = function(tab)
	if tab == nil then return 0 end
	local count = 0
  for _ in pairs(tab) do count = count + 1 end
  return count
end

m = {
  load = load,
  types = types,
  new = core.new_buf,
  tablen = tablen,
}

return m;
