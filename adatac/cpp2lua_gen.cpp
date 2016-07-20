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
#include <boost/algorithm/string/replace.hpp>
#include <boost/assert.hpp>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

namespace cpp2lua_gen
{

  const std::string cpp_lang("cpp");

  inline std::string make_typename(const descrip_define& desc_define, const std::string& name)
  {
    typename_map_type& map_define = get_cpp_typename_map();
    auto find = map_define.find(name);
    if (find == map_define.end())
    {
      type_define const* ty = desc_define.find_decl_type(name);
      BOOST_ASSERT_MSG(ty != nullptr, name.c_str());

      if (name.find('.') != std::string::npos)
      {
        std::string ty = "::";
        return ty + boost::algorithm::replace_all_copy(name, ".", "::");
      }
      else
      {
        return desc_define.m_namespace.m_cpp_fullname + name;
      }
    }
    else
    {
      return find->second;
    }
  }

  std::string make_type_desc(const descrip_define& desc_define, const member_define& define)
  {
    std::string type_name;

    if (define.is_container())
    {
      type_name = make_typename(desc_define, define.m_typename);
      auto find = define.m_options.find(cpp_lang);
      if (find != define.m_options.end())
      {
        type_name = find->second;
      }
      type_name += "<";
      type_name += make_typename(desc_define, define.m_template_parameters[0].m_typename);
      if (define.m_type == e_base_type::map)
      {
        type_name += ",";
        type_name += make_typename(desc_define, define.m_template_parameters[1].m_typename);
      }
      if (desc_define.m_option.m_cpp_allocator.length())
      {
        type_name += ",";
        type_name += desc_define.m_option.m_cpp_allocator;
      }
      type_name += ">";
    }
    else if (define.m_type == e_base_type::string)
    {
      type_name = make_typename(desc_define, define.m_typename);
      if (desc_define.m_option.m_cpp_allocator.length())
      {
        type_name = "::std::basic_string<<char, char_traits<char>,";
        type_name += desc_define.m_option.m_cpp_allocator;
        type_name += " > ";
      }
    }
    else
    {
      type_name = make_typename(desc_define, define.m_typename);
    }
    return type_name;
  }

  inline std::string gen_inline_code(const type_define& define)
  {
    /*
    if (!define.m_ismulti)
    {
      return "ADATA_INLINE ";
    }
    */
    return "ADATA_INLINE ";
  }

  void gen_adata_operator_load_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name , bool lua_pop = true)
  {
    if (mdefine.is_container())
    {
      os << tabs(tab_indent) << "{" << std::endl;
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "int len = (int)seq_len(L,-1);" << std::endl;
        os << tabs(tab_indent + 1) << var_name << ".resize(len);" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        os << tabs(tab_indent + 2) << "lua_rawgeti(L, -1, i+1);" << std::endl;
        gen_adata_operator_load_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, var_name + "[i]");
        os << tabs(tab_indent + 1) << "}" << std::endl;
        os << tabs(tab_indent + 1) << "lua_pop(L,1);" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "lua_pushnil(L);" << std::endl;
        os << tabs(tab_indent + 1) << "while(lua_next(L,-2) != 0)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        os << tabs(tab_indent + 2) << make_type_desc(desc_define, mdefine.m_template_parameters[0]) << " first_element;" << std::endl;
        os << tabs(tab_indent + 2) << make_type_desc(desc_define, mdefine.m_template_parameters[1]) << " second_element;" << std::endl;
        gen_adata_operator_load_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element");
        gen_adata_operator_load_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element" , false);
        os << tabs(tab_indent + 2) << var_name << ".insert(::std::make_pair(first_element,second_element));" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
        os << tabs(tab_indent + 1) << "lua_pop(L,1);" << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent) << "{";
      os << "load(L, " << var_name << ");";
      if (lua_pop)
      {
        os << "lua_pop(L, 1);";
      }
      os << "}" << std::endl;
    }
  }

  inline void gen_adata_load_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "value.";
    var_name += mdefine.m_name;

    if (!mdefine.m_deleted)
    {
      os << tabs(tab_indent) << "lua_getfield(L, -1, \"" << mdefine.m_name << "\");" << std::endl;
      gen_adata_operator_load_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
    }
  }

  void gen_adata_operator_load_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    os << tabs(2) << gen_inline_code(tdefine) << "void load( lua_State * L, " << full_type_name << "& value)" << std::endl;
    os << tabs(2) << "{" << std::endl;

    for (const auto& member : tdefine.m_members)
    {
      gen_adata_load_member_code(desc_define, tdefine, member, os, 3);
    }

    os << tabs(2) << "}" << std::endl << std::endl;
  }

  void gen_adata_operator_push_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name)
  {
    if (mdefine.is_container())
    {
      os << tabs(tab_indent) << "{" << std::endl;
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "int len = (int)" <<  var_name << ".size();" << std::endl;
        os << tabs(tab_indent + 1) << "lua_createtable(L, 0, len);" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_push_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, var_name + "[i]");
        os << tabs(tab_indent + 2) << "lua_rawseti(L, -2, i+1);" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "lua_createtable(L, 0, (int)" << var_name << ".size());" << std::endl;
        os << tabs(tab_indent + 1) << "for (" << make_type_desc(desc_define, mdefine) << "::const_iterator i = " << var_name << ".begin() ; i != " << var_name << ".end() ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_push_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i->first");
        gen_adata_operator_push_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "i->second");
        os << tabs(tab_indent + 2) << "lua_settable(L,-3);" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent) << "{";
      // Nous Xiong: add use_adata arg
      if (mdefine.m_type == e_base_type::type)
      {
        os << "push(L, " << var_name << ", use_adata);";
      }
      else
      {
        os << "push(L, " << var_name << ");";
      }
      os << "}" << std::endl;
    }
  }

  inline void gen_adata_push_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "value.";
    var_name += mdefine.m_name;

    if (!mdefine.m_deleted)
    {
      gen_adata_operator_push_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
      os << tabs(tab_indent) << "lua_setfield(L, -2, \"" << mdefine.m_name << "\");" << std::endl;
    }
  }

  void gen_adata_operator_push_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    // Nous Xiong: add use_adata arg
    os << tabs(2) << gen_inline_code(tdefine) << "void push( lua_State * L, " << full_type_name << " const& value, bool use_adata = true)" << std::endl;
    os << tabs(2) << "{" << std::endl;

    os << tabs(3) << "lua_createtable(L, 0, " << tdefine.m_members.size() << ");" << std::endl;
    std::string ns = desc_define.m_namespace.m_fullname;
    ns.append(".");
    ns.append(tdefine.m_name);
    // Nous Xiong: add use_adata arg
    os << tabs(3) << "if(use_adata && !set_metatable(L, \"ad.mt." << ns << "\")){ luaL_error(L,\"unknow type: " << ns << "\"); }" << std::endl;
    for (const auto& member : tdefine.m_members)
    {
      gen_adata_push_member_code(desc_define, tdefine, member, os, 3);
    }

    os << tabs(2) << "}" << std::endl << std::endl;
  }

  inline void gen_adata_operator_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_operator_load_type_code(desc_define, tdefine, os);
    gen_adata_operator_push_type_code(desc_define, tdefine, os);
  }

  void gen_adata_operator_code(const descrip_define& desc_define, std::ofstream& os)
  {
    os << "namespace adata" << std::endl << "{" << std::endl << tabs(1) << "namespace lua" << std::endl << tabs(1) << "{" << std::endl;

    for (auto& t_define : desc_define.m_types)
    {
      gen_adata_operator_type_code(desc_define, t_define, os);
    }

    os << tabs(1) << "}" << std::endl << "}" << std::endl << std::endl;
  }

  void gen_include(const descrip_define& desc_define, std::ofstream& os)
  {
    for (auto const& inc : desc_define.m_includes)
    {
      std::string inc_path = boost::algorithm::replace_all_copy(inc.first, ".", "/");
      os << "#include \"" << inc_path << ".adl.c2l.h" << "\"" << std::endl;
    }
    os << std::endl;
  }


  void	gen_code(const descrip_define& define, const std::string& cpp_file)
  {
    ofstream os(cpp_file + ".c2l.h");

    std::string header_id = cpp_file;

    auto path_pos = std::string::npos;
    for (auto i = header_id.length() - 1; i >= 0; --i)
    {
      auto& c = header_id[i];
      if (c == '.')
      {
        c = '_';
      }
      if (c == '\\' || c == '/')
      {
        path_pos = i;
        break;
      }
    }
    if (path_pos != std::string::npos)
    {
      header_id = header_id.substr(path_pos + 1);
    }

    header_id += "_cpp2lua_h_";
    header_id = define.m_namespace.m_lua_fullname + header_id;

    os << "#ifndef " << header_id << std::endl;
    os << "#define " << header_id << std::endl << std::endl;

    os << "#include <" << define.adata_header  << "adata_cpp2lua.hpp>" << std::endl;
    gen_include(define, os);
    path_pos = std::string::npos;
    std::string include_file = cpp_file;
    for (auto i = include_file.length() - 1; i >= 0; --i)
    {
      auto& c = include_file[i];
      if (c == '\\' || c == '/')
      {
        path_pos = i;
        break;
      }
    }
    if (path_pos != std::string::npos)
    {
      include_file = include_file.substr(path_pos + 1);
    }
    os << "#include \"" << include_file << ".h\"" << std::endl << std::endl;

    gen_adata_operator_code(define, os);

    os << "#endif" << std::endl;
    os.close();
  }

}