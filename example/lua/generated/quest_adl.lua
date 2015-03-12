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

regist_field('description');
regist_field('id');
regist_field('name');


local mt = {};

local m = {
  quest,
};

m.quest = function()
  local obj = {
    id = 0,
    name = '',
    description = '',
  };
  setmetatable(obj,mt[1]);
  return obj;
end

local layout_my_game_quest = regist_layout_c(
  '\3\0\20\2\105\100\13\0\0\0\4\110\97\109\101\19\0\0\0\11\100\101\115\99\114\105\112\116\105\111\110\19\0\0\0',
  {fields.id,fields.name,fields.description,},
  {});
layouts.my_game_quest = layout_my_game_quest

local mc = {};
mc = {
  adtype = function() return m.quest end,
  skip_read = function(o,buf) return skip_read_c( field_list , mt_type_list , buf , layout_my_game_quest , o) end,
  size_of = function(o,buf) return size_of_c( field_list , mt_type_list , buf , layout_my_game_quest , o) end,
  read = function(o,buf) return read_c( field_list , mt_type_list , buf , layout_my_game_quest , o) end,
  write = function(o,buf) return write_c( field_list , mt_type_list , buf , layout_my_game_quest , o) end,
};
mc.__index = mc;
mt[1] = mc;
if ns.my_game == nil then
  ns.my_game = m;
else
  ns.my_game.quest = m.quest
end
set_layout_mt_c( layout_my_game_quest , regist_mt_type(mt[1]));

return m;
