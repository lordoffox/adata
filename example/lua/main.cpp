///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include <adata_corec.hpp>

#include <iostream>
#include <string>
#include <cassert>

int main(int argc, char* argv[])
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  adata::lua::init_adata_corec(L);

  std::string source_dir = SOURCE_DIR;
  std::string path = "package.path = package.path .. ';";
  path += source_dir + "/generated/?.lua;";
  path += source_dir + "/../../lua/?.lua'";

  if (luaL_dostring(L, path.c_str()) != 0)
  {
    std::cerr << lua_tostring(L, -1) << std::endl;
  }

  std::string script = source_dir + "/main.lua";
  if (luaL_dofile(L, script.c_str()) != 0)
  {
    std::cerr << lua_tostring(L, -1) << std::endl;
  }

  if (L)
  {
    lua_close(L);
  }

  std::cin.get();
  return 0;
}

