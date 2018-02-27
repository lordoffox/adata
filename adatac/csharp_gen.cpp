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
#include <assert.h>
#include <set>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

namespace csharp_gen
{

  const std::string cpp_lang("csharp");

  inline std::string make_typename(const descrip_define& desc_define, const std::string& name)
  {
    typename_map_type& map_define = get_csharp_typename_map();
    auto find = map_define.find(name);
    if (find == map_define.end())
    {
      type_define const* ty = desc_define.find_decl_type(name);
      assert(ty != nullptr);
      return name;
    }
    return find->second;
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
      type_name += ">";
    }
    else
    {
      type_name = make_typename(desc_define, define.m_typename);
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
    case e_base_type::uint64:
    case e_base_type::int64:
    case e_base_type::float64:
    {
      return define.m_default_value;
    }
    case e_base_type::float32:
    {
      return define.m_default_value + "F";
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
    os << tabs(1) << "public partial class " << tdefine.m_name << ": adata.base_obj" << std::endl << "  {" << std::endl;

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
      os << tabs(2) << "public " << type_name << " " << member.m_name;
      if (member.is_initable())
      {
        os << " = " << make_type_default(desc_define, member);
      }
      else if (member.m_type == e_base_type::string)
      {
        os << " = " << "\"\"";
      }
      else if (member.m_type == e_base_type::buffer)
      {
        os << " = " << "new byte[0]";
      }
      else
      {
        os << " = " << "new " << type_name << "()";
      }
      os << ";" << std::endl;
    }

    os << tabs(1) << "}" << std::endl << std::endl;
  }

  inline void gen_trace_error_info(std::ofstream& os, int tab_indent, const std::string& context_name, const std::string& var_name, bool trace_error = true, const std::string sub = "-1")
  {
  }

  std::string gen_csharp_default_value(const member_define& mdefine, const std::string& decl_type)
  {
    switch (mdefine.m_type)
    {
    case e_base_type::uint8:
    case e_base_type::int8:
    case e_base_type::uint16:
    case e_base_type::int16:
    case e_base_type::uint32:
    case e_base_type::int32:
    case e_base_type::uint64:
    case e_base_type::int64:
    {
      return "0";
    }
    case e_base_type::float32:
    {
      return "0.0F";
    }
    case e_base_type::float64:
    {
      return "0.0";
    }
    case e_base_type::string:
    {
      return "\"\"";
    }
    case e_base_type::buffer:
    {
      return "new byte[0]";
    }
    case e_base_type::type:
    {
      std::string value = "new " + decl_type + "()";
      return value;
    }
    default:
    {
      break;
    }
    }
    return "";
  }

  std::string gen_stream_ns(const member_define& mdefine)
  {
    auto s = mdefine.m_typedef->m_name.find_last_of('.');
    std::string ns;
    if (s != std::string::npos)
    {
      ns = mdefine.m_typedef->m_name.substr(0, s + 1);
      ns += mdefine.m_typedef->m_filename;
      ns += "_stream.";
    }
    return std::move(ns);
  }

  void gen_adata_operator_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "Int32 len" << tab_indent << " = adata.stream.check_read_size(stream";
      if (mdefine.m_size.length())
      {
        os << "," << mdefine.m_size;
      }
      os << ");";
      os << std::endl;
      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent + 1) << "adata.stream.read(stream,ref " << var_name << ",len" << tab_indent << ");";
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".Clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 2) << decl_type << " element";
        std::string decl_value = gen_csharp_default_value(mdefine.m_template_parameters[0], decl_type);
        if (!decl_value.empty())
        {
          os << "= " << decl_value;
        }
        os << "; " << std::endl;
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        os << tabs(tab_indent + 2) << var_name << ".Add(element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << var_name << ".Clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 2) << decl_type1 << " first_element";
        std::string decl_value1 = gen_csharp_default_value(mdefine.m_template_parameters[0], decl_type1);
        if (!decl_value1.empty())
        {
          os << "= " << decl_value1;
        }
        os << "; " << std::endl;
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1]);
        os << tabs(tab_indent + 2) << decl_type2 << " second_element";
        std::string decl_value2 = gen_csharp_default_value(mdefine.m_template_parameters[1], decl_type2);
        if (!decl_value2.empty())
        {
          os << "= " << decl_value2;
        }
        os << "; " << std::endl;

        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        os << tabs(tab_indent + 2) << var_name << ".Add(first_element,second_element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      os << std::endl << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent);
      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "read(stream,ref " << var_name << ");";
      }
      else
      {
        os << var_name << ".read(stream);";
      }
      os << std::endl;
    }
  }

  void gen_adata_operator_read_skip_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (trace_error)os << std::endl;
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "Int32 len" << tab_indent << " = adata.stream.check_read_size(stream";
      if (mdefine.m_size.length())
      {
        os << "," << mdefine.m_size;
      }
      os << ");";
      os << std::endl;
      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent + 1) << "stream.skip_read(len" << tab_indent << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent) << "{" << make_type_desc(desc_define, mdefine) << " dummy_value = ";
      if (mdefine.is_initable())
      {
        os << "0";
      }
      else
      {
        os << "null";
      }
      os << ";";
      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "skip_read(stream,ref dummy_value);";
      }
      else
      {
        os << "adata.stream.skip_read_compatible(stream);";
      }
      os << "}" << std::endl;
    }
  }

  inline void gen_adata_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "this.";
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

  void gen_adata_len_tag_jump(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "if(len_tag >= 0)" << std::endl;
    os << tabs(tab_indent) << "{" << std::endl;
    os << tabs(tab_indent + 1) << "Int32 read_len = stream.read_length() - offset;" << std::endl;
    os << tabs(tab_indent + 1) << "if(len_tag > read_len) stream.skip_read((UInt32)(len_tag - read_len));" << std::endl;
    os << tabs(tab_indent) << "}" << std::endl;
  }

  void gen_adata_read_tag(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "int offset = stream.read_length();" << std::endl;
    os << tabs(tab_indent) << "Int64 tag = 0;" << std::endl;
    os << tabs(tab_indent) << "adata.stream.read(stream,ref tag);" << std::endl;
    os << tabs(tab_indent) << "Int32 len_tag = 0;" << std::endl;
    os << tabs(tab_indent) << "adata.stream.read(stream,ref len_tag);" << std::endl;
    os << std::endl;
  }

  void gen_adata_operator_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;
    gen_adata_read_tag(os, 3);

    uint64_t tag_mask = 1;
    uint64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      os << tabs(3) << "if((tag&" << tag_mask << "L)>0)";
      gen_adata_read_member_code(desc_define, tdefine, member, os, 3);
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }

    gen_adata_len_tag_jump(os, 3);
  }

  inline void gen_adata_skip_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "this.";
    var_name += mdefine.m_name;
    gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
  }

  void gen_adata_operator_skip_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;
    os << tabs(3) << "adata.stream.skip_read_compatible(stream);" << std::endl;
  }

  inline void gen_size_check_write_member_code(const descrip_define&, const type_define&, const member_define& mdefine, std::ofstream& os, int tab_indent, bool trace_error = true)
  {
    os << tabs(tab_indent) << "if(len" << tab_indent << ">" << mdefine.m_size
      << "){ stream.error_code = (adata.error_code_t.number_of_element_not_macth);}";
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
    os << tabs(tab_indent) << "Int64 tag = " << tag << "L;" << std::endl;
    for (const auto& member : tdefine.m_members)
    {
      if (member.m_deleted == 0)
      {
        if (member.is_container())
        {
          os << tabs(tab_indent) << "if(this." << member.m_name << ".Count > 0){tag|=" << tag_mask << "L;}" << std::endl;
        }
        else if (member.m_type == e_base_type::string || member.m_type == e_base_type::buffer)
        {
          os << tabs(tab_indent) << "if(this." << member.m_name << ".Length > 0){tag|=" << tag_mask << "L;}" << std::endl;
        }
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
      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent) << "size += adata.stream.size_of(" << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent) << "size += adata.stream.size_of(len" << tab_indent << ");" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent) << "foreach (" << decl_type << " i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "i", false);

        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent) << "size += adata.stream.size_of(len" << tab_indent << ");" << std::endl;

        os << tabs(tab_indent) << "foreach (var i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "i.Key", false);
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "i.Value", false);

        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
      --tab_indent;
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent);
      os << "size += ";

      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "size_of(" << var_name << ");" << std::endl;
      }
      else
      {
        auto stream_ns = gen_stream_ns(mdefine);
        os << var_name << ".size_of();" << std::endl;
      }
    }
  }

  void gen_adata_operator_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;
    os << tabs(3) << "Int32 size = 0;" << std::endl;

    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 3);
    uint64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(3) << "if((tag&" << tag_mask << "L)>0)" << std::endl;
        }
        gen_member_size_of_type_code(desc_define, tdefine, member, os, 3, var_name);
      }
      else
      {
        os << tabs(3) << "//" << var_name << " deleted , skip write." << std::endl;
      }
      tag_mask <<= 1;
    }
    os << tabs(3) << "size += adata.stream.size_of(tag);" << std::endl;

    os << tabs(3) << "size += adata.stream.size_of(size + adata.stream.size_of(size));" << std::endl;

    os << tabs(3) << "return size;" << std::endl;
  }

  void gen_member_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{" << std::endl;

      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent + 1) << "adata.stream.write(stream," << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent + 1) << "adata.stream.write(stream,len" << tab_indent << ");" << std::endl;
        os << tabs(tab_indent + 1) << "int count = 0;" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 1) << "foreach (" << decl_type << " i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i", false);
        os << tabs(tab_indent + 2) << "++count;" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent + 1) << "adata.stream.write(stream,len" << tab_indent << ");" << std::endl;
        os << tabs(tab_indent + 1) << "int count = 0;" << std::endl;
        os << tabs(tab_indent + 1) << "foreach (var i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i.Key", false);
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "i.Value", false);
        os << tabs(tab_indent + 2) << "++count;" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}"<<std::endl;
    }
    else
    {
      os << tabs(tab_indent);
      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "write(stream," << var_name << ");";
      }
      else
      {
        os << var_name << ".write(stream);";
      }
      os << std::endl;
    }
  }

  void gen_adata_operator_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;
    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 3);
    os << tabs(3) << "adata.stream.write(stream,tag);" << std::endl;
    os << tabs(3) << "adata.stream.write(stream,this.size_of());" << std::endl;

    uint64_t tag_mask = 1;
    uint64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(3) << "if((tag&" << tag_mask << "L)>0)";
        }
        gen_member_write_type_code(desc_define, tdefine, member, os, 3, var_name);
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write." << std::endl;
      }
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }
  }

  void gen_adata_operator_raw_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "Int32 len" << tab_indent << " = adata.stream.check_read_size(stream";
      if (mdefine.m_size.length())
      {
        os << "," << mdefine.m_size;
      }
      os << ");";
      os << std::endl;
      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent + 1) << "adata.stream.read(stream,ref " << var_name << ",len" << tab_indent << ");";
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".Clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 2) << decl_type << " element";
        std::string decl_value = gen_csharp_default_value(mdefine.m_template_parameters[0], decl_type);
        if (!decl_value.empty())
        {
          os << "= " << decl_value;
        }
        os << "; " << std::endl;
        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        os << tabs(tab_indent + 2) << var_name << ".Add(element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << var_name << ".Clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 2) << decl_type1 << " first_element";
        std::string decl_value1 = gen_csharp_default_value(mdefine.m_template_parameters[0], decl_type1);
        if (!decl_value1.empty())
        {
          os << "= " << decl_value1;
        }
        os << "; " << std::endl;
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1]);
        os << tabs(tab_indent + 2) << decl_type2 << " second_element";
        std::string decl_value2 = gen_csharp_default_value(mdefine.m_template_parameters[1], decl_type2);
        if (!decl_value2.empty())
        {
          os << "= " << decl_value2;
        }
        os << "; " << std::endl;

        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        os << tabs(tab_indent + 2) << var_name << ".Add(first_element,second_element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      os << std::endl << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      os << tabs(tab_indent);
      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "read(stream,ref " << var_name << ");";
      }
      else
      {
        os << var_name << ".raw_read(stream);";
      }
      os << std::endl;
    }
  }

  inline void gen_adata_raw_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "this.";
    var_name += mdefine.m_name;

    if (mdefine.m_deleted == false)
    {
      gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
    }
  }

  void gen_adata_operator_raw_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;

    for (const auto& member : tdefine.m_members)
    {
      gen_adata_raw_read_member_code(desc_define, tdefine, member, os, 3);
    }
  }

  void gen_member_raw_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent) << "size += adata.stream.size_of(" << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "size += adata.stream.size_of(" << var_name << ".Count); " << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent) << "foreach (" << decl_type << " i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "i", false);
        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "size += adata.stream.size_of(" << var_name << ".Count); " << std::endl;
        os << tabs(tab_indent) << "foreach (var i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent) << "{" << std::endl;
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "i.Key", false);
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "i.Value", false);
        os << tabs(tab_indent) << "}";
        os << std::endl;
      }
    }
    else
    {
      os << tabs(tab_indent);
      os << "size += ";

      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "size_of(" << var_name << ");" << std::endl;
      }
      else
      {
        os << var_name << ".raw_size_of();" << std::endl;
      }
    }
  }

  void gen_adata_operator_raw_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;

    os << tabs(3) << "Int32 size = 0;" << std::endl;

    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (member.m_deleted == false)
      {
        gen_member_raw_size_of_type_code(desc_define, tdefine, member, os, 3, var_name);
      }
    }
    os << tabs(3) << "return size;" << std::endl;
  }

  void gen_member_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{" << std::endl;

      if (mdefine.m_type == e_base_type::string || mdefine.m_type == e_base_type::buffer)
      {
        os << tabs(tab_indent + 1) << "adata.stream.write(stream," << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent + 1) << "adata.stream.write(stream,len" << tab_indent << ");" << std::endl;
        os << tabs(tab_indent + 1) << "int count = 0;" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0]);
        os << tabs(tab_indent + 1) << "foreach (" << decl_type << " i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i", false);
        os << tabs(tab_indent + 2) << "++count;" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "Int32 len" << tab_indent << " = " << var_name << ".Count;" << std::endl;
        os << tabs(tab_indent + 1) << "adata.stream.write(stream,len" << tab_indent << ");" << std::endl;
        os << tabs(tab_indent + 1) << "int count = 0;" << std::endl;
        os << tabs(tab_indent + 1) << "foreach (var i in " << var_name << ")" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "i.Key", false);
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "i.Value", false);
        os << tabs(tab_indent + 2) << "++count;" << std::endl;
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}"<<std::endl;
    }
    else
    {
      os << tabs(tab_indent);
      if (mdefine.m_type != e_base_type::type)
      {
        os << "adata.stream.";
        if (mdefine.m_fixed)
        {
          os << "fix_";
        }
        os << "write(stream," << var_name << ");";
      }
      else
      {
        os << var_name << ".raw_write(stream);";
      }
      os << std::endl;
    }
  }

  void gen_adata_operator_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    std::string full_type_name = tdefine.m_name;

    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        gen_member_raw_write_type_code(desc_define, tdefine, member, os, 3, var_name);
      }
    }
  }

  void gen_adata_operator_code(const descrip_define& desc_define, std::ofstream& os)
  {
    for (auto& tdefine : desc_define.m_types)
    {
      os << tabs(1) << "public partial class " << tdefine.m_name << std::endl << "  {" << std::endl;

      os << tabs(2) << "public override void read(adata.zero_copy_buffer stream)" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_read_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

//       os << tabs(2) << "public void skip_read(adata.zero_copy_buffer stream)" << std::endl << tabs(2) << "{" << std::endl;
//       gen_adata_operator_skip_read_type_code(desc_define, tdefine, os);
//       os << tabs(2) << "}" << std::endl << std::endl;
// 
      os << tabs(2) << "public override Int32 size_of()" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_size_of_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

      os << tabs(2) << "public override void write(adata.zero_copy_buffer stream)" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_write_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

      os << tabs(2) << "public override void raw_read(adata.zero_copy_buffer stream)" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_raw_read_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

      os << tabs(2) << "public override Int32 raw_size_of()" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_raw_size_of_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

      os << tabs(2) << "public override void raw_write(adata.zero_copy_buffer stream)" << std::endl << tabs(2) << "{" << std::endl;
      gen_adata_operator_raw_write_type_code(desc_define, tdefine, os);
      os << tabs(2) << "}" << std::endl << std::endl;

      os << tabs(1) << "}" << std::endl << std::endl;
    }
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

  }

  const char * using_define = R"(using System;
using System.IO;
using System.Collections.Generic;
using adata;

)";

  void	gen_code(const descrip_define& define, const std::string& csharp_file)
  {
    ofstream os(csharp_file);

    os << using_define;

    gen_type_code(define, os);

    gen_adata_operator_code(define, os);

    for (const auto& name : define.m_namespace.m_names)
    {
      os << "}";
    }
    os << std::endl << std::endl;

    os.close();
  }

}