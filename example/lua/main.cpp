///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include <my/game/player.adl.h>
#include <my/game/player.adl.c2l.h>
#include <adata_corec.hpp>

#include <iostream>
#include <string>
#include <cassert>

namespace util
{
  inline bool operator==(vec3 const& lhs, vec3 const& rhs)
  {
    return
      lhs.x == rhs.x &&
      lhs.y == rhs.y &&
      lhs.z == rhs.z
      ;
  }
}

namespace my
{
  namespace game
  {
    inline bool operator==(item const& lhs, item const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.level == rhs.level &&
        lhs.id == rhs.id
        ;
    }

    inline bool operator==(quest const& lhs, quest const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.name == rhs.name &&
        lhs.description == rhs.description
        ;
    }

    inline bool operator==(player_v1 const& lhs, player_v1 const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.age == rhs.age &&
        lhs.factor == rhs.factor &&
        lhs.pos == rhs.pos &&
        lhs.inventory == rhs.inventory &&
        lhs.quests == rhs.quests
        ;
    }

    inline bool operator==(player_v1 const& lhs, player_v2 const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.pos == rhs.pos &&
        lhs.inventory == rhs.inventory &&
        lhs.quests == rhs.quests
        ;
    }
  }
}

int main(int argc, char* argv[])
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  try
  {
    adata::lua::init_adata_corec(L);

    std::string source_dir = SOURCE_DIR;
    std::string path = "package.path = package.path .. ';";
    path += source_dir + "/generated/?.lua;";
    path += source_dir + "/../../lua/?.lua'";

    if (luaL_dostring(L, path.c_str()) != 0)
    {
      throw std::runtime_error(lua_tostring(L, -1));
    }

    std::string script = source_dir + "/main.lua";
    if (luaL_dofile(L, script.c_str()) != 0)
    {
      throw std::runtime_error(lua_tostring(L, -1));
    }

    my::game::player_v1 pv1;
    pv1.id = 1;
    pv1.age = 22;
    pv1.factor = 2.0f;
    pv1.name = "pv1";

    my::game::item itm;
    itm.id = 11;
    itm.level = 321110;
    itm.type = 3;
    pv1.inventory.push_back(itm);
    itm.id = 12;
    pv1.inventory.push_back(itm);

    my::game::quest qst;
    qst.id = 50;
    qst.name = "quest1";
    qst.description = "There are something unusual...";
    pv1.quests.push_back(qst);

    lua_getglobal(L, "cpp2lua");
    adata::lua::push(L, pv1);
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
      throw std::runtime_error(lua_tostring(L, -1));
    }

    my::game::player_v1 pv1_other;
    adata::lua::load(L, pv1_other);

    assert(pv1 == pv1_other);
  }
  catch (std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  if (L)
  {
    lua_close(L);
  }

  return 0;
}

