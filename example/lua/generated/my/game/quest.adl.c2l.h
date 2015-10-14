#ifndef quest_adl_cpp2lua_h_
#define quest_adl_cpp2lua_h_

#include <adata_cpp2lua.hpp>

#include "quest.adl.h"

namespace adata
{
  namespace lua
  {
    ADATA_INLINE void load( lua_State * L, ::my::game::quest& value)
    {
      lua_getfield(L, -1, "id");
      {load(L, value.id);lua_pop(L, 1);}
      lua_getfield(L, -1, "name");
      {load(L, value.name);lua_pop(L, 1);}
      lua_getfield(L, -1, "description");
      {load(L, value.description);lua_pop(L, 1);}
    }

    ADATA_INLINE void push( lua_State * L, ::my::game::quest const& value, bool use_adata = true)
    {
      lua_createtable(L, 0, 3);
      if(use_adata && !set_metatable(L, "ad.mt.my.game.quest")){ luaL_error(L,"unknow type: my.game.quest"); }
      {push(L, value.id);}
      lua_setfield(L, -2, "id");
      {push(L, value.name);}
      lua_setfield(L, -2, "name");
      {push(L, value.description);}
      lua_setfield(L, -2, "description");
    }

  }
}

#endif
