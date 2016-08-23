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
#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <ctime>
#include <direct.h>

using namespace std;

namespace java_gen
{

  const std::string lang("java");

  void mk_ns_dirs(std::string ns)
  {
    size_t len = ns.length();
    const char * path = ns.c_str();
    for (size_t i = 0; i < len; ++i)
    {
      char c = ns[i];
      if(c == '/' || c == '\\')
      {
        ns[i] = 0;
        mkdir(path);
        ns[i] = '/';
      }
    }
  }

  void check_java_type(e_base_type type)
  {
    switch (type)
    {
    case e_base_type::fix_uint8: throw std::logic_error("java not support uint8 type!");
    case e_base_type::fix_uint16: throw std::logic_error("java not support uint16 type!");
    case e_base_type::fix_uint32: throw std::logic_error("java not support uint32 type!");
    case e_base_type::fix_uint64: throw std::logic_error("java not support uint64 type!");
    case e_base_type::uint8: throw std::logic_error("java not support uint8 type!");
    case e_base_type::uint16: throw std::logic_error("java not support uint16 type!");
    case e_base_type::uint32: throw std::logic_error("java not support uint32 type!");
    case e_base_type::uint64: throw std::logic_error("java not support uint64 type!");
    }
  }

  inline std::string make_typename(const descrip_define& desc_define, const std::string& name , bool is_ref = false)
  {
    typename_map_type * map_define = &get_java_typename_map();
    if (is_ref)
    {
      map_define = &get_java_ref_typename_map();
    }
    auto find = map_define->find(name);
    if (find == map_define->end())
    {
      type_define const* ty = desc_define.find_decl_type(name);
      assert(ty != nullptr);
      return name;
    }
    return find->second;
  }

  std::string make_type_desc(const descrip_define& desc_define, const member_define& define , bool is_ref = false)
  {
    check_java_type(define.m_type);
    std::string type_name;

    if (define.is_container())
    {
      type_name = make_typename(desc_define, define.m_typename);
      type_name += "<";
      check_java_type(define.m_template_parameters[0].m_type);
      type_name += make_typename(desc_define, define.m_template_parameters[0].m_typename, true);
      if (define.m_type == e_base_type::map)
      {
        check_java_type(define.m_template_parameters[1].m_type);
        type_name += ",";
        type_name += make_typename(desc_define, define.m_template_parameters[1].m_typename, true);
      }
      type_name += ">";
    }
    else
    {
      type_name = make_typename(desc_define, define.m_typename,is_ref);
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
    case e_base_type::float64:
    {
      return define.m_default_value;
    }
    case e_base_type::uint64:
    case e_base_type::int64:
    {
      return define.m_default_value + "L";
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

  std::string gen_java_default_value(const member_define& mdefine, const std::string& decl_type)
  {
    switch (mdefine.m_type)
    {
    case e_base_type::int8:
    case e_base_type::int16:
    case e_base_type::int32:
    {
      return "0";
    }
    case e_base_type::int64:
    {
      return "0L";
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

  std::string gen_java_opt_name(const member_define& mdefine, std::string prefix)
  {
    
    if (mdefine.m_fixed)
    {
      prefix[0] = upper_case_char(prefix[0]);
      prefix = "fix" + prefix;
    }
    switch (mdefine.m_type)
    {
    case e_base_type::int8: return prefix + "Int8"; 
    case e_base_type::int16:return prefix + "Int16";
    case e_base_type::int32:return prefix + "Int32";
    case e_base_type::int64:return prefix + "Int64";
    case e_base_type::float32:return prefix + "Float";
    case e_base_type::float64:return prefix + "Double";
    default:
    {
      break;
    }
    }
    return "";
  }

  std::string gen_check_read_size(const member_define& mdefine)
  {
    std::string str = "= stream.checkReadSize(";
    if (mdefine.m_size.length())
    {
      str += mdefine.m_size;
    }
    else
    {
      str += "0";
    }
    str += ");";
    return std::move(str);
  }

  void gen_adata_operator_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "int len" << tab_indent << gen_check_read_size(mdefine) << std::endl;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << var_name << " = stream.readString(len"<< tab_indent << ");";
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent + 2) << decl_type << " element";
        std::string decl_value = gen_java_default_value(mdefine.m_template_parameters[0], decl_type);
        if (!decl_value.empty())
        {
          os << "= " << decl_value;
        }
        os << "; " << std::endl;
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        os << tabs(tab_indent + 2) << var_name << ".add(element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent + 2) << decl_type1 << " first_element";
        std::string decl_value1 = gen_java_default_value(mdefine.m_template_parameters[0], decl_type1);
        if (!decl_value1.empty())
        {
          os << "= " << decl_value1;
        }
        os << "; " << std::endl;
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent + 2) << decl_type2 << " second_element";
        std::string decl_value2 = gen_java_default_value(mdefine.m_template_parameters[1], decl_type2);
        if (!decl_value2.empty())
        {
          os << "= " << decl_value2;
        }
        os << "; " << std::endl;

        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        os << tabs(tab_indent + 2) << var_name << ".put(first_element,second_element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      os << std::endl << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if(mdefine.m_type == e_base_type::type)
      {
        os << tabs(tab_indent) << var_name << ".read(stream);" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << var_name << " = stream." << gen_java_opt_name(mdefine, "read") << "();" << std::endl;;
      }
    }
  }

  void gen_adata_operator_read_skip_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{";
      os << std::endl << tabs(tab_indent + 1)
        << "int len" << tab_indent << gen_check_read_size(mdefine) << std::endl;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << "stream.skipRead(len" << tab_indent << ");" << std::endl;
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
        os << tabs(tab_indent) << "stream." << gen_java_opt_name(mdefine, "skipRead") << "();" << std::endl;;
      }
      else
      {
        os << "stream.skipReadCompatible();";
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
    os << tabs(tab_indent + 1) << "int read_len = (int)(stream.readLength() - offset);" << std::endl;
    os << tabs(tab_indent + 1) << "if(len_tag > read_len) stream.skipRead(len_tag - read_len);" << std::endl;
    os << tabs(tab_indent) << "}" << std::endl;
  }

  void gen_adata_read_tag(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "int offset = stream.readLength();" << std::endl;
    os << tabs(tab_indent) << "long tag = stream.readInt64();" << std::endl;
    os << tabs(tab_indent) << "int len_tag = stream.readInt32();" << std::endl;
    os << std::endl;
  }

  void gen_adata_operator_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_read_tag(os, 2);

    int64_t tag_mask = 1;
    int64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      os << tabs(2) << "if((tag&" << tag_mask << "L)>0)";
      gen_adata_read_member_code(desc_define, tdefine, member, os, 2);
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }

    gen_adata_len_tag_jump(os, 2);
  }

  inline void gen_adata_skip_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "this.";
    var_name += mdefine.m_name;
    gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
  }

  void gen_adata_operator_skip_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(2) << "stream.skipReadCompatible();" << std::endl;
  }

  inline void gen_size_check_write_member_code(const descrip_define&, const type_define&, const member_define& mdefine, std::ofstream& os, int tab_indent, bool trace_error = true)
  {
    if(mdefine.m_size.length() > 0)
    {
      os << tabs(tab_indent+1) << "if(len" << tab_indent << ">" << mdefine.m_size
        << "){ throw new RuntimeException(\"number of element not match.\");}" << std::endl;
    }
  }

  void gen_adata_operator_write_tag_code(const descrip_define&, const type_define& tdefine, std::ofstream& os, int tab_indent)
  {
    int64_t tag = 0;
    int64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      if (!member.is_multi() && (!member.m_deleted))
      {
        tag |= tag_mask;
      }
      tag_mask <<= 1;
    }
    tag_mask = 1;
    os << tabs(tab_indent) << "long tag = " << tag << "L;" << std::endl;
    for (const auto& member : tdefine.m_members)
    {
      if (member.m_deleted == 0)
      {
        if (member.is_container())
        {
          os << tabs(tab_indent) << "if(this." << member.m_name << ".size() > 0){tag|=" << tag_mask << "L;}" << std::endl;
        }
        else if (member.m_type == e_base_type::string)
        {
          os << tabs(tab_indent) << "if(this." << member.m_name << ".length() > 0){tag|=" << tag_mask << "L;}" << std::endl;
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
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfString(" << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent) << "for(" << decl_type << " v : " <<var_name << ") {" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "v", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent) << "for(java.util.HashMap.Entry<" << decl_type1 << "," << decl_type2 << "> it : " << var_name << ".entrySet()) {" << std::endl;
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "it.getKey()", false);
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "it.getValue()", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      --tab_indent;
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "size += adata.Stream." << gen_java_opt_name(mdefine, "sizeOf") << "(" << var_name << ");" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << "size += "<< var_name << ".sizeOf();" << std::endl;;
      }
    }
  }

  void gen_adata_operator_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(2) << "int size = 0;" << std::endl;

    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    int64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if((tag&" << tag_mask << "L)>0)" << std::endl;
        }
        gen_member_size_of_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write." << std::endl;
      }
      tag_mask <<= 1;
    }
    os << tabs(2) << "size += adata.Stream.sizeOfInt64(tag);" << std::endl;

    os << tabs(2) << "size += adata.Stream.sizeOfInt32(size + adata.Stream.sizeOfInt32(size));" << std::endl;

    os << tabs(2) << "return size;" << std::endl;
  }

  void gen_member_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{";
      os << std::endl;

      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << "stream.writeString(" << var_name;
        if(mdefine.m_size.length() > 0)
        {
          os << "," << mdefine.m_size;
        }
        else
        {
          os << ",0";
        }
        os << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        gen_size_check_write_member_code(desc_define, tdefine, mdefine, os, tab_indent, false);
        os << tabs(tab_indent + 1) << "stream.writeInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent + 1) << "for(" << decl_type << " v : " << var_name << ") {" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "v", false);
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        gen_size_check_write_member_code(desc_define, tdefine, mdefine, os, tab_indent, false);
        os << tabs(tab_indent + 1) << "stream.writeInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent + 1) << "for(java.util.HashMap.Entry<" << decl_type1 << "," << decl_type2 << "> it : " << var_name << ".entrySet()) {" << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "it.getKey()", false);
        os << std::endl;
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "it.getValue()", false);
        os << tabs(tab_indent + 1) << "}" << std::endl;
      }
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "stream." << gen_java_opt_name(mdefine, "write") << "(" << var_name << ");" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << var_name << ".write(stream);" << std::endl;;
      }
    }
  }

  void gen_adata_operator_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    os << tabs(2) << "stream.writeInt64(tag);" << std::endl;
    os << tabs(2) << "stream.writeInt32(this.sizeOf());" << std::endl;

    int64_t tag_mask = 1;
    int64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if((tag&" << tag_mask << "L)>0)";
        }
        gen_member_write_type_code(desc_define, tdefine, member, os, 2, var_name);
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
        << "int len" << tab_indent << gen_check_read_size(mdefine) << std::endl;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << var_name << " = stream.readString(len" << tab_indent << ");";
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent + 2) << decl_type << " element";
        std::string decl_value = gen_java_default_value(mdefine.m_template_parameters[0], decl_type);
        if (!decl_value.empty())
        {
          os << "= " << decl_value;
        }
        os << "; " << std::endl;
        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "element", false);
        os << tabs(tab_indent + 2) << var_name << ".add(element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear();" << std::endl;
        os << tabs(tab_indent + 1) << "for (int i = 0 ; i < len" << tab_indent << " ; ++i)" << std::endl;
        os << tabs(tab_indent + 1) << "{" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent + 2) << decl_type1 << " first_element";
        std::string decl_value1 = gen_java_default_value(mdefine.m_template_parameters[0], decl_type1);
        if (!decl_value1.empty())
        {
          os << "= " << decl_value1;
        }
        os << "; " << std::endl;
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent + 2) << decl_type2 << " second_element";
        std::string decl_value2 = gen_java_default_value(mdefine.m_template_parameters[1], decl_type2);
        if (!decl_value2.empty())
        {
          os << "= " << decl_value2;
        }
        os << "; " << std::endl;

        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "first_element", false);
        gen_adata_operator_raw_read_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "second_element", false);
        os << tabs(tab_indent + 2) << var_name << ".put(first_element,second_element);" << std::endl;
        os << tabs(tab_indent + 1) << "}";
      }
      os << std::endl << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if (mdefine.m_type == e_base_type::type)
      {
        os << tabs(tab_indent) << var_name << ".rawRead(stream);" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << var_name << " = stream." << gen_java_opt_name(mdefine, "read") << "();" << std::endl;;
      }
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
    for (const auto& member : tdefine.m_members)
    {
      gen_adata_raw_read_member_code(desc_define, tdefine, member, os, 2);
    }
  }

  void gen_member_raw_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{" << std::endl;
      ++tab_indent;
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfString(" << var_name << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent) << "for(" << decl_type << " v : " << var_name << ") {" << std::endl;
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "v", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        os << tabs(tab_indent) << "size += adata.Stream.sizeOfInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent) << "for(java.util.HashMap.Entry<" << decl_type1 << "," << decl_type2 << "> it : " << var_name << ".entrySet()) {" << std::endl;
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "it.getKey()", false);
        gen_member_raw_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "it.getValue()", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      --tab_indent;
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "size += adata.Stream." << gen_java_opt_name(mdefine, "sizeOf") << "(" << var_name << ");" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << "size += " << var_name << ".rawSizeOf();" << std::endl;;
      }
    }
  }

  void gen_adata_operator_raw_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(2) << "int size = 0;" << std::endl;

    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (member.m_deleted == false)
      {
        gen_member_raw_size_of_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
    }
    os << tabs(2) << "return size;" << std::endl;
  }

  void gen_member_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{" << std::endl;
      ++tab_indent;

      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "stream.writeString(" << var_name;
        if (mdefine.m_size.length() > 0)
        {
          os << "," << mdefine.m_size;
        }
        else
        {
          os << ",0";
        }
        os << ");" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        gen_size_check_write_member_code(desc_define, tdefine, mdefine, os, tab_indent, false);
        os << tabs(tab_indent) << "stream.writeInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        os << tabs(tab_indent) << "for(" << decl_type << " v : " << var_name << ") {" << std::endl;
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "v", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "int len" << tab_indent << " = " << var_name << ".size();" << std::endl;
        gen_size_check_write_member_code(desc_define, tdefine, mdefine, os, tab_indent, false);
        os << tabs(tab_indent) << "stream.writeInt32(len" << tab_indent << ");" << std::endl;
        std::string decl_type1 = make_type_desc(desc_define, mdefine.m_template_parameters[0],true);
        std::string decl_type2 = make_type_desc(desc_define, mdefine.m_template_parameters[1],true);
        os << tabs(tab_indent) << "for(java.util.HashMap.Entry<" << decl_type1 << "," << decl_type2 << "> it : " << var_name << ".entrySet()) {" << std::endl;
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "it.getKey()", false);
        gen_member_raw_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "it.getValue()", false);
        os << tabs(tab_indent) << "}" << std::endl;
      }
      --tab_indent;
      os << tabs(tab_indent) << "}" << std::endl;
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "stream." << gen_java_opt_name(mdefine, "write") << "(" << var_name << ");" << std::endl;;
      }
      else
      {
        os << tabs(tab_indent) << var_name << ".rawWrite(stream);" << std::endl;;
      }
    }
  }

  void gen_adata_operator_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "this.";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        gen_member_raw_write_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
    }
  }

  void gen_code_type(const descrip_define& desc_define, const type_define& tdefine, std::string const& path)
  {
    std::ofstream os(path + tdefine.m_name + ".java");
    os << "package " << desc_define.m_namespace.m_fullname << ";" << std::endl;
    {

      bool has_list = false;
      bool has_map = false;
      for (auto& member : tdefine.m_members)
      {
        member_define * mb = (member_define *)&member;
        if(mb->m_deleted)
        {
          continue;
        }
        if(mb->m_type == e_base_type::list)
        {
          has_list = true;
        }
        else if (mb->m_type == e_base_type::map)
        {
          has_map = true;
        }
      }
      if (has_list)
        os << "import java.util.ArrayList;" << std::endl;
      if (has_map)
        os << "import java.util.HashMap;" << std::endl;
      if (has_list || has_map)
        os << std::endl;
    }

    os << "public class " << tdefine.m_name << " extends adata.Base  {" << std::endl;

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
        os << tabs(1) << "//" << member.m_name << " deleted , skip define." << std::endl;
        continue;
      }
      std::string type_name = make_type_desc(desc_define, member);
      os << tabs(1) << "public " << type_name << " " << member.m_name;
      if (member.is_initable())
      {
        os << " = " << make_type_default(desc_define, member);
      }
      else if (member.m_type == e_base_type::string)
      {
        os << " = " << "\"\"";
      }
      else
      {
        os << " = " << "new " << type_name << "()";
      }
      os << ";" << std::endl;
    }

    os << tabs(1) << "public " << tdefine.m_name << "()" << std::endl;
    os << tabs(1) << "{" << std::endl;
    size_t mcount = mb_list.size();
    for (const auto& mb : mb_list)
    {
      --mcount;
      const member_define& member = *mb;
      if (member.m_deleted)
      {
        continue;
      }
      std::string type_name = make_type_desc(desc_define, member);
      os << tabs(2) << member.m_name;
      if (member.is_initable())
      {
        os << " = " << make_type_default(desc_define, member);
      }
      else if (member.m_type == e_base_type::string)
      {
        os << " = " << "\"\"";
      }
      else
      {
        os << " = " << "new " << type_name << "()";
      }
      os << ";" << std::endl;
    }

    os << tabs(1) << "}" << std::endl;

    for (const auto& mb : mb_list)
    {
      --mcount;
      const member_define& member = *mb;
      if (member.m_deleted)
      {
        continue;
      }
      std::string type_name = make_type_desc(desc_define, member);
      std::string mname = member.m_name;
      if (mname.length() > 0)
      {
        mname[0] = upper_case_char(mname[0]);
      }
      os << tabs(1) << "public " << type_name << " get" << mname << "(){" << std::endl;
      os << tabs(2) << "return this." << member.m_name << ";" << std::endl;
      os << tabs(1) << "}" << std::endl;
      os << tabs(1) << "public void set" << mname << "(" << type_name << " value){" << std::endl;
      os << tabs(2) << "this." << member.m_name << " = value;" << std::endl;
      os << tabs(1) << "}" << std::endl;
    }

    os << tabs(1) << "public void read(adata.Stream stream)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_read_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public void skipRead(adata.Stream stream)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_skip_read_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public int sizeOf()" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_size_of_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public void write(adata.Stream stream)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_write_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public void rawRead(adata.Stream stream)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_raw_read_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public int rawSizeOf()" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_raw_size_of_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << tabs(1) << "public void rawWrite(adata.Stream stream)" << std::endl;
    os << tabs(1) << "{" << std::endl;
    gen_adata_operator_raw_write_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}" << std::endl;

    os << "}" << std::endl << std::endl;
  }

  void gen_type_code(const descrip_define& desc_define, std::string const& path)
  {
    std::string full_path = desc_define.m_namespace.m_fullname;
    full_path.append(".");
    full_path = replace_all_copy(full_path, ".", "/");
    full_path = path + full_path;
    mk_ns_dirs(full_path);

    for (auto& t_define : desc_define.m_types)
    {
      gen_code_type(desc_define, t_define, full_path);
    }
  }

  void	gen_code(const descrip_define& define, const std::string& java_file)
  {
    try
    {
      gen_type_code(define, java_file);
      //gen_adata_operator_code(define, os);
    }
    catch (std::exception& e)
    {
      std::cerr << "gen java code error:" << e.what() << std::endl;
    }
  }

}