#ifndef player_adl_cpp2lua_h_
#define player_adl_cpp2lua_h_

#include <adata_cpp2lua.hpp>
#include "my/game/quest.adl.c2l.h"
#include "util/vec3.adl.c2l.h"

#include "player.adl.h"

namespace adata
{
  namespace lua
  {
    ADATA_INLINE void load( lua_State * L, ::my::game::item& value)
    {
      lua_getfield(L, -1, "id");
      {load(L, value.id);lua_pop(L, 1);}
      lua_getfield(L, -1, "type");
      {load(L, value.type);lua_pop(L, 1);}
      lua_getfield(L, -1, "level");
      {load(L, value.level);lua_pop(L, 1);}
    }

    ADATA_INLINE void push( lua_State * L, ::my::game::item const& value, bool use_adata = true)
    {
      lua_createtable(L, 0, 3);
      if(use_adata && !set_metatable(L, "ad.mt.my.game.item")){ luaL_error(L,"unknow type: my.game.item"); }
      {push(L, value.id);}
      lua_setfield(L, -2, "id");
      {push(L, value.type);}
      lua_setfield(L, -2, "type");
      {push(L, value.level);}
      lua_setfield(L, -2, "level");
    }

    ADATA_INLINE void load( lua_State * L, ::my::game::player_v1& value)
    {
      lua_getfield(L, -1, "id");
      {load(L, value.id);lua_pop(L, 1);}
      lua_getfield(L, -1, "name");
      {load(L, value.name);lua_pop(L, 1);}
      lua_getfield(L, -1, "age");
      {load(L, value.age);lua_pop(L, 1);}
      lua_getfield(L, -1, "pos");
      {load(L, value.pos);lua_pop(L, 1);}
      lua_getfield(L, -1, "inventory");
      {
        int len = (int)seq_len(L,-1);
        value.inventory.resize(len);
        for (int i = 0 ; i < len ; ++i)
        {
          lua_rawgeti(L, -1, i+1);
          {load(L, value.inventory[i]);lua_pop(L, 1);}
        }
        lua_pop(L,1);
      }
      lua_getfield(L, -1, "quests");
      {
        int len = (int)seq_len(L,-1);
        value.quests.resize(len);
        for (int i = 0 ; i < len ; ++i)
        {
          lua_rawgeti(L, -1, i+1);
          {load(L, value.quests[i]);lua_pop(L, 1);}
        }
        lua_pop(L,1);
      }
      lua_getfield(L, -1, "factor");
      {load(L, value.factor);lua_pop(L, 1);}
    }

    ADATA_INLINE void push( lua_State * L, ::my::game::player_v1 const& value, bool use_adata = true)
    {
      lua_createtable(L, 0, 7);
      if(use_adata && !set_metatable(L, "ad.mt.my.game.player_v1")){ luaL_error(L,"unknow type: my.game.player_v1"); }
      {push(L, value.id);}
      lua_setfield(L, -2, "id");
      {push(L, value.name);}
      lua_setfield(L, -2, "name");
      {push(L, value.age);}
      lua_setfield(L, -2, "age");
      {push(L, value.pos, use_adata);}
      lua_setfield(L, -2, "pos");
      {
        int len = (int)value.inventory.size();
        lua_createtable(L, 0, len);
        for (int i = 0 ; i < len ; ++i)
        {
          {push(L, value.inventory[i], use_adata);}
          lua_rawseti(L, -2, i+1);
        }
      }
      lua_setfield(L, -2, "inventory");
      {
        int len = (int)value.quests.size();
        lua_createtable(L, 0, len);
        for (int i = 0 ; i < len ; ++i)
        {
          {push(L, value.quests[i], use_adata);}
          lua_rawseti(L, -2, i+1);
        }
      }
      lua_setfield(L, -2, "quests");
      {push(L, value.factor);}
      lua_setfield(L, -2, "factor");
    }

    ADATA_INLINE void load( lua_State * L, ::my::game::player_v2& value)
    {
      lua_getfield(L, -1, "id");
      {load(L, value.id);lua_pop(L, 1);}
      lua_getfield(L, -1, "name");
      {load(L, value.name);lua_pop(L, 1);}
      lua_getfield(L, -1, "pos");
      {load(L, value.pos);lua_pop(L, 1);}
      lua_getfield(L, -1, "inventory");
      {
        int len = (int)seq_len(L,-1);
        value.inventory.resize(len);
        for (int i = 0 ; i < len ; ++i)
        {
          lua_rawgeti(L, -1, i+1);
          {load(L, value.inventory[i]);lua_pop(L, 1);}
        }
        lua_pop(L,1);
      }
      lua_getfield(L, -1, "quests");
      {
        int len = (int)seq_len(L,-1);
        value.quests.resize(len);
        for (int i = 0 ; i < len ; ++i)
        {
          lua_rawgeti(L, -1, i+1);
          {load(L, value.quests[i]);lua_pop(L, 1);}
        }
        lua_pop(L,1);
      }
      lua_getfield(L, -1, "friends");
      {
        int len = (int)seq_len(L,-1);
        value.friends.resize(len);
        for (int i = 0 ; i < len ; ++i)
        {
          lua_rawgeti(L, -1, i+1);
          {load(L, value.friends[i]);lua_pop(L, 1);}
        }
        lua_pop(L,1);
      }
    }

    ADATA_INLINE void push( lua_State * L, ::my::game::player_v2 const& value, bool use_adata = true)
    {
      lua_createtable(L, 0, 8);
      if(use_adata && !set_metatable(L, "ad.mt.my.game.player_v2")){ luaL_error(L,"unknow type: my.game.player_v2"); }
      {push(L, value.id);}
      lua_setfield(L, -2, "id");
      {push(L, value.name);}
      lua_setfield(L, -2, "name");
      {push(L, value.pos, use_adata);}
      lua_setfield(L, -2, "pos");
      {
        int len = (int)value.inventory.size();
        lua_createtable(L, 0, len);
        for (int i = 0 ; i < len ; ++i)
        {
          {push(L, value.inventory[i], use_adata);}
          lua_rawseti(L, -2, i+1);
        }
      }
      lua_setfield(L, -2, "inventory");
      {
        int len = (int)value.quests.size();
        lua_createtable(L, 0, len);
        for (int i = 0 ; i < len ; ++i)
        {
          {push(L, value.quests[i], use_adata);}
          lua_rawseti(L, -2, i+1);
        }
      }
      lua_setfield(L, -2, "quests");
      {
        int len = (int)value.friends.size();
        lua_createtable(L, 0, len);
        for (int i = 0 ; i < len ; ++i)
        {
          {push(L, value.friends[i]);}
          lua_rawseti(L, -2, i+1);
        }
      }
      lua_setfield(L, -2, "friends");
    }

  }
}

#endif
