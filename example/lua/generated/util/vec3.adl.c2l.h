#ifndef vec3_adl_cpp2lua_h_
#define vec3_adl_cpp2lua_h_

#include <adata_cpp2lua.hpp>

#include "vec3.adl.h"

namespace adata
{
  namespace lua
  {
    ADATA_INLINE void load( lua_State * L, ::util::vec3& value)
    {
      lua_getfield(L, -1, "x");
      {load(L, value.x);lua_pop(L, 1);}
      lua_getfield(L, -1, "y");
      {load(L, value.y);lua_pop(L, 1);}
      lua_getfield(L, -1, "z");
      {load(L, value.z);lua_pop(L, 1);}
    }

    ADATA_INLINE void push( lua_State * L, ::util::vec3 const& value, bool use_adata = true)
    {
      lua_createtable(L, 0, 3);
      if(use_adata && !set_metatable(L, "ad.mt.util.vec3")){ luaL_error(L,"unknow type: util.vec3"); }
      {push(L, value.x);}
      lua_setfield(L, -2, "x");
      {push(L, value.y);}
      lua_setfield(L, -2, "y");
      {push(L, value.z);}
      lua_setfield(L, -2, "z");
    }

  }
}

#endif
