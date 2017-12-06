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

#include <string>
#include <vector>
#include <map>

struct namespace_type
{
  typedef std::vector<std::string>	namespace_name_list_type;
  namespace_name_list_type					m_names;				//
  std::string												m_fullname;			//packeage name
  std::string												m_cpp_fullname;	//in C++: n1::n2::n3
  std::string												m_csharp_fullname;	//in C#: n1.n2.n3
  std::string												m_lua_fullname;	//in lua: n1.n2.n3
  std::string												m_js_fullname;	//in js: n1_n2_n3
};

struct option_type
{
  std::string										m_cpp_allocator;			//defalut is ::std::allocator<char>
  option_type()
  {
  }
};

enum e_base_type
{
  unknow,
  fix_int8,
  fix_uint8,
  fix_int16,
  fix_uint16,
  fix_int32,
  fix_uint32,
  fix_int64,
  fix_uint64,
  int8,
  uint8,
  int16,
  uint16,
  int32,
  uint32,
  int64,
  uint64,
  float32,
  float64,
  string,
  list,
  map,
  type,
  max_count
};

e_base_type get_type(std::string const& type);

inline bool is_container(e_base_type type)
{
  switch (type)
  {
  case list:
  case map:
    return true;
  default:
  {
    break;
  }
  }
  return false;
}

struct type_define;

struct member_define
{
  typedef std::map<std::string, std::string>	option_map_type;
  typedef std::vector<member_define>					template_parameter_list_type;
  std::string										m_name;
  e_base_type										m_type;
  std::string										m_typename;
  option_map_type								m_options;
  std::string										m_size;
  template_parameter_list_type	m_template_parameters;
  std::string										m_default_value;
  type_define *                 m_typedef;
  bool													m_fixed;
  bool													m_deleted;
  int														m_tag;
  int														m_parser_lines;
  int														m_parser_cols;
  std::string                   m_parser_include;

  member_define()
    : m_type(unknow)
    , m_typedef(NULL)
    , m_fixed(false)
    , m_deleted(false)
    , m_tag(0)
    , m_parser_lines(0)
    , m_parser_cols(0)
  {
  }

  inline bool	is_fixed()const
  {
    if (m_type >= fix_int8 && m_type <= fix_uint64)
    {
      return true;
    }
    return false;
  }

  inline bool	is_integer()const
  {
    if (m_type >= int8 && m_type <= uint64)
    {
      return true;
    }
    return false;
  }

  inline bool	is_float()const
  {
    if (m_type >= float32 && m_type <= float64)
    {
      return true;
    }
    return false;
  }

  inline bool	is_container()const
  {
    return ::is_container(m_type);
  }

  inline bool	is_multi()const
  {
    return ::is_container(m_type) || m_type == e_base_type::string;
  }

  inline bool is_initable()const
  {
    if (m_type >= int8 && m_type < string)
    {
      return true;
    }
    return false;
  }

};

struct type_define
{
  typedef std::vector<member_define>	member_list_type;
  std::string										m_name;
  member_list_type							m_members;
  int														m_parser_lines;
  int														m_parser_cols;
  std::string                   m_parser_include;
  int                           m_index;
  bool                          m_ismulti;
  std::string                   m_filename;
  struct cpp_opt
  {
    bool force_inline = true;
  }m_cpp_opt;
  type_define()
    : m_parser_lines(0)
    , m_parser_cols(0)
    , m_index(0)
    , m_ismulti(false)
  {
  }

  bool has_member(const std::string& member_name) const;
};

struct option_value
{
  std::string m_value;
  int					m_parser_lines;
  int					m_parser_cols;
  std::string m_parser_include;

  option_value()
    : m_parser_lines(0)
    , m_parser_cols(0)
  {
  }
};

struct descrip_define;
struct include_define
{
  std::string m_name;
  namespace_type m_namespace;
  descrip_define const& m_descrip;

  explicit include_define(descrip_define const& descrip)
    : m_descrip(descrip)
  {
  }
};

struct descrip_define
{
  typedef std::vector<type_define>	type_list_type;
  typedef std::map<std::string, option_value> option_map_type;
  typedef std::map<std::string, include_define> include_map_type;

  include_map_type m_includes;
  namespace_type m_namespace;
  option_type m_option;
  option_map_type m_option_values;
  type_list_type m_types;
  // Nous Xiong: add include type_defines
  type_list_type m_include_types;
  // Nous Xiong: add filename
  std::string m_filename;
  // Nous Xiong: adata.hpp include path
  std::string adata_header;

  bool has_decl_type(const std::string& type) const;
  const type_define* find_decl_type(const std::string& type) const;
};

typedef std::map<std::string, std::string> typename_map_type;

typename_map_type& get_cpp_typename_map();
typename_map_type& get_csharp_typename_map();
typename_map_type& get_java_typename_map();
typename_map_type& get_java_ref_typename_map();
typename_map_type& get_kotlin_typename_map();