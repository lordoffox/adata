///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include "adata.hpp"
#include "descrip.h"
#include "util.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include <ctime>
#include <memory>

using namespace std;

namespace adt_gen
{

  struct string_pool
  {
    std::map<std::string, uint32_t>	pool_map;
    std::string			data;
    uint32_t id(const std::string& name)
    {
      uint32_t offset = 0;
      auto pos = pool_map.find(name);
      if (pos == pool_map.end())
      {
        offset = (uint32_t)pool_map.size();
        pool_map.insert(std::make_pair(name, offset));
      }
      else
      {
        offset = pos->second;
      }
      return (uint32_t)offset;
    }
  };

  inline void encode_default_value(const member_define& mdefine, adata::zero_copy_buffer& zbuf, string_pool& spool)
  {
    if (mdefine.m_deleted == true)
    {
      return;
    }
    switch (mdefine.m_type)
    {
    case e_base_type::fix_int8:
    case e_base_type::fix_int16:
    case e_base_type::fix_int32:
    case e_base_type::fix_int64:
    case e_base_type::int8:
    case e_base_type::int16:
    case e_base_type::int32:
    case e_base_type::int64:
    {
      int64_t value = 0;
      if (mdefine.m_default_value.length() > 0)
      {
        value = std::strtoll(mdefine.m_default_value.c_str(),nullptr,10);
      }
      adata::write(zbuf, value);
      break;
    }
    case e_base_type::fix_uint8:
    case e_base_type::fix_uint16:
    case e_base_type::fix_uint32:
    case e_base_type::fix_uint64:
    case e_base_type::uint8:
    case e_base_type::uint16:
    case e_base_type::uint32:
    case e_base_type::uint64:
    {
      uint64_t value = 0;
      if (mdefine.m_default_value.length() > 0)
      {
        value = std::strtoull(mdefine.m_default_value.c_str(),nullptr,10);
      }
      adata::write(zbuf, value);
      break;
    }
    case e_base_type::float32:
    {
      float value = 0;
      if (mdefine.m_default_value.length() > 0)
      {
        value = std::strtof(mdefine.m_default_value.c_str(),nullptr);
      }
      adata::write(zbuf, value);
      break;
    }
    case e_base_type::float64:
    {
      double value = 0;
      if (mdefine.m_default_value.length() > 0)
      {
        value = std::strtod(mdefine.m_default_value.c_str(), nullptr);
      }
      adata::write(zbuf, value);
      break;
    }
    default:
      break;
    }
  }

  inline int32_t find_type_namespace_idx(const descrip_define& define, const std::string& name)
  {
    for (size_t i = 0; i < define.m_types.size(); ++i)
    {
      if (name == define.m_types[i].m_name)
      {
        return (int32_t)i;
      }
    }
    return -1;
  }

  inline int32_t real_type(bool fixed , e_base_type type)
  {
    if (fixed)
    {
      return type - (e_base_type::int8 - e_base_type::fix_int8);
    }
    return type;
  }

  inline FILE * open_file(const std::string& filename , const char * mode)
  {
    FILE * fp = std::fopen(filename.c_str(), mode);
    if (fp == nullptr)
    {
      std::string err_str = "can't open file:";
      err_str += filename;
      throw std::exception(err_str.c_str());
    }
    return fp;
  }

  void gen_code(const descrip_define& define, const std::string& adt_file)
  {
    std::string ns = define.m_namespace.m_fullname;

    FILE * fp = open_file(adt_file,"wb");

    const size_t buffer_size = 1024 * 1024;

    unsigned char  * buff = new unsigned char[buffer_size];
    std::unique_ptr<unsigned char> buffer_graud(buff);

    adata::zero_copy_buffer zbuf;
    zbuf.set_write(buff, buffer_size);
    adata::write(zbuf, int32_t(1));
    adata::write(zbuf, ns);

    string_pool spool;

    for (auto& t_define : define.m_types)
    {
      spool.id(t_define.m_name);
      for (auto& m_define : t_define.m_members)
      {
        spool.id(m_define.m_name);
        if (m_define.m_type == e_base_type::type)
        {
          spool.id(m_define.m_typename);
        }
        for (const auto& ptype : m_define.m_template_parameters)
        {
          if (ptype.m_type == e_base_type::type)
          {
            spool.id(ptype.m_typename);
          }
        }
      }
    }

    uint32_t str_pool_count = (uint32_t)spool.pool_map.size();
    adata::write(zbuf, str_pool_count);

    std::vector<std::string> str_list;
    str_list.resize(spool.pool_map.size());
    for (auto& i : spool.pool_map)
    {
      str_list[i.second] = i.first;
    }
    for (auto& i : str_list)
    {
      adata::write(zbuf, i);
    }

    uint32_t define_count = (uint32_t)define.m_types.size();
    adata::write(zbuf, define_count);
    for (auto& t_define : define.m_types)
    {
      uint32_t member_count = 0;
      uint32_t param_type_count = 0;
      member_count = (uint32_t)t_define.m_members.size();
      for (auto& m_define : t_define.m_members)
      {
        param_type_count += (uint32_t)m_define.m_template_parameters.size();
      }
      adata::write(zbuf, member_count);
      adata::write(zbuf, param_type_count);
      adata::write(zbuf, spool.id(t_define.m_name));
      for (auto& m_define : t_define.m_members)
      {
        adata::write(zbuf, spool.id(m_define.m_name));
        adata::write(zbuf, real_type(m_define.m_fixed,m_define.m_type));
        if (m_define.m_type == e_base_type::type)
        {
          adata::write(zbuf, spool.id(m_define.m_typename));
          int32_t namespace_idx = find_type_namespace_idx(define, m_define.m_typename);
          adata::write(zbuf, namespace_idx);
        }
        adata::write(zbuf, (int32_t)m_define.m_deleted);
        encode_default_value(m_define, zbuf, spool);
        int32_t size = atoi(m_define.m_size.c_str());
        adata::write(zbuf, size);
        size = (int32_t)m_define.m_template_parameters.size();
        adata::write(zbuf, size);
        for (const auto& ptype : m_define.m_template_parameters)
        {
          adata::write(zbuf, real_type(ptype.m_fixed,ptype.m_type));
          if (ptype.m_type == e_base_type::type)
          {
            adata::write(zbuf, spool.id(ptype.m_typename));
            int32_t namespace_idx = find_type_namespace_idx(define, ptype.m_typename);
            adata::write(zbuf, namespace_idx);
          }          
          size = atoi(ptype.m_size.c_str());
          adata::write(zbuf, size);
        }
      }
    }
    std::fwrite(zbuf.write_data() , zbuf.write_length() , 1 , fp);
    std::fclose(fp);
  }

  void pack_adt(const std::vector<std::string>& adt_list, const std::string& adp_file)
  {
    FILE * adp_fp = open_file(adp_file,"wb");

    const size_t buffer_size = 1024 * 1024;

    unsigned char  * buff = new unsigned char[buffer_size];
    std::unique_ptr<unsigned char> buffer_graud(buff);

    char adbuf[5];
    adata::zero_copy_buffer buf;
    buf.set_write(adbuf, sizeof(adbuf));
    adata::write(buf, (int32_t)adt_list.size());

    std::fwrite(buf.write_data(), 1, buf.write_length(), adp_fp);
    for (auto& adt_file : adt_list)
    {
      FILE * adt_fp = open_file(adt_file,"rb");
      size_t rd_len = 0;
      char sbuf[1];
      std::fread(sbuf, 1, 1, adt_fp);
      if (sbuf[0] != 1)
      {
        std::string err_str = adp_file + " isn't a single adt file!";
        throw std::exception(err_str.c_str());
      }
      do 
      {
        rd_len = std::fread(buff, 1, buffer_size ,adt_fp);
        if (rd_len > 0)
        {
          std::cout << "merge file buffer " << adt_file << std::endl;
          std::fwrite(buff, 1, rd_len , adp_fp);
        }
        else
        {
          std::cout << "read nothing " << adt_file  << " error no:" << errno << std::endl;
        }
      } while (rd_len == buffer_size);
      std::fclose(adt_fp);
    }
    std::fclose(adp_fp);
  }
}
