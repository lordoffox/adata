///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include "descrip.h"
#include "util.h"
#include <algorithm>

bool type_define::has_member(const std::string& member_name) const
{
  for (const auto& define : this->m_members)
  {
    if (define.m_name == member_name)
    {
      return true;
    }
  }
  return false;
}

bool descrip_define::has_decl_type(const std::string& type)	const
{
  e_base_type e_type = get_type(type);
  if (e_type == e_base_type::type)
  {
    auto pred = [&type](type_define const& define){ return define.m_name == type; };
    if (
      std::find_if(
      std::begin(m_types), std::end(m_types),
      pred
      ) != std::end(m_types)
      )
    {
      return true;
    }

    // Nous Xiong: check include types
    if (
      std::find_if(
      std::begin(m_include_types), std::end(m_include_types),
      pred
      ) != std::end(m_include_types)
      )
    {
      return true;
    }

    return false;
  }
  return true;
}

const type_define * descrip_define::find_decl_type(const std::string& type) const
{
  e_base_type e_type = get_type(type);
  if (e_type == e_base_type::type)
  {
    auto pred = [&type](type_define const& define){ return define.m_name == type; };
    auto itr = std::find_if(std::begin(m_types), std::end(m_types), pred);

    if (itr != std::end(m_types))
    {
      return std::addressof(*itr);
    }

    // Nous Xiong: find from include types
    itr = std::find_if(std::begin(m_include_types), std::end(m_include_types), pred);

    if (itr != std::end(m_include_types))
    {
      return std::addressof(*itr);
    }
  }
  return NULL;
}

namespace
{
  std::string s_fix_int8 = "fix_int8";
  std::string s_fix_uint8 = "fix_uint8";
  std::string s_fix_int16 = "fix_int16";
  std::string s_fix_uint16 = "fix_uint16";
  std::string s_fix_int32 = "fix_int32";
  std::string s_fix_uint32 = "fix_uint32";
  std::string s_fix_int64 = "fix_int64";
  std::string s_fix_uint64 = "fix_uint64";
  std::string s_int8 = "int8";
  std::string s_uint8 = "uint8";
  std::string s_int16 = "int16";
  std::string s_uint16 = "uint16";
  std::string s_int32 = "int32";
  std::string s_uint32 = "uint32";
  std::string s_int64 = "int64";
  std::string s_uint64 = "uint64";
  std::string s_float32 = "float32";
  std::string s_float64 = "float64";
  std::string s_string = "string";
  std::string s_list = "list";
  std::string s_map = "map";

  static typename_map_type make_typedef_cpp()
  {
    typename_map_type define;
    define.insert(std::make_pair(s_fix_int8, "int8_t"));
    define.insert(std::make_pair(s_fix_uint8, "uint8_t"));
    define.insert(std::make_pair(s_fix_int16, "int16_t"));
    define.insert(std::make_pair(s_fix_uint16, "uint16_t"));
    define.insert(std::make_pair(s_fix_int32, "int32_t"));
    define.insert(std::make_pair(s_fix_uint32, "uint32_t"));
    define.insert(std::make_pair(s_fix_int64, "int64_t"));
    define.insert(std::make_pair(s_fix_uint64, "uint64_t"));
    define.insert(std::make_pair(s_int8, "int8_t"));
    define.insert(std::make_pair(s_uint8, "uint8_t"));
    define.insert(std::make_pair(s_int16, "int16_t"));
    define.insert(std::make_pair(s_uint16, "uint16_t"));
    define.insert(std::make_pair(s_int32, "int32_t"));
    define.insert(std::make_pair(s_uint32, "uint32_t"));
    define.insert(std::make_pair(s_int64, "int64_t"));
    define.insert(std::make_pair(s_uint64, "uint64_t"));
    define.insert(std::make_pair(s_float32, "float"));
    define.insert(std::make_pair(s_float64, "double"));
    define.insert(std::make_pair(s_string, "::std::string"));
    define.insert(std::make_pair(s_list, "::std::vector"));
    define.insert(std::make_pair(s_map, "::std::map"));

    return define;
  }

  typedef std::map<std::string, e_base_type>	type_map_type;

  static type_map_type make_typedef_type()
  {
    type_map_type define;
    define.insert(std::make_pair(s_fix_int8, fix_int8));
    define.insert(std::make_pair(s_fix_uint8, fix_uint8));
    define.insert(std::make_pair(s_fix_int16, fix_int16));
    define.insert(std::make_pair(s_fix_uint16, fix_uint16));
    define.insert(std::make_pair(s_fix_int32, fix_int32));
    define.insert(std::make_pair(s_fix_uint32, fix_uint32));
    define.insert(std::make_pair(s_fix_int64, fix_int64));
    define.insert(std::make_pair(s_fix_uint64, fix_uint64));
    define.insert(std::make_pair(s_int8, int8));
    define.insert(std::make_pair(s_uint8, uint8));
    define.insert(std::make_pair(s_int16, int16));
    define.insert(std::make_pair(s_uint16, uint16));
    define.insert(std::make_pair(s_int32, int32));
    define.insert(std::make_pair(s_uint32, uint32));
    define.insert(std::make_pair(s_int64, int64));
    define.insert(std::make_pair(s_uint64, uint64));
    define.insert(std::make_pair(s_float32, float32));
    define.insert(std::make_pair(s_float64, float64));
    define.insert(std::make_pair(s_string, e_base_type::string));

    define.insert(std::make_pair(s_list, list));
    define.insert(std::make_pair(s_map, map));

    return define;
  }

  static type_map_type& get_bast_type()
  {
    static type_map_type define = make_typedef_type();
    return define;
  }

  static typename_map_type make_typedef_csharp()
  {
    typename_map_type define;
    define.insert(std::make_pair(s_fix_int8, "sbyte"));
    define.insert(std::make_pair(s_fix_uint8, "byte"));
    define.insert(std::make_pair(s_fix_int16, "Int16"));
    define.insert(std::make_pair(s_fix_uint16, "UInt16"));
    define.insert(std::make_pair(s_fix_int32, "Int32"));
    define.insert(std::make_pair(s_fix_uint32, "UInt32"));
    define.insert(std::make_pair(s_fix_int64, "Int64"));
    define.insert(std::make_pair(s_fix_uint64, "UInt64"));
    define.insert(std::make_pair(s_int8, "sbyte"));
    define.insert(std::make_pair(s_uint8, "byte"));
    define.insert(std::make_pair(s_int16, "Int16"));
    define.insert(std::make_pair(s_uint16, "UInt16"));
    define.insert(std::make_pair(s_int32, "Int32"));
    define.insert(std::make_pair(s_uint32, "UInt32"));
    define.insert(std::make_pair(s_int64, "Int64"));
    define.insert(std::make_pair(s_uint64, "UInt64"));
    define.insert(std::make_pair(s_float32, "float"));
    define.insert(std::make_pair(s_float64, "double"));
    define.insert(std::make_pair(s_string, "string"));
    define.insert(std::make_pair(s_list, "List"));
    define.insert(std::make_pair(s_map, "Dictionary"));

    return define;
  }

  static typename_map_type make_typedef_java()
  {
    typename_map_type define;
    define.insert(std::make_pair(s_fix_int8, "byte"));
    define.insert(std::make_pair(s_fix_int16, "short"));
    define.insert(std::make_pair(s_fix_int32, "int"));
    define.insert(std::make_pair(s_fix_int64, "long"));
    define.insert(std::make_pair(s_int8, "byte"));
    define.insert(std::make_pair(s_int16, "short"));
    define.insert(std::make_pair(s_int32, "int"));
    define.insert(std::make_pair(s_int64, "long"));
    define.insert(std::make_pair(s_float32, "float"));
    define.insert(std::make_pair(s_float64, "double"));
    define.insert(std::make_pair(s_string, "String"));
    define.insert(std::make_pair(s_list, "ArrayList"));
    define.insert(std::make_pair(s_map, "HashMap"));

    return define;
  }

  static typename_map_type make_typedef_kotlin()
  {
    typename_map_type define;
    define.insert(std::make_pair(s_fix_int8, "Byte"));
    define.insert(std::make_pair(s_fix_int16, "Short"));
    define.insert(std::make_pair(s_fix_int32, "Int"));
    define.insert(std::make_pair(s_fix_int64, "Long"));
    define.insert(std::make_pair(s_int8, "Byte"));
    define.insert(std::make_pair(s_int16, "Short"));
    define.insert(std::make_pair(s_int32, "Int"));
    define.insert(std::make_pair(s_int64, "Long"));
    define.insert(std::make_pair(s_float32, "Float"));
    define.insert(std::make_pair(s_float64, "Double"));
    define.insert(std::make_pair(s_string, "String"));
    define.insert(std::make_pair(s_list, "MutableList"));
    define.insert(std::make_pair(s_map, "MutableMap"));

    return define;
  }

  static typename_map_type make_typedef_ref_java()
  {
    typename_map_type define;
    define.insert(std::make_pair(s_fix_int8, "Byte"));
    define.insert(std::make_pair(s_fix_int16, "Short"));
    define.insert(std::make_pair(s_fix_int32, "Integer"));
    define.insert(std::make_pair(s_fix_int64, "Long"));
    define.insert(std::make_pair(s_int8, "Byte"));
    define.insert(std::make_pair(s_int16, "Short"));
    define.insert(std::make_pair(s_int32, "Integer"));
    define.insert(std::make_pair(s_int64, "Long"));
    define.insert(std::make_pair(s_float32, "Float"));
    define.insert(std::make_pair(s_float64, "Double"));
    define.insert(std::make_pair(s_string, "String"));
    define.insert(std::make_pair(s_list, "ArrayList"));
    define.insert(std::make_pair(s_map, "HashMap"));

    return define;
  }
}

typename_map_type& get_cpp_typename_map()
{
  static typename_map_type define = make_typedef_cpp();
  return define;
}

typename_map_type& get_csharp_typename_map()
{
  static typename_map_type define = make_typedef_csharp();
  return define;
}

typename_map_type& get_java_typename_map()
{
  static typename_map_type define = make_typedef_java();
  return define;
}

typename_map_type& get_java_ref_typename_map()
{
  static typename_map_type define = make_typedef_ref_java();
  return define;
}

typename_map_type& get_kotlin_typename_map()
{
  static typename_map_type define = make_typedef_kotlin();
  return define;
}

std::string is_base_type(std::string const& type)
{
  type_map_type& define = get_bast_type();
  std::string ltype = lower_case_str(type);
  type_map_type::const_iterator i = define.find(ltype);
  if (i == define.end())
  {
    return "";
  }
  return ltype;
}

e_base_type get_type(std::string const& type_)
{
  type_map_type& define = get_bast_type();
  std::string ltype = lower_case_str(type_);
  type_map_type::const_iterator i = define.find(ltype);
  if (i == define.end())
  {
    return type;
  }
  return i->second;
}