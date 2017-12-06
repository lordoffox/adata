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

namespace kotlin_gen
{

  const std::string lang("kotlin");

  static inline void check_kt_type(e_base_type type)
  {
    switch (type)
    {
    case e_base_type::fix_uint8: throw std::logic_error("kotlin not support uint8 type!");
    case e_base_type::fix_uint16: throw std::logic_error("kotlin not support uint16 type!");
    case e_base_type::fix_uint32: throw std::logic_error("kotlin not support uint32 type!");
    case e_base_type::fix_uint64: throw std::logic_error("kotlin not support uint64 type!");
    case e_base_type::uint8: throw std::logic_error("kotlin not support uint8 type!");
    case e_base_type::uint16: throw std::logic_error("kotlin not support uint16 type!");
    case e_base_type::uint32: throw std::logic_error("kotlin not support uint32 type!");
    case e_base_type::uint64: throw std::logic_error("kotlin not support uint64 type!");
    }
  }

  static inline std::string make_typename(const descrip_define& desc_define, const std::string& name)
  {
    typename_map_type * map_define = &get_kotlin_typename_map();
    auto find = map_define->find(name);
    if (find == map_define->end())
    {
      type_define const* ty = desc_define.find_decl_type(name);
      assert(ty != nullptr);
      return name;
    }
    return find->second;
  }

  static std::string make_type_desc(const descrip_define& desc_define, const member_define& define)
  {
    check_kt_type(define.m_type);
    std::string type_name;

    if (define.is_container())
    {
      type_name = make_typename(desc_define, define.m_typename);
      type_name += "<";
      check_kt_type(define.m_template_parameters[0].m_type);
      type_name += make_typename(desc_define, define.m_template_parameters[0].m_typename);
      if (define.m_type == e_base_type::map)
      {
        check_kt_type(define.m_template_parameters[1].m_type);
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

  static std::string make_type_default(const descrip_define&, const member_define& define)
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

  static std::string gen_check_size(const member_define& mdefine)
  {
    if (mdefine.m_size.length())
    {
      return mdefine.m_size;
    }
    else
    {
      return "0";
    }
  }

  std::string gen_kotlin_opt_name(const member_define& mdefine, std::string prefix)
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

  std::string gen_kotlin_read_element(const descrip_define& desc_define, const member_define& mdefine)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = "readElement(stream)";
      //str += make_type_desc(desc_define, mdefine);
      //str += ">(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = "stream.readString(";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    std::string str = "stream.";
    return str + gen_kotlin_opt_name(mdefine, "read") + "()";
  }

  std::string gen_kotlin_read_value(const member_define& mdefine)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = ".read(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = " = stream.readString(";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    std::string str = " = stream.";
    return str + gen_kotlin_opt_name(mdefine, "read") + "()";
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
    str += ")";
    return std::move(str);
  }

  static void gen_adata_operator_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << var_name << " = stream.readString(" << gen_check_size(mdefine) << ")\n";
        return;
      }

      os << tabs(tab_indent) << "{\n";
      os << tabs(tab_indent + 1) << "val len" << gen_check_read_size(mdefine) << "\n";
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear()\n";
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        os << tabs(tab_indent + 2) << var_name << ".add(" << gen_kotlin_read_element(desc_define, mdefine.m_template_parameters[0]) << ")\n";
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        os << tabs(tab_indent + 2) << var_name << "[" << gen_kotlin_read_element(desc_define, mdefine.m_template_parameters[0]) << "] = "
          << gen_kotlin_read_element(desc_define, mdefine.m_template_parameters[1]) << "\n";
        os << tabs(tab_indent + 1) << "}";
      }
      os << "\n" << tabs(tab_indent) << "}\n";
    }
    else
    {
      os << tabs(tab_indent) << var_name << gen_kotlin_read_value(mdefine) << "\n";
    }
  }

  void gen_adata_operator_read_skip_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    if (mdefine.is_multi())
    {
      os << tabs(tab_indent) << "{\n";
      os << tabs(tab_indent + 1) << "var len" << gen_check_read_size(mdefine) << "\n";
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent + 1) << "stream.skipRead(len" << tab_indent << ")\n";
      }
      else if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2);
        os << tabs(tab_indent + 1) << "}\n";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2);
        gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2);
        os << tabs(tab_indent + 1) << "}\n";
      }
      os << tabs(tab_indent) << "}\n";
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "stream." << gen_kotlin_opt_name(mdefine, "skipRead") << "()\n";
      }
      else
      {
        os << "stream.skipReadCompatible()\n";
      }
    }
  }

  inline void gen_adata_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = mdefine.m_name;
    if (mdefine.m_deleted)
    {
      gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent);
    }
    else
    {
      gen_adata_operator_read_member_code(desc_define, tdefine, mdefine, os, tab_indent, var_name);
    }
  }

  void gen_adata_len_tag_jump(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "if(lenTag >= 0){\n";
    os << tabs(tab_indent + 1) << "val readLen = stream.readLength() - offset\n";
    os << tabs(tab_indent + 1) << "if(lenTag > readLen) stream.skipRead(lenTag - readLen)\n";
    os << tabs(tab_indent) << "}\n";
  }

  void gen_adata_read_tag(std::ofstream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "val offset = stream.readLength()\n";
    os << tabs(tab_indent) << "val tag = stream.readInt64()\n";
    os << tabs(tab_indent) << "val lenTag = stream.readInt32()\n\n";
  }

  void gen_adata_operator_read_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_read_tag(os, 2);

    int64_t tag_mask = 1;
    int64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      os << tabs(2) << "if(tag and " << tag_mask << "L>0)";
      gen_adata_read_member_code(desc_define, tdefine, member, os, 2);
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }

    gen_adata_len_tag_jump(os, 2);
  }

  inline void gen_adata_skip_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    gen_adata_operator_read_skip_member_code(desc_define, tdefine, mdefine, os, tab_indent);
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
    os << tabs(tab_indent) << "var tag = " << tag << "L\n";
    for (const auto& member : tdefine.m_members)
    {
      if (member.m_deleted == 0)
      {
        if (member.is_container())
        {
          os << tabs(tab_indent) << "if(" << member.m_name << ".isNotEmpty()){tag = tag or " << tag_mask << "L}\n";
        }
        else if (member.m_type == e_base_type::string)
        {
          os << tabs(tab_indent) << "if(" << member.m_name << ".isNotEmpty()){tag = tag or " << tag_mask << "L}\n";
        }
      }
      tag_mask <<= 1;
    }
  }

  void gen_member_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ")\n";
        return;
      }

      os << tabs(tab_indent) << "{\n";
      ++tab_indent;
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ".size)\n";
        os << tabs(tab_indent) << "for(v in " << var_name << ") {\n";
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "v");
        os << tabs(tab_indent) << "}\n";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ".size)\n"; 
        os << tabs(tab_indent) << "for((key,value) in " << var_name << ") {\n";
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "key");
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "value");
        os << tabs(tab_indent) << "}\n";
      }
      --tab_indent;
      os << tabs(tab_indent) << "}\n";
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ")\n";
      }
      else
      {
        os << tabs(tab_indent) << "size += " << var_name << ".sizeOf()\n";
      }
    }
  }

  void gen_adata_operator_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(2) << "var size = 0\n";

    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    int64_t tag_mask = 1;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if(tag and " << tag_mask << "L>0)";
        }
        gen_member_size_of_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write.\n";
      }
      tag_mask <<= 1;
    }
    os << tabs(2) << "size += sizeOf(tag)\n";
    os << tabs(2) << "size += sizeOf(size + sizeOf(size))\n";
    os << tabs(2) << "return size\n";
  }

  std::string gen_kotlin_write_value(const member_define& mdefine,
    std::string const& var_name)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = var_name + ".write(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = "stream.writeString(";
      str += var_name;
      str += ",";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    if (mdefine.m_fixed)
    {
      std::string str = "stream.fixWrite(";
      return str + var_name + ")";
    }
    std::string str = "stream.write(";
    return str + var_name + ")";
  }

  void gen_member_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "stream.writeString(" << var_name << "," << gen_check_size(mdefine) << ")\n";
        return;
      }
      os << tabs(tab_indent) << "{\n";
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "val len = " << var_name << ".size\n";
        os << tabs(tab_indent + 1) << "checkWriteSize(" << var_name << ".size,len)\n";
        os << tabs(tab_indent + 1) << "stream.write(len)\n";
        os << tabs(tab_indent + 1) << "for(v in " << var_name << ") {\n";
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "v");
        os << tabs(tab_indent + 1) << "}\n";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "val len = " << var_name << ".size\n";
        os << tabs(tab_indent + 1) << "checkWriteSize(" << var_name << ".size,len)\n";
        os << tabs(tab_indent + 1) << "stream.write(len)\n";
        os << tabs(tab_indent + 1) << "for((key,value) in " << var_name << ") {\n";
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "key");
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "value");
        os << tabs(tab_indent + 1) << "}\n";
      }
      os << tabs(tab_indent) << "}\n";
    }
    else
    {
      os << tabs(tab_indent) << gen_kotlin_write_value(mdefine, var_name) << "\n";
    }
  }

  void gen_adata_operator_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    gen_adata_operator_write_tag_code(desc_define, tdefine, os, 2);
    os << tabs(2) << "stream.write(tag)\n";
    os << tabs(2) << "stream.write(sizeOf())\n";

    int64_t tag_mask = 1;
    int64_t total_mask = 0;
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = member.m_name;
      if (!member.m_deleted)
      {
        if (member.is_multi())
        {
          os << tabs(2) << "if(tag and " << tag_mask << "L>0)";
        }
        gen_member_write_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
      else
      {
        os << tabs(2) << "//" << var_name << " deleted , skip write.\n";
      }
      total_mask |= tag_mask;
      tag_mask <<= 1;
    }
  }

  std::string gen_kotlin_raw_opt_name(const member_define& mdefine, std::string prefix)
  {
    switch (mdefine.m_type)
    {
    case e_base_type::int8:
    case e_base_type::int16:
    case e_base_type::int32:
    case e_base_type::int64:
    {
      prefix[0] = upper_case_char(prefix[0]);
      prefix = "fix" + prefix;
    }
    default:
    {
      break;
    }
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

  std::string gen_kotlin_raw_read_element(const descrip_define& desc_define, const member_define& mdefine)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = "rawReadElement(stream)";
      //str += make_type_desc(desc_define, mdefine);
      //str += ">(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = "stream.readString(";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    std::string str = "stream.";
    return str + gen_kotlin_opt_name(mdefine, "read") + "()";
  }

  std::string gen_kotlin_raw_read_value(const member_define& mdefine, std::string prefix)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = ".rawRead(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = " = stream.readString(";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    std::string str = " = stream.";
    return str + gen_kotlin_opt_name(mdefine, prefix) + "()";
  }


  void gen_adata_operator_raw_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << var_name << " = stream.readString(" << gen_check_size(mdefine)<< ")\n";
        return;
      }

      os << tabs(tab_indent) << "run{\n";
      os << tabs(tab_indent + 1) << "val len" << gen_check_read_size(mdefine) << "\n";
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << var_name << ".clear()\n";
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        os << tabs(tab_indent + 2) << var_name << ".add(" << gen_kotlin_raw_read_element(desc_define, mdefine.m_template_parameters[0]) << ")\n";
        os << tabs(tab_indent + 1) << "}";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "for (i in 0 until len){\n";
        os << tabs(tab_indent + 2) << var_name << "[" << gen_kotlin_raw_read_element(desc_define, mdefine.m_template_parameters[0]) << "] = "
          << gen_kotlin_raw_read_element(desc_define, mdefine.m_template_parameters[1]) << "\n";
        os << tabs(tab_indent + 1) << "}";
      }
      os << "\n" << tabs(tab_indent) << "}\n";
    }
    else
    {
      os << tabs(tab_indent) << var_name << gen_kotlin_raw_read_value(mdefine, "read") << "\n";
    }
  }

  inline void gen_adata_raw_read_member_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent)
  {
    std::string var_name = "";
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
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "size += fixSizeOf(" << var_name << ")\n";
        return;
      }
      os << tabs(tab_indent) << "run{\n";
      ++tab_indent;
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ".size)\n";
        os << tabs(tab_indent) << "for(v in " << var_name << ") {\n";
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "v");
        os << tabs(tab_indent) << "}\n";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ".size)\n";
        os << tabs(tab_indent) << "for((key,value) in " << var_name << ") {\n";
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 1, "key");
        gen_member_size_of_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 1, "value");
        os << tabs(tab_indent) << "}\n";
      }
      --tab_indent;
      os << tabs(tab_indent) << "}\n";
    }
    else
    {
      if (mdefine.m_type != e_base_type::type)
      {
        os << tabs(tab_indent) << "size += sizeOf(" << var_name << ")\n";
      }
      else
      {
        os << tabs(tab_indent) << "size += " << var_name << ".rawSizeOf()\n";
      }
    }
  }

  void gen_adata_operator_raw_size_of_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << tabs(2) << "var size = 0\n";

    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "";
      var_name += member.m_name;
      if (member.m_deleted == false)
      {
        gen_member_raw_size_of_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
    }
    os << tabs(2) << "return size\n";
  }

  std::string gen_kotlin_raw_write_value(const member_define& mdefine,
    std::string const& var_name)
  {
    if (mdefine.m_type == e_base_type::type)
    {
      std::string str = var_name + ".rawWrite(stream)";
      return str;
    }
    else if (mdefine.m_type == e_base_type::string)
    {
      std::string str = "stream.writeString(";
      str += var_name;
      str += ",";
      str += gen_check_size(mdefine);
      return str + ")";
    }

    if (mdefine.m_fixed)
    {
      std::string str = "stream.fixWrite(";
      return str + var_name + ")";
    }
    std::string str = "stream.write(";
    return str + var_name + ")";
  }

  void gen_member_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, const member_define& mdefine, std::ofstream& os, int tab_indent, const std::string& var_name, bool trace_error = true)
  {
    if (mdefine.is_multi())
    {
      if (mdefine.m_type == e_base_type::string)
      {
        os << tabs(tab_indent) << "stream.writeString(" << var_name << "," << gen_check_size(mdefine) << ")\n";
        return;
      }
      os << tabs(tab_indent) << "run{\n";
      if (mdefine.m_type == e_base_type::list)
      {
        os << tabs(tab_indent + 1) << "val len = " << var_name << ".size\n";
        os << tabs(tab_indent + 1) << "checkWriteSize(" << var_name << ".size,len)\n";
        os << tabs(tab_indent + 1) << "stream.write(len)\n";
        os << tabs(tab_indent + 1) << "for(v in " << var_name << ") {\n";
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "v");
        os << tabs(tab_indent + 1) << "}\n";
      }
      else if (mdefine.m_type == e_base_type::map)
      {
        os << tabs(tab_indent + 1) << "val len = " << var_name << ".size\n";
        os << tabs(tab_indent + 1) << "checkWriteSize(" << var_name << ".size,len)\n";
        os << tabs(tab_indent + 1) << "stream.write(len)\n";
        os << tabs(tab_indent + 1) << "for((key,value) in " << var_name << ") {\n";
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[0], os, tab_indent + 2, "key");
        gen_member_write_type_code(desc_define, tdefine, mdefine.m_template_parameters[1], os, tab_indent + 2, "value");
        os << tabs(tab_indent + 1) << "}\n";
      }
      os << tabs(tab_indent) << "}\n";
    }
    else
    {
      os << tabs(tab_indent) << gen_kotlin_raw_write_value(mdefine, var_name) << "\n";
    }
  }

  void gen_adata_operator_raw_write_type_code(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    for (const auto& member : tdefine.m_members)
    {
      std::string var_name = "";
      var_name += member.m_name;
      if (!member.m_deleted)
      {
        gen_member_raw_write_type_code(desc_define, tdefine, member, os, 2, var_name);
      }
    }
  }

  void gen_code_type(const descrip_define& desc_define, const type_define& tdefine, std::ofstream& os)
  {
    os << "class " << tdefine.m_name << " : Base  {\n";

    for (auto const& member : tdefine.m_members)
    {
      if (member.m_deleted)
      {
        os << tabs(1) << "//" << member.m_name << " deleted , skip define.\n";
        continue;
      }
      std::string type_name = make_type_desc(desc_define, member);
      os << tabs(1) << "var " << member.m_name << ":" << type_name;
      if (member.is_initable())
      {
        os << " = " << make_type_default(desc_define, member);
      }
      else if (member.m_type == e_base_type::string)
      {
        os << " = " << "\"\"";
      }
      else if (member.m_type == e_base_type::list)
      {
        os << " = " << "mutableListOf()";
      }
      else if (member.m_type == e_base_type::map)
      {
        os << " = " << "mutableMapOf()";
      }
      else
      {
        os << " = " << type_name << "()";
      }
      os << "\n";
    }
    os << "\n";
    os << tabs(1) << "override fun read(stream: Stream){\n";
    gen_adata_operator_read_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << tabs(1) << "override fun sizeOf(): Int {\n";
    gen_adata_operator_size_of_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << tabs(1) << "override fun write(stream: Stream) {\n";
    gen_adata_operator_write_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << tabs(1) << "override fun rawRead(stream: Stream) {\n";
    gen_adata_operator_raw_read_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << tabs(1) << "override fun rawSizeOf(): Int {\n";
    gen_adata_operator_raw_size_of_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << tabs(1) << "override fun rawWrite(stream: Stream) {\n";
    gen_adata_operator_raw_write_type_code(desc_define, tdefine, os);
    os << tabs(1) << "}\n";

    os << "}\n\n";
  }

  void gen_type_code(const descrip_define& desc_define, std::string const& path)
  {
    std::ofstream os(path);
    os << "package " << desc_define.m_namespace.m_fullname;
    os << "\nimport adata.*\n\n";

    for (auto& t_define : desc_define.m_types)
    {
      gen_code_type(desc_define, t_define, os);
    }
  }

  void	gen_code(const descrip_define& define, const std::string& file)
  {
    try
    {
      gen_type_code(define, file);
    }
    catch (std::exception& e)
    {
      std::cerr << "gen kotlin code error:" << e.what() << std::endl;
    }
  }

}