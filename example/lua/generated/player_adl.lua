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

regist_field('age');
regist_field('factor');
regist_field('friends');
regist_field('id');
regist_field('inventory');
regist_field('level');
regist_field('name');
regist_field('pos');
regist_field('quests');
regist_field('type');

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
    id = 0,
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

local layout_my_game_item = regist_layout_c(
  '\3\0\14\2\105\100\15\0\0\0\4\116\121\112\101\13\0\0\0\5\108\101\118\101\108\13\0\0\0',
  {fields.id,fields.type,fields.level,},
  {});
layouts.my_game_item = layout_my_game_item
local layout_my_game_player_v1 = regist_layout_c(
  '\7\2\40\2\105\100\13\0\0\0\4\110\97\109\101\19\0\30\0\3\97\103\101\13\0\0\0\3\112\111\115\22\0\0\0\9\105\110\118\101\110\116\111\114\121\20\0\0\1\22\0\6\113\117\101\115\116\115\20\0\0\1\22\0\6\102\97\99\116\111\114\17\0\0\0',
  {fields.id,fields.name,fields.age,fields.pos,fields.inventory,fields.quests,fields.factor,},
  {layouts.util_vec3,layouts.my_game_item,layouts.my_game_quest,});
layouts.my_game_player_v1 = layout_my_game_player_v1
local layout_my_game_player_v2 = regist_layout_c(
  '\8\3\48\2\105\100\13\0\0\0\4\110\97\109\101\19\0\30\0\3\97\103\101\13\1\0\0\3\112\111\115\22\0\0\0\9\105\110\118\101\110\116\111\114\121\20\0\0\1\22\0\6\113\117\101\115\116\115\20\0\0\1\22\0\6\102\97\99\116\111\114\17\1\0\0\7\102\114\105\101\110\100\115\20\0\0\1\13\0',
  {fields.id,fields.name,fields.age,fields.pos,fields.inventory,fields.quests,fields.factor,fields.friends,},
  {layouts.util_vec3,layouts.my_game_item,layouts.my_game_quest,});
layouts.my_game_player_v2 = layout_my_game_player_v2

local mc = {};
mc = {
  adtype = function() return m.item end,
  skip_read = function(o,buf) return skip_read_c( field_list , mt_type_list , buf , layout_my_game_item , o) end,
  size_of = function(o,buf) return size_of_c( field_list , mt_type_list , buf , layout_my_game_item , o) end,
  read = function(o,buf) return read_c( field_list , mt_type_list , buf , layout_my_game_item , o) end,
  write = function(o,buf) return write_c( field_list , mt_type_list , buf , layout_my_game_item , o) end,
};
mc.__index = mc;
mt[1] = mc;
mc = {
  adtype = function() return m.player_v1 end,
  skip_read = function(o,buf) return skip_read_c( field_list , mt_type_list , buf , layout_my_game_player_v1 , o) end,
  size_of = function(o,buf) return size_of_c( field_list , mt_type_list , buf , layout_my_game_player_v1 , o) end,
  read = function(o,buf) return read_c( field_list , mt_type_list , buf , layout_my_game_player_v1 , o) end,
  write = function(o,buf) return write_c( field_list , mt_type_list , buf , layout_my_game_player_v1 , o) end,
};
mc.__index = mc;
mt[2] = mc;
mc = {
  adtype = function() return m.player_v2 end,
  skip_read = function(o,buf) return skip_read_c( field_list , mt_type_list , buf , layout_my_game_player_v2 , o) end,
  size_of = function(o,buf) return size_of_c( field_list , mt_type_list , buf , layout_my_game_player_v2 , o) end,
  read = function(o,buf) return read_c( field_list , mt_type_list , buf , layout_my_game_player_v2 , o) end,
  write = function(o,buf) return write_c( field_list , mt_type_list , buf , layout_my_game_player_v2 , o) end,
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
set_layout_mt_c( layout_my_game_item , regist_mt_type(mt[1]) , 'ad_mt_my_game.item' , mt[1]);
set_layout_mt_c( layout_my_game_player_v1 , regist_mt_type(mt[2]) , 'ad_mt_my_game.player_v1' , mt[2]);
set_layout_mt_c( layout_my_game_player_v2 , regist_mt_type(mt[3]) , 'ad_mt_my_game.player_v2' , mt[3]);

return m;
