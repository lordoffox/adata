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

namespace cpp_gen
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
        // Noux Xiong: if has dot, then poof it is include type
        // repalce all '.' to "::"
        std::string ty = "::";
        return ty + boost::algorithm::replace_all_copy(name, ".", "::");
      }
      else
      {
        // Noux Xiong: none include type, just add cpp fullname in namespace
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
      type_name += "< ";
      /*if (define.m_template_parameters[0].m_type == e_base_type::type)
      {
      type_name += desc_define.m_namespace.m_cpp_fullname;
      }*/
      type_name += make_typename(desc_define, define.m_template_parameters[0].m_typename);
      if (define.m_type == e_base_type::map)
      {
        type_name += ",";
        /*if (define.m_template_parameters[1].m_type == e_base_type::type)
        {
        type_name += desc_define.m_namespace.m_cpp_fullname;
        }*/
        type_name += make_typename(desc_define, define.m_template_parameters[1].m_typename);
      }
      if (desc_define.m_option.m_cpp_allocator.length())
      {
        type_name += ",";
        type_name += desc_define.m_option.m_cpp_allocator;
      }
      type_name += " >";
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
      /*if (define.m_type == e_base_type::type)
      {
      type_name = desc_define.m_namespace.m_cpp_fullname + type_name;
      }*/
    }
    return type_name;
  }

  std::string make_type_default(const descrip_define&, const member_define& define)
  {
    switch (define.m_type)
    {
    case e_base_type::uint8:
    case e_base_type::int8:
    case e_base_type::uint16:
    case e_base_type::int16:
    case e_base_type::uint32:
    case e_base_type::int32:
    {
      return define.m_default_value;
    }
    case e_base_type::uint64:
    {
      std::string value = define.m_default_value + "ULL";
      return value;
    }
    case e_base_type::int64:
    {
      std::string value = define.m_default_value + "LL";
      return value;
    }
    case e_base_type::float32:
    {
      std::string value = define.m_default_value + "f";
      return value;
    }
    case e_base_type::float64:
    {
      std::string value = define.m_default_value;
      return value;
    }
    case e_base_type::string:
    {
      std::string value = "\"";
      value += define.m_default_value;
      value += "\"";
      return value;
    }
    default:
    {
      break;
    }
    }
    return "";
  }

  void gen_code_type(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(1) << "struct " << tdefine.m_name << std::endl << "  {" << std::endl;

    std::vector<member_define*> mb_list;

    //sort member by type to arrange layout
    for (auto& member : tdefine.m_members)
    {
      member_define * mb = (member_define *)&member;
      auto it = mb_list.begin();
      for (; it != mb_list.end(); ++it)
      {
        if (member_cmp(mb, *it) < 0)
        {
          break;
        }
      }
      mb_list.insert(it, mb);
    }

    for (const auto& mb : mb_list)
    {
      const member_define& member = *mb;
      if (member.m_deleted)
      {
        os << tabs(2) << "//" << member.m_name << " deleted , skip define." << std::endl;
        continue;
      }
      std::string type_name = make_type_desc(desc_define, member);
      os << tabs(2) << type_name << " " << member.m_name << ";" << std::endl;
    }

    os << tabs(2) << tdefine.m_name << "()" << std::endl;
    auto pos = 0;
    int init_count = 0;
    for (const auto& mb : mb_list)
    {
      const member_define& member = *mb;
      if (member.m_deleted)
      {
        continue;
      }
      if (member.is_initable())
      {
        ++init_count;
      }
    }
    if (init_count)
    {
      os << tabs(2) << ":";
    }
    for (const auto& member : mb_list)
    {
      if (member->m_deleted)
      {
        continue;
      }
      if (member->is_initable())
      {
        ++pos;
        os << tabs(2) << member->m_name;
        os << "(";
        os << make_type_default(desc_define, *member);
        os << ")";
        if (pos < init_count)
        {
          os << ",";
        }
        os << std::endl;
      }
    }
    os << tabs(2) << "{}" << std::endl;
    os << tabs(1) << "};" << std::endl << std::endl;
  }

  inline void gen_trace_error_info(std::ofstream& os, int tab_indent, const std::string& context_name, const std::string& var_name, bool trace_error = true, const std::string sub = "-1")
  {
    if (trace_error == false)
    {
      return;
    }
    os << tabs(tab_indent) << "{if(" << context_name << ".error()){" << context_name << ".trace_error(\"" << var_name << "\"," << sub << ");return;}}";
  }

  void gen_adata_operator_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (trace_error)os << std::endl;
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "uint32_t len = check_read_size(stream";
      if (mdefine.m_size.length())
      {
        os << "," << mdefine.m_size;
      }
      os << ");";
      os << std::endl;
      gen_trace_error_info(os, tab_indent + 1, "stream", mdefine.m_name, trace_error);
      if (trace_error)os << std::endl;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << var_name << ".resize(len);" << std::endl
          << tabs(tab_indent + 1) << "stream.read((char *)" << var_name << ".data(),len);";
        if (trace_error)os << std::endl;
        gen_trace_error_info(os, tab_indent + 1, "stream", mdefine.m_name, trace_error);
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".resize(len);" << std::endl;
        os << tabs(tab_indent + 1) << "for (std::size_t i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, var_name + "[i]", false);
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "(int32_t)i");
        os << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (std::size_t i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        os << tabs(tab_indent + 2) << make_type_desc(desc_define, mdefine.m_template_parameters[0]) << " first_element;" << std::endl;
        os << tabs(tab_indent + 2) << make_type_desc(desc_define, mdefine.m_template_parameters[1]) << " second_element;" << std::endl;
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "(int32_t)i");
        os << std::endl;
        os << tabs(tab_indent + 2) << var_name << ".insert(::std::make_pair(first_element,second_element));" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      os << std::endl << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent) << "{";
      if (mdefine.m_fixed)
      {
        os << "fix_";
      }
      os << "read(stream," << var_name << ");";
      gen_trace_error_info(os, 0, "stream", mdefine.m_name, trace_error);
      os << "}" << std::endl;
    }
  }

  void gen_adata_operator_read_skip_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string&, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (trace_error)os << std::endl;
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "uint32_t len = check_read_size(stream";
      if (mdefine.m_size.length())
      {
        os << "," << mdefine.m_size;
      }
      os << ");";
      os << std::endl;
      gen_trace_error_info(os, tab_indent + 1, "stream", mdefine.m_name, trace_error);
      if (trace_error)os << std::endl;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << "stream.skip_read(len);" << std::endl;
        gen_trace_error_info(os, tab_indent + 1, "stream", mdefine.m_name, trace_error);
        if (trace_error)os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "for (std::size_t i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "(int32_t)i");
        os << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (std::size_t i = 0 ; i < len ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "(int32_t)i");
        os << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent) << "{" << make_type_desc(desc_define, mdefine) << "* dummy_value = 0;";
      if (mdefine.m_fixed)
      {
        os << "fix_";
      }
      os << "skip_read(stream,dummy_value);";
      gen_trace_error_info(os, 0, "stream", mdefine.m_name, trace_error);
      os << "}" << std::endl;
    }
  }

  inline void gen_adata_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "value.";
    var_name += mdefine.m_name;

    if (mdefine.m_deleted)
    {
      gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
    }
    else
    {
      gen_adata_operator_read_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
    }
  }

  inline std::string gen_inline_code(const type_define& define)
  {
    return "inline ";
  }

  // Nous Xiong: add len tag jump
  void gen_adata_len_tag_jump(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "if(len_tag >= 0)" << std::endl;
    os << tabs(tab_indent) << "{" << std::endl;
    os << tabs(tab_indent + 1) << "::std::size_t read_len = stream.read_length() - offset;" << std::endl;
    os << tabs(tab_indent + 1) << "::std::size_t len = (::std::size_t)len_tag;" << std::endl;
    os << tabs(tab_indent + 1) << "if(len > read_len) stream.skip_read(len - read_len);" << std::endl;
    os << tabs(tab_indent) << "}" << std::endl;
  }

  // Nous Xiong: 
  void gen_adata_read_tag(std::ofstream& os, int tab_indent)
  {
    // Nous Xiong: get read offset
    os << tabs(tab_indent) << "::std::size_t offset = stream.read_length();" << std::endl;

    os << tabs(tab_indent) << "uint64_t tag = 0;" << std::endl;
    os << tabs(tab_indent) << "read(stream,tag);" << std::endl;
    os << tabs(tab_indent) << "if(stream.error()){return;}" << std::endl;

    // Nous Xiong: add len tag
    os << tabs(tab_indent) << "int32_t len_tag = 0;" << std::endl;
    os << tabs(tab_indent) << "read(stream,len_tag);" << std::endl;
    os << tabs(tab_indent) << "if(stream.error()){return;}" << std::endl;
    os << std::endl;
  }

  void gen_adata_operator_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    os << tabs(1) << "template<typename stream_ty>" << std::endl;
    os << tabs(1) << gen_inline_code(tdefine) << "void read( stream_ty& stream, " << full_type_name << "& value)" << std::endl;
    os << tabs(1) << "{" << std::endl;

    gen_adata_read_tag(os, 2);

    uint64_t tag_mask = 1;
    uint64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      os << tabs(2) << "if(tag&" << tag_mask << "ULL)";
      gen_adata_read_member_code(desc_define, tdefine, member, os, 2);
      if (member.is_multi())
      {
        std::string var_name = "value.";
        var_name += member.m_name;
        os << tabs(2) << "else {" << var_name << ".clear();}" << std::endl;
      }
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }

    // Nous Xiong: remove max mask check, for backward compat
    //os << tabs(2) << "if(tag&(~" << total_mask << "ULL)){stream.set_error_code(undefined_member_protocol_not_compatible);return;}" << std::endl;

    // Nous Xiong: add len tag jump
    gen_adata_len_tag_jump(os, 2);

    os << tabs(1) << "}" << std::endl << std::endl;
  }

  inline void gen_adata_skip_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "value.";
    var_name += mdefine.m_name;
    gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
  }

  void gen_adata_operator_skip_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    os << tabs(1) << "template <typename stream_ty>" << std::endl;
    os << tabs(1) << gen_inline_code(tdefine) << "void skip_read(stream_ty& stream, " << full_type_name << "*)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    os << tabs(2) << "skip_read_compatible(stream);" << std::endl;
    os << tabs(1) << "}" << std::endl << std::endl;
  }

  void gen_adata_operator_write_tag_code(const descrip_define&, const type_define& tdefine, std::ofstream& os, int tab_indent)
  {
    uint64_t tag = 0;
    uint64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      if (!member.is_multi() && (!member.m_deleted))
      {
        tag |= tag_mask;
      }
      tag_mask <<= 1;
    }
    tag_mask = 1;
    os << tabs(tab_indent) << "uint64_t tag = " << tag << "ULL;" << std::endl;
    for (const auto& member : tdefine.m_members)
    {
      if (member.is_multi() && (!member.m_deleted))
      {
        os << tabs(tab_indent) << "if(!value." << member.m_name << ".empty()){tag|=" << tag_mask << "ULL;}" << std::endl;
      }
      tag_mask <<= 1;
    }
  }

  void gen_member_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{" << std::endl;
      ++tab_indent;
      os << tabs(tab_indent) << "uint32_t len = (uint32_t)(" << var_name << ").size();" << std::endl;

      // Nous Xiong: remove size_of length check
      /*if (mdefine.m_size.length())
      {
      gen_size_check_write_member_code(desc_define, tdefine, mdefine, os, tab_indent, trace_error);
      }*/
      // Nous Xiong: change to size_of
      os << tabs(tab_indent) << "size += size_of(len);" << std::endl;

      if (mdefine.m_type == e_base_type::string)
      {
        // Nous Xiong: change for size_of
        os << tabs(tab_indent) << "size += len;";
        os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        // Nous Xiong: remove count bcz no error check
        //os << tabs(tab_indent) << "int32_t count = 0;" << std::endl;

        os << tabs(tab_indent) << "for (" << make_type_desc(desc_define, mdefine) << "::const_iterator i = " << var_name << ".begin() ; i != " << var_name << ".end() ; ++i)" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "*i", false);

        // Nous Xiong: rmv trace error bcz no need for that
        //gen_trace_error_info(os, tab_indent + 1, "context", mdefine.m_name, true, "count");
        //os << std::endl;

        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        // Nous Xiong: remove count bcz no error check
        //os << tabs(tab_indent) << "int32_t count = 0;" << std::endl;

        os << tabs(tab_indent) << "for (" << make_type_desc(desc_define, mdefine) << "::const_iterator i = " << var_name << ".begin() ; i != " << var_name << ".end() ; ++i)" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "i->first", false);
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "i->second", false);

        // Nous Xiong: rmv trace error bcz no need for that
        //gen_trace_error_info(os, tab_indent + 1, "context", mdefine.m_name, true, "count");
        //os << std::endl;

        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent);

      // Nous Xiong: add size += 
      os << "size += ";

      if (mdefine.m_fixed)
      {
        os << "fix_";
      }
      // Nous Xiong: change pre_write to size_of
      os << "size_of(" << var_name << ");" << std::endl;

      // Nous Xiong: rmv trace error bcz no need for that
      //gen_trace_error_info(os, tab_indent, "context", mdefine.m_name, trace_error);
      //if (trace_error)os << std::endl;
    }
  }

  void gen_adata_operator_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    os << tabs(1) << gen_inline_code(tdefine) << "int32_t size_of(const " << full_type_name << "& value)" << std::endl;
    os << tabs(1) << "{" << std::endl;

    // Nous Xiong: add size_of result var
    os << tabs(2) << "int32_t size = 0;" << std::endl;

    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    uint64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "value.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if(tag&" << tag_mask << "ULL)" << std::endl;
        }
        os << tabs(2) << "{" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, member, os, 3, var_name);
        os << tabs(2) << "}" << std::endl;
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write." << std::endl;
      }
      tag_mask <<= 1;
    }
    os << tabs(2) << "size += size_of(tag);" << std::endl;

    // Nous Xiong: add len tag
    os << tabs(2) << "size += size_of(size + size_of(size));" << std::endl;

    os << tabs(2) << "return size;" << std::endl;
    os << tabs(1) << "}" << std::endl << std::endl;
  }

  void gen_member_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (trace_error)os << std::endl;
      os << tabs(tab_indent) << "{";
      os << std::endl;
      os << tabs(tab_indent + 1) << "uint32_t len = (uint32_t)(" << var_name << ").size();" << std::endl;
      os << tabs(tab_indent + 1) << "write(stream,len);" << std::endl;

      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << "stream.write((" << var_name << ").data(),len);" << std::endl;
        gen_trace_error_info(os, tab_indent + 1, "stream", mdefine.m_name, trace_error);
        if (trace_error)os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "int32_t count = 0;" << std::endl;
        os << tabs(tab_indent + 1) << "for (" << make_type_desc(desc_define, mdefine) << "::const_iterator i = " << var_name << ".begin() ; i != " << var_name << ".end() ; ++i, ++count)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "*i", false);
        os << std::endl;
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "count");
        os << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "int32_t count = 0;" << std::endl;
        os << tabs(tab_indent + 1) << "for (" << make_type_desc(desc_define, mdefine) << "::const_iterator i = " << var_name << ".begin() ; i != " << var_name << ".end() ; ++i, ++count)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i->first", false);
        os << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "i->second", false);
        os << std::endl;
        gen_trace_error_info(os, tab_indent + 2, "stream", mdefine.m_name, true, "count");
        os << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}";
    }
    else
    {
      os << tabs(tab_indent) << "{";
      if (mdefine.m_fixed)
      {
        os << "fix_";
      }
      os << "write(stream," << var_name << ");";
      gen_trace_error_info(os, 0, "stream", mdefine.m_name, trace_error);
      os << "}";
    }
  }

  void gen_adata_operator_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = desc_define.m_namespace.m_cpp_fullname + tdefine.m_name;
    os << tabs(1) << "template<typename stream_ty>" << std::endl;
    os << tabs(1) << gen_inline_code(tdefine) << "void write(stream_ty& stream , const " << full_type_name << "&value)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    os << tabs(2) << "write(stream,tag);" << std::endl;
    os << tabs(2) << "if(stream.error()){return;}" << std::endl;

    // Nous Xiong: add len tag
    os << tabs(2) << "write(stream,size_of(value));" << std::endl;
    os << tabs(2) << "if(stream.error()){return;}" << std::endl;

    uint64_t tag_mask = 1;
    uint64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "value.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if(tag&" << tag_mask << "ULL)";
        }
        gen_member_write_type_code(desc_define, tdefine, member, os, 2, var_name);
        os << std::endl;
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write." << std::endl;
      }
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }
    os << tabs(2) << "return;" << std::endl;
    os << tabs(1) << "}" << std::endl << std::endl;

  }

  inline void gen_adata_operator_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_operator_read_type_code(desc_define, tdefine, os);
    gen_adata_operator_skip_read_type_code(desc_define, tdefine, os);
    gen_adata_operator_size_of_type_code(desc_define, tdefine, os);
    gen_adata_operator_write_type_code(desc_define, tdefine, os);
  }

  void gen_adata_operator_code(const descrip_define& desc_define, std::ofstream& os)
  {
    os << "namespace adata" << std::endl << "{" << std::endl;

    for (auto& t_define : desc_define.m_types)
    {
      gen_adata_operator_type_code(desc_define, t_define, os);
    }

    os << "}" << std::endl << std::endl;
  }

  void gen_include(const descrip_define& desc_define, std::ofstream& os)
  {
    for (auto const& inc : desc_define.m_includes)
    {
      std::string inc_path = boost::algorithm::replace_all_copy(inc.first, ".", "/");
      inc_path += ".adl.h";
      os << "#include \"" << inc_path << "\"" << std::endl;
    }
    os << std::endl;
  }

  
  void gen_traits_type(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << "template<>\nstruct is_adata<";
    for (const auto& name : desc_define.m_namespace.m_names)
    {
      os<< name << "::";
    }
    os << tdefine.m_name << ">\n{\n  static const bool value = true;\n};\n\n";
  }

  void gen_type_code(const descrip_define& desc_define, std::ofstream& os)
  {
    for (const auto& name : desc_define.m_namespace.m_names)
    {
      os << "namespace " << name << " {";
    }
    os << std::endl;

    for (auto& t_define : desc_define.m_types)
    {
      gen_code_type(desc_define, t_define, os);
    }

    for (const auto& name : desc_define.m_namespace.m_names)
    {
      os << "}";
    }
    os << std::endl << std::endl;
    os << "namespace adata\n{\n";
    for (auto& t_define : desc_define.m_types)
    {
      gen_traits_type(desc_define, t_define, os);
    }
    os << "}\n";
  }

  void	gen_code(const descrip_define& define, const std::string& cpp_file)
  {
    ofstream os(cpp_file);

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

    header_id += "_adata_header_define";
    header_id = define.m_namespace.m_lua_fullname + header_id;

    os << "#ifndef " << header_id << std::endl;
    os << "#define " << header_id << std::endl << std::endl;

    os << "#include <" << define.adata_header << "adata.hpp>" << std::endl;

    gen_include(define, os);
    gen_type_code(define, os);

    gen_adata_operator_code(define, os);

    os << "#endif" << std::endl;
    os.close();
  }

}