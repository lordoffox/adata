require'check_lua_version'(5,1);

local adata_m = require'adata_core'
local int64 = require'int64'
local detail = require'adata_detail';
local ns = require'adata';
local regist_field = detail.regist_field;
local fields = detail.fields;
local field_list = detail.field_list;
local layouts = detail.layouts;
local regist_mt_type = detail.regist_mt_type;
local mt_type_list = detail.mt_type_list;
local read_c = adata_m.read;
local skip_read_c = adata_m.skip_read;
local size_of_c = adata_m.size_of;
local write_c = adata_m.write;
local regist_layout_c = adata_m.regist_layout;
local set_layout_mt_c = adata_m.set_layout_mt;

regist_field('x');
regist_field('y');
regist_field('z');


local mt = {};

local m = {
  vec3,
};

m.vec3 = function()
  local obj = {
    x = 0.0,
    y = 0.0,
    z = 0.0,
  };
  setmetatable(obj,mt[1]);
  return obj;
end

local layout_util_vec3 = regist_layout_c(
  '\3\0\6\1\120\17\0\0\0\1\121\17\0\0\0\1\122\17\0\0\0',
  {fields.x,fields.y,fields.z,},
  {});
layouts.util_vec3 = layout_util_vec3

local mc = {};
mc = {
  adtype = function() return m.vec3 end,
  skip_read = function(o,buf) return skip_read_c( field_list , mt_type_list , buf , layout_util_vec3 , o) end,
  size_of = function(o,buf) return size_of_c( field_list , mt_type_list , buf , layout_util_vec3 , o) end,
  read = function(o,buf) return read_c( field_list , mt_type_list , buf , layout_util_vec3 , o) end,
  write = function(o,buf) return write_c( field_list , mt_type_list , buf , layout_util_vec3 , o) end,
};
mc.__index = mc;
mt[1] = mc;
if ns.util == nil then
  ns.util = m;
else
  ns.util.vec3 = m.vec3
end
set_layout_mt_c( layout_util_vec3 , regist_mt_type(mt[1]) , 'ad_mt_util.vec3' , mt[1]);

return m;
