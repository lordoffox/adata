local adata_m = require'adata_core'
local ffi = require'ffi'
local ns = require'adata'
local new_buf = adata_m.new_buf;
local resize_buf = adata_m.resize_buf;
local clear_buf = adata_m.clear_buf;
local set_error = adata_m.set_error;
local trace_error = adata_m.trace_error;
local trace_info = adata_m.trace_info;
local get_write_data = adata_m.get_write_data;
local set_read_data = adata_m.set_read_data;
local get_rd_len = adata_m.get_rd_len;
local get_wt_len = adata_m.get_wt_len;
local skip_rd_len = adata_m.skip_rd_len;
local tablen = ns.tablen;

local rd_i32 = adata_m.rd_i32
local rd_u32 = adata_m.rd_u32
local rd_i64 = adata_m.rd_i64
local rd_u64 = adata_m.rd_u64
local rd_f32 = adata_m.rd_f32
local rd_str = adata_m.rd_str
local skip_rd_i32 = adata_m.skip_rd_i32
local skip_rd_u32 = adata_m.skip_rd_u32
local skip_rd_i64 = adata_m.skip_rd_i64
local skip_rd_u64 = adata_m.skip_rd_u64
local skip_rd_f32 = adata_m.skip_rd_f32
local skip_rd_str = adata_m.skip_rd_str
local wt_i32 = adata_m.wt_i32
local wt_u32 = adata_m.wt_u32
local wt_i64 = adata_m.wt_i64
local wt_u64 = adata_m.wt_u64
local wt_f32 = adata_m.wt_f32
local wt_str = adata_m.wt_str
local szof_i32 = adata_m.szof_i32
local szof_u32 = adata_m.szof_u32
local szof_i64 = adata_m.szof_i64
local szof_u64 = adata_m.szof_u64
local szof_f32 = adata_m.szof_f32
local szof_str = adata_m.szof_str

local field_info = {
  ffi.new("char[3]","age"),
  ffi.new("char[6]","factor"),
  ffi.new("char[7]","friends"),
  ffi.new("char[2]","id"),
  ffi.new("char[9]","inventory"),
  ffi.new("char[5]","level"),
  ffi.new("char[4]","name"),
  ffi.new("char[3]","pos"),
  ffi.new("char[6]","quests"),
  ffi.new("char[4]","type"),
}

require'quest_adl'
require'vec3_adl'

local mt = {};

local m = {
  item,
  player_v1,
  player_v2,
};

m.item = function()
  local obj = {
    id = 0ll,
    type = 0,
    level = 0,
  };
  setmetatable(obj,mt[1]);
  return obj;
end

m.player_v1 = function()
  local obj = {
    id = 0,
    name = '',
    age = 0,
    pos = ns.util.vec3(),
    inventory = {},
    quests = {},
    factor = 1.0,
  };
  setmetatable(obj,mt[2]);
  return obj;
end

m.player_v2 = function()
  local obj = {
    id = 0,
    name = '',
    pos = ns.util.vec3(),
    inventory = {},
    quests = {},
    friends = {},
  };
  setmetatable(obj,mt[3]);
  return obj;
end

local mc = {};
mc = {
  adtype = function() return m.item end,
  skip_read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec = skip_rd_i64(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[10] , -1); return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[6] , -1); return ec; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  size_of = function(o)
    local size = 0
    local tag = 0
    tag = tag + 1;
    tag = tag + 2;
    tag = tag + 4;
    if (tag % 2) >= 1 then
      size = size + szof_i64(o.id);
    end
    if (tag % 4) >= 2 then
      size = size + szof_i32(o.type);
    end
    if (tag % 8) >= 4 then
      size = size + szof_i32(o.level);
    end
    size = size + szof_u64(tag);
    size = size + szof_i32(size + szof_i32(size));
    return size;
  end,
  read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec,o.id = rd_i64(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec,o.type = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[10] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec,o.level = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[6] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  write = function(o,buf)
    local write_tag = 0
    write_tag = write_tag + 1;
    write_tag = write_tag + 2;
    write_tag = write_tag + 4;
    ec = wt_u64(buf,write_tag);
    if ec >0 then return ec; end;
    ec = wt_i32(buf,o:size_of());
    if ec >0 then return ec; end;
    if (write_tag % 2) >= 1 then
      ec = wt_i64(buf,o.id);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (write_tag % 4) >= 2 then
      ec = wt_i32(buf,o.type);if ec > 0 then trace_error(buf, field_info[10] , -1); return ec; end;
    end
    if (write_tag % 8) >= 4 then
      ec = wt_i32(buf,o.level);if ec > 0 then trace_error(buf, field_info[6] , -1); return ec; end;
    end
    return ec;
  end,
};
mc.__index = mc;
mt[1] = mc;
mc = {
  adtype = function() return m.player_v1 end,
  skip_read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec = skip_rd_str(buf,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[1] , -1); return ec; end;
    end
    if (read_tag % 16) >= 8 then
      read_tag = read_tag - 8;
      local temp = ns.util.vec3(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
    end
    if (read_tag % 32) >= 16 then
      read_tag = read_tag - 16;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[5] , -1); return ec; end;
      for i = 1 , len do local temp = m.item(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; end;
    end
    if (read_tag % 64) >= 32 then
      read_tag = read_tag - 32;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[9] , -1); return ec; end;
      for i = 1 , len do local temp = ns.my_game.quest(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; end;
    end
    if (read_tag % 128) >= 64 then
      read_tag = read_tag - 64;
      ec = skip_rd_f32(buf);if ec > 0 then trace_error(buf, field_info[2] , -1); return ec; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  size_of = function(o)
    local size = 0
    local tag = 0
    tag = tag + 1;
    if #o.name > 0 then tag = tag + 2; end;
    tag = tag + 4;
    tag = tag + 8;
    if #o.inventory > 0 then tag = tag + 16; end;
    if #o.quests > 0 then tag = tag + 32; end;
    tag = tag + 64;
    if (tag % 2) >= 1 then
      size = size + szof_i32(o.id);
    end
    if (tag % 4) >= 2 then
      size = size + szof_str(o.name);
    end
    if (tag % 8) >= 4 then
      size = size + szof_i32(o.age);
    end
    if (tag % 16) >= 8 then
      size = size + o.pos:size_of();
    end
    if (tag % 32) >= 16 then
      local len = #o.inventory;
      size = size + szof_u32(len);
      local v;
      for i = 1,len do v = o.inventory[i]; size = size + v:size_of(); end;
    end
    if (tag % 64) >= 32 then
      local len = #o.quests;
      size = size + szof_u32(len);
      local v;
      for i = 1,len do v = o.quests[i]; size = size + v:size_of(); end;
    end
    if (tag % 128) >= 64 then
      size = size + szof_f32(o.factor);
    end
    size = size + szof_u64(tag);
    size = size + szof_i32(size + szof_i32(size));
    return size;
  end,
  read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec,o.id = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec,o.name = rd_str(buf,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec,o.age = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[1] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 16) >= 8 then
      read_tag = read_tag - 8;
      ec = o.pos:read(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 32) >= 16 then
      read_tag = read_tag - 16;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[5] , -1); return ec; end;
      local v;
      for i = 1 , len do v = m.item(); ec = v:read(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; o.inventory[i] = v end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 64) >= 32 then
      read_tag = read_tag - 32;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[9] , -1); return ec; end;
      local v;
      for i = 1 , len do v = ns.my_game.quest(); ec = v:read(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; o.quests[i] = v end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 128) >= 64 then
      read_tag = read_tag - 64;
      ec,o.factor = rd_f32(buf);if ec > 0 then trace_error(buf, field_info[2] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  write = function(o,buf)
    local write_tag = 0
    write_tag = write_tag + 1;
    if #o.name > 0 then write_tag = write_tag + 2; end;
    write_tag = write_tag + 4;
    write_tag = write_tag + 8;
    if #o.inventory > 0 then write_tag = write_tag + 16; end;
    if #o.quests > 0 then write_tag = write_tag + 32; end;
    write_tag = write_tag + 64;
    ec = wt_u64(buf,write_tag);
    if ec >0 then return ec; end;
    ec = wt_i32(buf,o:size_of());
    if ec >0 then return ec; end;
    if (write_tag % 2) >= 1 then
      ec = wt_i32(buf,o.id);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (write_tag % 4) >= 2 then
      ec = wt_str(buf,o.name,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
    end
    if (write_tag % 8) >= 4 then
      ec = wt_i32(buf,o.age);if ec > 0 then trace_error(buf, field_info[1] , -1); return ec; end;
    end
    if (write_tag % 16) >= 8 then
      ec = o.pos:write(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
    end
    if (write_tag % 32) >= 16 then
      local len = #o.inventory;
      local ec  = wt_u32(buf,len);
      if ec > 0 then return ec; end;
      local v;
      for i = 1,len do v = o.inventory[i]; ec = v:write(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; end;
    end
    if (write_tag % 64) >= 32 then
      local len = #o.quests;
      local ec  = wt_u32(buf,len);
      if ec > 0 then return ec; end;
      local v;
      for i = 1,len do v = o.quests[i]; ec = v:write(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; end;
    end
    if (write_tag % 128) >= 64 then
      ec = wt_f32(buf,o.factor);if ec > 0 then trace_error(buf, field_info[2] , -1); return ec; end;
    end
    return ec;
  end,
};
mc.__index = mc;
mt[2] = mc;
mc = {
  adtype = function() return m.player_v2 end,
  skip_read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec = skip_rd_str(buf,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[1] , -1); return ec; end;
    end
    if (read_tag % 16) >= 8 then
      read_tag = read_tag - 8;
      local temp = ns.util.vec3(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
    end
    if (read_tag % 32) >= 16 then
      read_tag = read_tag - 16;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[5] , -1); return ec; end;
      for i = 1 , len do local temp = m.item(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; end;
    end
    if (read_tag % 64) >= 32 then
      read_tag = read_tag - 32;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[9] , -1); return ec; end;
      for i = 1 , len do local temp = ns.my_game.quest(); ec = temp:skip_read(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; end;
    end
    if (read_tag % 128) >= 64 then
      read_tag = read_tag - 64;
      ec = skip_rd_f32(buf);if ec > 0 then trace_error(buf, field_info[2] , -1); return ec; end;
    end
    if (read_tag % 256) >= 128 then
      read_tag = read_tag - 128;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[3] , -1); return ec; end;
      for i = 1 , len do ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[3] , i); return ec; end; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  size_of = function(o)
    local size = 0
    local tag = 0
    tag = tag + 1;
    if #o.name > 0 then tag = tag + 2; end;
    tag = tag + 8;
    if #o.inventory > 0 then tag = tag + 16; end;
    if #o.quests > 0 then tag = tag + 32; end;
    if #o.friends > 0 then tag = tag + 128; end;
    if (tag % 2) >= 1 then
      size = size + szof_i32(o.id);
    end
    if (tag % 4) >= 2 then
      size = size + szof_str(o.name);
    end
    if (tag % 16) >= 8 then
      size = size + o.pos:size_of();
    end
    if (tag % 32) >= 16 then
      local len = #o.inventory;
      size = size + szof_u32(len);
      local v;
      for i = 1,len do v = o.inventory[i]; size = size + v:size_of(); end;
    end
    if (tag % 64) >= 32 then
      local len = #o.quests;
      size = size + szof_u32(len);
      local v;
      for i = 1,len do v = o.quests[i]; size = size + v:size_of(); end;
    end
    if (tag % 256) >= 128 then
      local len = #o.friends;
      size = size + szof_u32(len);
      local v;
      for i = 1,len do v = o.friends[i]; size = size + szof_i32(v); end;
    end
    size = size + szof_u64(tag);
    size = size + szof_i32(size + szof_i32(size));
    return size;
  end,
  read = function(o,buf)
    local offset = get_rd_len(buf);
    local ec,read_tag = rd_u64(buf);
    if ec > 0 then return ec; end;
    local len_tag = 0;
    ec,len_tag = rd_i32(buf);
    if ec > 0 then return ec; end;
    if (read_tag % 2) >= 1 then
      read_tag = read_tag - 1;
      ec,o.id = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 4) >= 2 then
      read_tag = read_tag - 2;
      ec,o.name = rd_str(buf,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 8) >= 4 then
      read_tag = read_tag - 4;
      ec = skip_rd_i32(buf);if ec > 0 then trace_error(buf, field_info[1] , -1); return ec; end;
    end
    if (read_tag % 16) >= 8 then
      read_tag = read_tag - 8;
      ec = o.pos:read(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 32) >= 16 then
      read_tag = read_tag - 16;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[5] , -1); return ec; end;
      local v;
      for i = 1 , len do v = m.item(); ec = v:read(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; o.inventory[i] = v end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 64) >= 32 then
      read_tag = read_tag - 32;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[9] , -1); return ec; end;
      local v;
      for i = 1 , len do v = ns.my_game.quest(); ec = v:read(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; o.quests[i] = v end;
      if ec > 0 then return ec; end;
    end
    if (read_tag % 128) >= 64 then
      read_tag = read_tag - 64;
      ec = skip_rd_f32(buf);if ec > 0 then trace_error(buf, field_info[2] , -1); return ec; end;
    end
    if (read_tag % 256) >= 128 then
      read_tag = read_tag - 128;
      local ec,len = rd_u32(buf);
      if ec > 0 then trace_error(buf, field_info[3] , -1); return ec; end;
      local v;
      for i = 1 , len do ec,v = rd_i32(buf);if ec > 0 then trace_error(buf, field_info[3] , i); return ec; end; o.friends[i] = v end;
      if ec > 0 then return ec; end;
    end
    if len_tag >= 0 then
      local read_len = get_rd_len(buf) - offset;
      if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
    end
    return ec;
  end,
  write = function(o,buf)
    local write_tag = 0
    write_tag = write_tag + 1;
    if #o.name > 0 then write_tag = write_tag + 2; end;
    write_tag = write_tag + 8;
    if #o.inventory > 0 then write_tag = write_tag + 16; end;
    if #o.quests > 0 then write_tag = write_tag + 32; end;
    if #o.friends > 0 then write_tag = write_tag + 128; end;
    ec = wt_u64(buf,write_tag);
    if ec >0 then return ec; end;
    ec = wt_i32(buf,o:size_of());
    if ec >0 then return ec; end;
    if (write_tag % 2) >= 1 then
      ec = wt_i32(buf,o.id);if ec > 0 then trace_error(buf, field_info[4] , -1); return ec; end;
    end
    if (write_tag % 4) >= 2 then
      ec = wt_str(buf,o.name,30);if ec > 0 then trace_error(buf, field_info[7] , -1); return ec; end;
    end
    if (write_tag % 16) >= 8 then
      ec = o.pos:write(buf);if ec > 0 then trace_error(buf, field_info[8] , -1); return ec; end;
    end
    if (write_tag % 32) >= 16 then
      local len = #o.inventory;
      local ec  = wt_u32(buf,len);
      if ec > 0 then return ec; end;
      local v;
      for i = 1,len do v = o.inventory[i]; ec = v:write(buf);if ec > 0 then trace_error(buf, field_info[5] , i); return ec; end; end;
    end
    if (write_tag % 64) >= 32 then
      local len = #o.quests;
      local ec  = wt_u32(buf,len);
      if ec > 0 then return ec; end;
      local v;
      for i = 1,len do v = o.quests[i]; ec = v:write(buf);if ec > 0 then trace_error(buf, field_info[9] , i); return ec; end; end;
    end
    if (write_tag % 256) >= 128 then
      local len = #o.friends;
      local ec  = wt_u32(buf,len);
      if ec > 0 then return ec; end;
      local v;
      for i = 1,len do v = o.friends[i]; ec = wt_i32(buf,v);if ec > 0 then trace_error(buf, field_info[3] , i); return ec; end; end;
    end
    return ec;
  end,
};
mc.__index = mc;
mt[3] = mc;
if ns.my_game == nil then
  ns.my_game = m;
else
  ns.my_game.item = m.item
  ns.my_game.player_v1 = m.player_v1
  ns.my_game.player_v2 = m.player_v2
end
return m;
