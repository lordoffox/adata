///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#pragma once

#include "descrip.h"
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cerrno>

int member_cmp(member_define* const a, member_define* const b);

void split_string(const std::string& value, std::vector<std::string>& list, char separator);

extern std::string tab_strings[10];

inline std::string& tabs(int indent)
{
  return tab_strings[indent];
}

extern const char * data_type_name[];

inline const char * type_name(int type)
{
  if (type >= e_base_type::fix_int8 && type <= e_base_type::string)
  {
    return data_type_name[type];
  }
  return data_type_name[0];
}

inline const char * type_name(int type, bool fix)
{
  if (type >= e_base_type::int8 && fix)
  {
    type -= (int8 - fix_int8);
  }
  if (type >= e_base_type::fix_int8 && type <= e_base_type::string)
  {
    return data_type_name[type];
  }
  return data_type_name[0];
}

inline std::string replace_all_copy(std::string const& str , std::string const& c1 , std::string const& c2)
{
  std::string ret;
  size_t start = 0;
  auto p = str.find(c1, start);
  if( p == std::string::npos)
  {
    ret = str;
  }
  else
  {
    do
    {
      ret += str.substr(start, p - start);
      ret += c2;
      start = p + c1.length();
      p = str.find(c1, start);
    } while (p != std::string::npos);
    ret += str.substr(start);
  }
  return ret;
}

