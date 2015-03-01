///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include "util.h"

int type_weight[e_base_type::max_count] = {
  0,    //   unknow,
  1,   //   fix_int8,
  1,   //   fix_uint8,
  2,   //   fix_int16,
  2,   //   fix_uint16,
  3,   //   fix_int32,
  3,   //   fix_uint32,
  4,   //   fix_int64,
  4,   //   fix_uint64,
  1,   //   int8,
  1,   //   uint8,
  2,   //   int16,
  2,   //   uint16,
  3,   //   int32,
  3,   //   uint32,
  4,   //   int64,
  4,   //   uint64,
  3,   //   float32,
  4,   //   float64,
  5,   //   string,
  5,   //   list,
  5,   //   map,
  5   //   type,
};

int member_cmp(member_define* const a, member_define* const b)
{
  return type_weight[a->m_type] - type_weight[b->m_type];
}

void split_string(const std::string& value, std::vector<std::string>& list, char separator)
{
  std::string::size_type start_pos = 0;
  for (std::string::size_type current_pos = 0; current_pos < value.length(); ++current_pos)
  {
    if (value[current_pos] == separator)
    {
      if (current_pos > start_pos)
      {
        std::string s_value = value.substr(start_pos, current_pos);
        list.push_back(s_value);
      }
      start_pos = current_pos + 1;
    }
  }
}

std::string tab_strings[10] = {
  "",
  "  ",
  "    ",
  "      ",
  "        ",
  "          ",
  "            ",
  "              ",
  "                ",
  "                  ",
};

const char * data_type_name[] = {
  "",
  "fixi8",
  "fixu8",
  "fixi16",
  "fixu16",
  "fixi32",
  "fixu32",
  "fixi64",
  "fixu64",
  "i8",
  "u8",
  "i16",
  "u16",
  "i32",
  "u32",
  "i64",
  "u64",
  "f32",
  "f64",
  "str",
};
