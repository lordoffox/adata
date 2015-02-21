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
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <ctime>

using namespace std;

struct int64_mask
{
	int64_mask(int bit_) :bit(bit_){}
	int bit;
};

std::ostream& operator << (std::ostream& os, const int64_mask& mask)
{
	if (mask.bit > 53)
	{
		os << "mask" << mask.bit;
	}
	else
	{
		os << (1LL << (mask.bit-1));
	}
	return os;
}

namespace lua_gen
{

  const std::string lua_lang("lua");

	typedef std::map<std::string, size_t> field_info_map;

	struct field_info
	{
		field_info_map	map;
		std::string			data;
		size_t field(const std::string& name)
		{
			size_t offset = 0;
			auto pos = map.find(name);
			if (pos == map.end())
			{
				offset = data.length();
				data.append(name);
				data.append(1,0);
				map.insert(std::make_pair(name, offset));
			}
			else
			{
				offset = pos->second;
			}
			return offset;
		}
	};

	typedef std::set<e_base_type> type_info_set_type;

	struct gen_contex
	{
		enum { use_field_direct , use_field_pool , use_field_list };
		int		use_field_type;
		bool	use_luajit;
		field_info f_info;
		gen_contex() :use_field_type(use_field_direct), use_luajit(false)
		{}
	};

  // Nous Xiong: change xxx.xx_t to ns.xxx_xx_t; if no '.' then do nothing
  inline std::string make_typename(std::string const& name)
  {
    std::size_t s = name.find_last_of('.');
    std::string tab;
    if (s != std::string::npos)
    {
      std::string ns = name.substr(0, s);
      boost::algorithm::replace_all(ns, ".", "_");
      tab = "ns.";
      return tab + ns + name.substr(s);
    }
    else
    {
      tab = "m.";
      return tab + name;
    }
  }

  // Nous Xiong: gen requires
  void gen_include(const descrip_define& desc_define, std::ostream& os)
  {
    for (auto const& inc : desc_define.m_includes)
    {
      auto s = inc.first.find_last_of('.');
      BOOST_ASSERT(s != std::string::npos);
      std::string require = inc.first.substr(s+1);
      require += "_adl";
      os << "require\'" << require << "\'" << std::endl;
    }
    os << std::endl;
  }

	inline void gen_meta(std::ostream& os, const type_define& t_define, int tab)
	{
		os << tabs(tab) << t_define.m_name << " = {adtype = nil,rd = nil,skip_rd = nil,wt = nil}," << endl;
	}

	std::string z_size = "0";

	inline std::string make_size(const member_define& m_define)
	{
		if (m_define.m_size.length())
		{
			return m_define.m_size;
		}
		return z_size;
	}

	inline std::ostream& make_trace_info(std::ostream& os, const std::string& name, const std::string& idx, gen_contex& ctx)
	{
		switch (ctx.use_field_type)
		{
		case gen_contex::use_field_direct:
		{
			os << "trace_error(buf, '" << name << "' , " << idx << ");";
			break;
		}
		case gen_contex::use_field_pool:
		{
			os << "trace_error(buf, field_info , " << ctx.f_info.field(name) << " , " << idx << ");";
			break;
		}
		case gen_contex::use_field_list:
		{
			os << "trace_error(buf, field_info[" << ctx.f_info.field(name) << "] , " << idx << ");";
			break;
		}
		}
		return os;
	}

	inline std::ostream& set_error(std::ostream& os, int ec)
	{
		os << "set_error(buf, " << ec << ");";
		return os;
	}

	void gen_filed_name_info(std::ostream& os, const descrip_define& define, gen_contex& ctx)
	{
		for (auto& t_define : define.m_types)
		{
			for (auto& m_define : t_define.m_members)
			{
				ctx.f_info.field(m_define.m_name);
			}
		}
		if (ctx.use_field_type == gen_contex::use_field_pool)
		{
			os << "local field_info = regiest_field_info('";
			std::size_t code_size = ctx.f_info.data.length();
			unsigned char * code_ptr = (unsigned char *)ctx.f_info.data.c_str();
			for (std::size_t i = 0; i < code_size; ++i)
			{
				uint32_t v = *code_ptr++;
				os << '\\';
				os << v;
			}
			os << "');" << std::endl;
		}
		else if (ctx.use_field_type == gen_contex::use_field_list)
		{
			size_t c = 1;
			for (auto& i : ctx.f_info.map)
			{
				i.second = c;
				++c;
			}
			os << "local field_info = {" << std::endl;;
			for (auto& i : ctx.f_info.map)
			{
				os << tabs(1) << "ffi.new(\"char[" << i.first.length() << "]\",\"" << i.first << "\")," << std::endl;
			}
			os << "}" << std::endl << std::endl;;
		}
	}

	void gen_filed_type_info(std::ostream& os, const descrip_define& define , gen_contex& ctx)
	{
		type_info_set_type set;
    set.insert(e_base_type::uint32);
    set.insert(e_base_type::int32);
		if (ctx.use_luajit)
		{
			set.insert(e_base_type::uint64);
		}
		else
		{
			set.insert(e_base_type::int64);
		}
		for (auto& t_define : define.m_types)
		{
			for (auto& m_define : t_define.m_members)
			{
				if (m_define.m_fixed)
				{
					set.insert(e_base_type(m_define.m_type - (e_base_type::int8 - e_base_type::fix_int8)));
				}
				else
				{
					set.insert(m_define.m_type);
				}
			}
		}
		for (auto& ti : set)
		{
			std::string tn = type_name(ti,false);
			if (tn.length())
				os << "local rd_" << tn << " = adata_m.rd_" << tn << std::endl;
		}
		for (auto& ti : set)
		{
			std::string tn = type_name(ti, false);
			if (tn.length())
				os << "local skip_rd_" << tn << " = adata_m.skip_rd_" << tn << std::endl;
		}
		for (auto& ti : set)
		{
			std::string tn = type_name(ti, false);
			if (tn.length())
				os << "local wt_" << tn << " = adata_m.wt_" << tn << std::endl;
		}
    // Nous Xiong: add szof
    for (auto& ti : set)
    {
      std::string tn = type_name(ti, false);
      if (tn.length())
        os << "local szof_" << tn << " = adata_m.szof_" << tn << std::endl;
    }
		os << std::endl;
	}

	inline void gen_meta_member_skip_read_value(std::ostream& os, const member_define& m_define, const std::string& var_name)
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		{
			os << "ec = skip_rd_" << type_name(m_define.m_type, m_define.m_fixed) << "(buf);";
			break;
		}
		case e_base_type::string:
		{
			os << "ec = skip_rd_str(buf," << make_size(m_define) << ");";
			break;
		}
		case e_base_type::type:
		{
      auto name = make_typename(m_define.m_typename);
      os << "local temp = " << name << "(); ";
			os << "ec = temp:skip_read(buf);";
			break;
		}
		default:
		{}
		}
	}

	inline void gen_meta_member_skip_read(std::ostream& os, const member_define& m_define, int tab, gen_contex& ctx)
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		case e_base_type::string:
		{
			os << tabs(tab);
			gen_meta_member_skip_read_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
			break;
		}
		case e_base_type::list:
		{
			os << tabs(tab) << "local ec,len = rd_u32(buf);" << std::endl;
			os << tabs(tab) << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "for i = 1 , len do ";
			gen_meta_member_skip_read_value(os, m_define.m_template_parameters[0], "v");
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " end;" << std::endl;
			break;
		}
		case e_base_type::map:
		{
			os << tabs(tab) << "local ec,len = rd_u32(buf);" << std::endl;
			os << tabs(tab) << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "for i = 1 , len do ";
			gen_meta_member_skip_read_value(os, m_define.m_template_parameters[0], "k");
			gen_meta_member_skip_read_value(os, m_define.m_template_parameters[1], "v");
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " end;" << std::endl;
			break;
		}
		case e_base_type::type:
		{
			os << tabs(tab);
			gen_meta_member_skip_read_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
		}
		default:
		{}
		}
	}

	inline void gen_meta_member_read_value(
    std::ostream& os, 
    const member_define& m_define, 
    const std::string& var_name,
    bool need_make = false
    )
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		{
			os << "ec," << var_name << " = rd_" << type_name(m_define.m_type, m_define.m_fixed) << "(buf);";
			break;
		}
		case e_base_type::string:
		{
			os << "ec," << var_name << " = rd_str(buf," << make_size(m_define) << ");";
			break;
		}
		case e_base_type::type:
		{
      if (need_make)
      {
        os << var_name << " = " << make_typename(m_define.m_typedef->m_name) << "(); ";
      }
			os << "ec = " << var_name << ":read(buf);";
			break;
		}
		default:
		{}
		}
	}

  // Nous Xiong: add len tag jump
  void gen_meta_len_tag_jump(std::ostream& os, int tab_indent)
  {
    os << tabs(tab_indent) << "if len_tag >= 0 then" << std::endl;
    os << tabs(tab_indent + 1) << "local read_len = get_rd_len(buf) - offset;" << std::endl;
    os << tabs(tab_indent + 1) << "if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;" << std::endl;
    os << tabs(tab_indent) << "end" << std::endl;
  }

	inline void gen_meta_member_read(std::ostream& os, const member_define& m_define, int tab, gen_contex& ctx)
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		case e_base_type::string:
		{
			os << tabs(tab);
			gen_meta_member_read_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
			break;
		}
		case e_base_type::list:
		{
			os << tabs(tab) << "local ec,len = rd_u32(buf);" << std::endl;
			os << tabs(tab) << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "local v;" << std::endl;
			os << tabs(tab) << "for i = 1 , len do ";
			gen_meta_member_read_value(os, m_define.m_template_parameters[0], "v", true);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " o." << m_define.m_name << "[i] = v";
			os << " end;" << std::endl;
			break;
		}
		case e_base_type::map:
		{
			os << tabs(tab) << "local ec,len = rd_u32(buf);" << std::endl;
			os << tabs(tab) << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "local k,v;" << std::endl;
			os << tabs(tab) << "for i = 1 , len do" << std::endl;
			os << tabs(tab + 1);
      gen_meta_member_read_value(os, m_define.m_template_parameters[0], "k", true);
      gen_meta_member_read_value(os, m_define.m_template_parameters[1], "v", true);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " o." << m_define.m_name << "[k] = v;" << std::endl;
			os << tabs(tab) << "end;" << std::endl;
			break;
		}
		case e_base_type::type:
		{
			os << tabs(tab);
			gen_meta_member_read_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
		}
		default:
		{}
		}
	}

	inline void gen_meta_member_write_value(std::ostream& os, const member_define& m_define, const std::string& var_name)
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		{
			os << "ec = wt_" << type_name(m_define.m_type, m_define.m_fixed) << "(buf," << var_name << ");";
			break;
		}
		case e_base_type::string:
		{
			os << "ec = wt_str(buf," << var_name << "," << make_size(m_define) << ");";
			break;
		}
		case e_base_type::type:
		{
			os << "ec = " << var_name << ":write(buf);";
			break;
		}
		default:
		{}
		}
	}

  // Nous Xiong: add gen_meta_member_size_of_value
  inline void gen_meta_member_size_of_value(std::ostream& os, const member_define& m_define, const std::string& var_name)
  {
    switch (m_define.m_type)
    {
    case e_base_type::uint8:
    case e_base_type::int8:
    case e_base_type::uint16:
    case e_base_type::int16:
    case e_base_type::uint32:
    case e_base_type::int32:
    case e_base_type::uint64:
    case e_base_type::int64:
    case e_base_type::float32:
    case e_base_type::float64:
    {
      os << "size = size + szof_" << type_name(m_define.m_type, m_define.m_fixed) << "(" << var_name << ");";
      break;
    }
    case e_base_type::string:
    {
      os << "size = size + szof_str(" << var_name << ");";
      break;
    }
    case e_base_type::type:
    {
      os << "size = size + " << var_name << ":size_of();";
      break;
    }
    default:
    {}
    }
  }

	inline void gen_meta_member_write(std::ostream& os, const member_define& m_define, int tab, gen_contex& ctx)
	{
		switch (m_define.m_type)
		{
		case e_base_type::uint8:
		case e_base_type::int8:
		case e_base_type::uint16:
		case e_base_type::int16:
		case e_base_type::uint32:
		case e_base_type::int32:
		case e_base_type::uint64:
		case e_base_type::int64:
		case e_base_type::float32:
		case e_base_type::float64:
		case e_base_type::string:
		{
			os << tabs(tab);
			gen_meta_member_write_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
			break;
		}
		case e_base_type::list:
		{
			os << tabs(tab) << "local len = #o." << m_define.m_name << ";" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "local ec  = wt_u32(buf,len);" << std::endl;
			os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;
			os << tabs(tab) << "local v;" << std::endl;
			os << tabs(tab) << "for i = 1,len do v = o." << m_define.m_name << "[i]; ";
			gen_meta_member_write_value(os, m_define.m_template_parameters[0], "v");
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " end;" << std::endl;
			break;
		}
		case e_base_type::map:
		{
			os << tabs(tab) << "local len = tablen(o." << m_define.m_name << ");" << std::endl;
			if (m_define.m_size.length())
			{
				os << tabs(tab) << "if len >" << m_define.m_size << " then ";
				set_error(os, adata::error_code_t::sequence_length_overflow);
				make_trace_info(os, m_define.m_name, "-1", ctx);
				os << " return ec; end;" << std::endl;
			}
			os << tabs(tab) << "local ec  = wt_u32(buf,len);" << std::endl;
			os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;
			os << tabs(tab) << "for k,v in pairs( o." << m_define.m_name << ") do ";
			gen_meta_member_write_value(os, m_define.m_template_parameters[0], "k");
			gen_meta_member_write_value(os, m_define.m_template_parameters[1], "v");
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "i", ctx);
			os << " return ec; end;";
			os << " end;" << std::endl;
			break;
		}
		case e_base_type::type:
		{
			os << tabs(tab);
			gen_meta_member_write_value(os, m_define, "o." + m_define.m_name);
			os << "if ec > 0 then ";
			make_trace_info(os, m_define.m_name, "-1", ctx);
			os << " return ec; end;";
			os << std::endl;
		}
		default:
		{}
		}
	}

  // Nous Xiong: add gen_meta_member_size_of
  inline void gen_meta_member_size_of(std::ostream& os, const member_define& m_define, int tab, gen_contex& ctx)
  {
    switch (m_define.m_type)
    {
    case e_base_type::uint8:
    case e_base_type::int8:
    case e_base_type::uint16:
    case e_base_type::int16:
    case e_base_type::uint32:
    case e_base_type::int32:
    case e_base_type::uint64:
    case e_base_type::int64:
    case e_base_type::float32:
    case e_base_type::float64:
    case e_base_type::string:
    {
      os << tabs(tab);
      gen_meta_member_size_of_value(os, m_define, "o." + m_define.m_name);
      os << std::endl;
      break;
    }
    case e_base_type::list:
    {
      os << tabs(tab) << "local len = #o." << m_define.m_name << ";" << std::endl;
      os << tabs(tab) << "size = size + szof_u32(len);" << std::endl;
      os << tabs(tab) << "local v;" << std::endl;
      os << tabs(tab) << "for i = 1,len do v = o." << m_define.m_name << "[i]; ";
      gen_meta_member_size_of_value(os, m_define.m_template_parameters[0], "v");
      os << " end;" << std::endl;
      break;
    }
    case e_base_type::map:
    {
      os << tabs(tab) << "local len = tablen(o." << m_define.m_name << ");" << std::endl;
      os << tabs(tab) << "size = size + szof_u32(len);" << std::endl;
      os << tabs(tab) << "for k,v in pairs( o." << m_define.m_name << ") do ";
      gen_meta_member_size_of_value(os, m_define.m_template_parameters[0], "k");
      gen_meta_member_size_of_value(os, m_define.m_template_parameters[1], "v");
      os << " end;" << std::endl;
      break;
    }
    case e_base_type::type:
    {
      os << tabs(tab);
      gen_meta_member_size_of_value(os, m_define, "o." + m_define.m_name);
      os << std::endl;
    }
    default:
    {}
    }
  }

	namespace lua_5_2
	{
		const char * lua_check_version = R"(require'check_lua_version'(5,)";

    const char * lua_require_define = R"(local adata_m = require'adata_core'
local ns = require'adata'
local regiest_field_info = adata_m.regiest_field_info;
local new_buf = adata_m.new_buf;
local del_buf = adata_m.del_buf;
local resize_buf = adata_m.resize_buf;
local clear_buf = adata_m.clear_buf;
local set_error = adata_m.set_error;
local trace_error = adata_m.trace_error;
local trace_info = adata_m.trace_info;
local get_write_data = adata_m.get_write_data;
local set_read_data = adata_m.set_read_data;
local rd_tag = adata_m.rd_tag;
local wt_tag = adata_m.wt_tag;
local get_rd_len = adata_m.get_rd_len;
local get_wt_len = adata_m.get_wt_len;
local skip_rd_len = adata_m.skip_rd_len;
local tablen = ns.tablen;

)";

		inline void gen_member_define(std::ostream& os, const member_define& m_define, int tab)
		{
			if (m_define.m_deleted)
			{
				return;
			}
			os << tabs(tab) << m_define.m_name << " = ";
			switch (m_define.m_type)
			{
			case e_base_type::uint8:
			case e_base_type::int8:
			case e_base_type::uint16:
			case e_base_type::int16:
			case e_base_type::uint32:
			case e_base_type::int32:
			case e_base_type::float32:
			case e_base_type::float64:
			{
				os << m_define.m_default_value;
				break;
			}
			case e_base_type::int64:
			{
				int64_t v = boost::lexical_cast<int64_t>(m_define.m_default_value);
				if (v > -(1LL << 52) && v < (1LL << 52))
				{
					os << m_define.m_default_value;
				}
				else
				{
					os << "int64.int64(" << m_define.m_default_value << ")";
				}
				break;

			}
			case e_base_type::uint64:
			{
				uint64_t v = boost::lexical_cast<uint64_t>(m_define.m_default_value);
				if (v < (1LL << 53))
				{
					os << m_define.m_default_value;
				}
				else
				{
					os << "int64.uint64(" << m_define.m_default_value << ")";
				}
				break;
			}
			case e_base_type::string:
			{
				os << "''";
				break;
			}
			case e_base_type::list:
			case e_base_type::map:
			{
				os << "{}";
				break;
			}
			case e_base_type::type:
			{
        // Nous Xiong: change typename gen
        auto name = make_typename(m_define.m_typedef->m_name);
        os << name << "()";
			}
			default:
			{}
			}
			os << "," << std::endl;
		}

    void gen_meta_read_tag(std::ostream& os, int tab)
    {
      // Nous Xiong: add len tag
      os << tabs(tab) << "local offset = get_rd_len(buf);" << std::endl;

      os << tabs(tab) << "local ec,read_tag = rd_tag(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab) << "local len_tag = 0;" << std::endl;
      os << tabs(tab) << "ec,len_tag = rd_i32(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;
    }

		void gen_meta_skip_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "skip_read = function(o,buf)" << std::endl;
			
      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (count == 63)
				{
					os << tabs(tab + 1) << "if read_tag >= " << read_mask << " then" << std::endl;
				}
				else
				{
					os << tabs(tab + 1) << "if (read_tag % " << int64_mask(count+1) << ") >= " << int64_mask(count) << " then" << std::endl;
				}
				os << tabs(tab + 2) << "read_tag = read_tag - " << int64_mask(count) << ";" << std::endl;
				gen_meta_member_skip_read(os, m_define, tab + 2, ctx);
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "read = function(o,buf)" << std::endl;
			
      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (count == 63)
				{
					os << tabs(tab + 1) << "if read_tag >= " << read_mask << " then" << std::endl;
				}
				else
				{
					os << tabs(tab + 1) << "if (read_tag % " << int64_mask(count + 1) << ") >= " << int64_mask(count) << " then" << std::endl;
				}
				os << tabs(tab + 2) << "read_tag = read_tag - " << int64_mask(count) << ";" << std::endl;
				if (!m_define.m_deleted)
				{
					gen_meta_member_read(os, m_define, tab + 2, ctx);
					os << tabs(tab + 2) << "if ec > 0 then return ec; end;" << std::endl;
					//os << tabs(tab + 1) << "else" << std::endl;
				}
        else
        {
				  gen_meta_member_skip_read(os, m_define, tab + 2, ctx);
        }
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_write(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "write = function(o,buf)" << std::endl;
			os << tabs(tab + 1) << "local write_tag = 0" << std::endl;
			int64_t write_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					if (m_define.is_multi())
					{
            if (m_define.m_type == e_base_type::map)
            {
						  os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then write_tag = write_tag + " << int64_mask(count) << "; end;" << std::endl;
            }
            else
            {
              os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then write_tag = write_tag + " << int64_mask(count) << "; end;" << std::endl;
            }
					}
					else
					{
						os << tabs(tab + 1) << "write_tag = write_tag + " << int64_mask(count) << ";" << std::endl;
					}
				}
				write_mask <<= 1;
				++count;
			}
			count = 1;
			os << tabs(tab + 1) << "ec = wt_tag(buf,write_tag);" << std::endl;
			os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "ec = wt_i32(buf,o:size_of());" << std::endl;
      os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

			write_mask = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					if (count == 63)
					{
						os << tabs(tab + 1) << "if write_tag >= " << write_mask << " then" << std::endl;
					}
					else
					{
						os << tabs(tab + 1) << "if (write_tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
					}
					gen_meta_member_write(os, m_define, tab + 2, ctx);
					os << tabs(tab + 1) << "end" << std::endl;
				}
				write_mask <<= 1;
				++count;
			}
			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

    // Nous Xiong: add gen_meta_size_of
    void gen_meta_size_of(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
    {
      os << tabs(tab) << "size_of = function(o)" << std::endl;
      os << tabs(tab + 1) << "local size = 0" << std::endl;
      os << tabs(tab + 1) << "local tag = 0" << std::endl;
      int64_t write_mask = 1;
      int count = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (m_define.is_multi())
          {
            if (m_define.m_type == e_base_type::map)
            {
              os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then tag = tag + " << int64_mask(count) << "; end;" << std::endl;
            }
            else
            {
              os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then tag = tag + " << int64_mask(count) << "; end;" << std::endl;
            }
          }
          else
          {
            os << tabs(tab + 1) << "tag = tag + " << int64_mask(count) << ";" << std::endl;
          }
        }
        write_mask <<= 1;
        ++count;
      }

      count = 1;
      write_mask = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (count == 63)
          {
            os << tabs(tab + 1) << "if tag >= " << write_mask << " then" << std::endl;
          }
          else
          {
            os << tabs(tab + 1) << "if (tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
          }
          gen_meta_member_size_of(os, m_define, tab + 2, ctx);
          os << tabs(tab + 1) << "end" << std::endl;
        }
        write_mask <<= 1;
        ++count;
      }
      os << tabs(tab + 1) << "size = size + szof_i64(tag);" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "size = size + szof_i32(size + szof_i32(size));" << std::endl;

      os << tabs(tab + 1) << "return size;" << std::endl;
      os << tabs(tab) << "end," << std::endl;
    }

		inline void gen_meta_imp(std::ostream& os, const type_define& t_define, int idx, gen_contex& ctx)
		{
			os << "mc = {" << std::endl;
			os << tabs(1) << "adtype = function() return m." << t_define.m_name << " end," << std::endl;
      gen_meta_skip_read(os, t_define, 1, ctx);
      gen_meta_size_of(os, t_define, 1, ctx);
			gen_meta_read(os, t_define, 1, ctx);
			gen_meta_write(os, t_define, 1, ctx);
			os << "};" << std::endl;
			os << "mc.__index = mc;" << std::endl;
			os << "mt[" << idx << "] = mc;" << std::endl;
		}

		void gen_proto(std::ostream& os, const type_define& t_define, int tab, int idx)
		{
			os << tabs(tab) << "m." << t_define.m_name << " = function()" << std::endl;
			os << tabs(tab + 1) << "local obj = {" << std::endl;
			for (auto& m_define : t_define.m_members)
			{
				gen_member_define(os, m_define, tab + 2);
			}
			os << tabs(tab + 1) << "};" << std::endl;
			os << tabs(tab + 1) << "setmetatable(obj,mt[" << idx << "]);" << std::endl;
			os << tabs(tab + 1) << "return obj;" << std::endl;
			os << tabs(tab) << "end" << std::endl << std::endl;
		}

		void gen_int64_const(std::ostream& os, const descrip_define& define)
		{
			size_t max_field = 0;
			for (auto& t_define : define.m_types)
			{
				max_field = std::max(max_field, t_define.m_members.size());
			}
			if (max_field > 52)
			{
				for (int i = 53; i < (int)max_field+1; ++i)
				{
					os << "local mask" << i << "= int64.int64(" << (1LL << i) << ");" << std::endl;
				}
			}
			os << std::endl;
		}

		void gen_code(const descrip_define& define, const std::string& lua_file , int min_ver)
		{
			ofstream os(lua_file);
			os << lua_check_version << min_ver << ");" << std::endl << std::endl;
			os << lua_require_define;
			gen_contex ctx;
			gen_filed_type_info(os, define, ctx);
			gen_filed_name_info(os, define, ctx);
			gen_int64_const(os, define);

      // Nous Xiong: add include gen
      gen_include(define, os);

			os << "local mt = {};" << std::endl << std::endl;
			os << "local m = {" << std::endl;
			for (auto& t_define : define.m_types)
			{
				os << tabs(1) << t_define.m_name << "," << std::endl;
			}
			os << "};" << std::endl << std::endl;
			int idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_proto(os, t_define, 0, idx);
				++idx;
			}
			os << "local mc = {};" << std::endl;
			idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_meta_imp(os, t_define, idx, ctx);
				++idx;
			}
      std::string ns = define.m_namespace.m_lua_fullname;
      ns.pop_back();

      // Nous Xiong: add ns table set
      os << "if ns." << ns << " == nil then" << std::endl;
      os << tabs(1) << "ns." << ns << " = m;" << std::endl;
      os << "else" << std::endl;
      for (auto& t_define : define.m_types)
      {
        os << tabs(1) << "ns." << ns << "." << t_define.m_name
          << " = m." << t_define.m_name << std::endl;
      }
      os << "end" << std::endl;

      //os << "ns." << ns << " = m;" << std::endl;
			os << "return m;" << std::endl;
			os.close();
		}

	}

	namespace lua_5_3
	{
		const char * lua_check_version = R"(require'check_lua_version'(5,3)

)";

		const char * lua_require_define = R"(local adata_m = require'adata_core'
local ns = require'adata'
local regiest_field_info = adata_m.regiest_field_info;
local new_buf = adata_m.new_buf;
local del_buf = adata_m.del_buf;
local resize_buf = adata_m.resize_buf;
local clear_buf = adata_m.clear_buf;
local set_error = adata_m.set_error;
local trace_error = adata_m.trace_error;
local trace_info = adata_m.trace_info;
local get_write_data = adata_m.get_write_data;
local set_read_data = adata_m.set_read_data;
local rd_tag = adata_m.rd_tag;
local wt_tag = adata_m.wt_tag;
local get_rd_len = adata_m.get_rd_len;
local get_wt_len = adata_m.get_wt_len;
local skip_rd_len = adata_m.skip_rd_len;
local tablen = ns.tablen;

)";

		inline void gen_member_define(std::ostream& os, const member_define& m_define, int tab)
		{
			if ( m_define.m_deleted )
			{
				return;
			}
			os << tabs(tab) << m_define.m_name << " = ";
			switch (m_define.m_type)
			{
			case e_base_type::uint8:
			case e_base_type::int8:
			case e_base_type::uint16:
			case e_base_type::int16:
			case e_base_type::uint32:
			case e_base_type::int32:
			case e_base_type::int64:
			case e_base_type::float32:
			case e_base_type::float64:
			{
				os << m_define.m_default_value;
				break;
			}
			case e_base_type::uint64:
			{
				uint64_t v = boost::lexical_cast<uint64_t>(m_define.m_default_value);
				if (v < (1ULL << 63) )
				{
					os << m_define.m_default_value;
				}
				else
				{
					os << "int64.uint64(" << m_define.m_default_value << ")";
				}
				break;
			}
			case e_base_type::string:
			{
				os << "''";
				break;
			}
			case e_base_type::list:
			case e_base_type::map:
			{
				os << "{}";
				break;
			}
			case e_base_type::type:
			{
        // Nous Xiong: change typename gen
        auto name = make_typename(m_define.m_typedef->m_name);
        os << name << "()";
			}
			default:
			{}
			}
			os << "," << std::endl;
		}

    void gen_meta_read_tag(std::ostream& os, int tab)
    {
      // Nous Xiong: add len tag
      os << tabs(tab) << "local offset = get_rd_len(buf);" << std::endl;

      os << tabs(tab) << "local ec,read_tag = rd_tag(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab) << "local len_tag = 0;" << std::endl;
      os << tabs(tab) << "ec,len_tag = rd_i32(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;
    }

		void gen_meta_skip_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "skip_read = function(o,buf)" << std::endl;
      
      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (count == 63)
				{
					os << tabs(tab + 1) << "if read_tag >= " << read_mask << " then" << std::endl;
				}
				else
				{
					os << tabs(tab + 1) << "if (read_tag % " << (read_mask << 1) << ") >= " << read_mask << " then" << std::endl;
				}
				os << tabs(tab + 2) << "read_tag = read_tag - " << read_mask << ";" << std::endl;
				gen_meta_member_skip_read(os, m_define, tab + 2,ctx);
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab+1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "read = function(o,buf)" << std::endl;
			
      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (count == 63)
				{
					os << tabs(tab + 1) << "if read_tag >= " << read_mask << " then" << std::endl;
				}
				else
				{
					os << tabs(tab + 1) << "if (read_tag % " << (read_mask << 1) << ") >= " << read_mask << " then" << std::endl;
				}
				os << tabs(tab + 2) << "read_tag = read_tag - " << read_mask << ";" << std::endl;
				if (!m_define.m_deleted)
				{
					gen_meta_member_read(os, m_define, tab + 2,ctx);
					os << tabs(tab + 2) << "if ec > 0 then return ec; end;" << std::endl;
					//os << tabs(tab + 1) << "else" << std::endl;
				}
        else
        {
				  gen_meta_member_skip_read(os, m_define, tab + 2,ctx);
        }
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible  << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab+1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_write(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "write = function(o,buf)" << std::endl;
			os << tabs(tab + 1) << "local write_tag = 0" << std::endl;
			int64_t write_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					if (m_define.is_multi())
					{
            if (m_define.m_type == e_base_type::map)
            {
						  os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then write_tag = write_tag + " << write_mask << "; end;" << std::endl;
            }
            else
            {
              os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then write_tag = write_tag + " << write_mask << "; end;" << std::endl;
            }
					}
					else
					{
						os << tabs(tab+1) << "write_tag = write_tag + " << write_mask << ";" << std::endl;
					}
				}
				write_mask <<= 1;
			}
			os << tabs(tab + 1) << "ec = wt_tag(buf,write_tag);" << std::endl;
			os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "ec = wt_i32(buf,o:size_of());" << std::endl;
      os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

			write_mask = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					if (count == 63)
					{
						os << tabs(tab + 1) << "if write_tag >= " << write_mask << " then" << std::endl;
					}
					else
					{
						os << tabs(tab + 1) << "if (write_tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
					}
					gen_meta_member_write(os, m_define, tab + 2,ctx);
					os << tabs(tab + 1) << "end" << std::endl;
				}
				write_mask <<= 1;
				++count;
			}
			os << tabs(tab+1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

    // Nous Xiong: add gen_meta_size_of
    void gen_meta_size_of(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
    {
      os << tabs(tab) << "size_of = function(o)" << std::endl;
      os << tabs(tab + 1) << "local size = 0" << std::endl;
      os << tabs(tab + 1) << "local tag = 0" << std::endl;
      int64_t write_mask = 1;
      int count = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (m_define.is_multi())
          {
            if (m_define.m_type == e_base_type::map)
            {
              os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then tag = tag + " << write_mask << "; end;" << std::endl;
            }
            else
            {
              os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then tag = tag + " << write_mask << "; end;" << std::endl;
            }
          }
          else
          {
            os << tabs(tab + 1) << "tag = tag + " << write_mask << ";" << std::endl;
          }
        }
        write_mask <<= 1;
      }

      write_mask = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (count == 63)
          {
            os << tabs(tab + 1) << "if tag >= " << write_mask << " then" << std::endl;
          }
          else
          {
            os << tabs(tab + 1) << "if (tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
          }
          gen_meta_member_size_of(os, m_define, tab + 2, ctx);
          os << tabs(tab + 1) << "end" << std::endl;
        }
        write_mask <<= 1;
        ++count;
      }
      os << tabs(tab + 1) << "size = size + szof_i64(tag);" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "size = size + szof_i32(size + szof_i32(size));" << std::endl;

      os << tabs(tab + 1) << "return size;" << std::endl;
      os << tabs(tab) << "end," << std::endl;
    }

		inline void gen_meta_imp(std::ostream& os, const type_define& t_define, int idx, gen_contex& ctx)
		{
			os << "mc = {" << std::endl;
			os << tabs(1) << "adtype = function() return m." << t_define.m_name << " end," << std::endl;
      gen_meta_skip_read(os, t_define, 1, ctx);
      gen_meta_size_of(os, t_define, 1, ctx);
			gen_meta_read(os, t_define, 1, ctx);
			gen_meta_write(os, t_define, 1, ctx);
			os << "};" << std::endl;
			os << "mc.__index = mc;" << std::endl;
			os << "mt[" << idx << "] = mc;" << std::endl;
		}

		void gen_proto(std::ostream& os, const type_define& t_define, int tab, int idx)
		{
			os << tabs(tab) << "m." << t_define.m_name << " = function()" << std::endl;
			os << tabs(tab + 1) << "local obj = {" << std::endl;
			for (auto& m_define : t_define.m_members)
			{
				gen_member_define(os, m_define, tab + 2);
			}
			os << tabs(tab + 1) << "};" << std::endl;
			os << tabs(tab + 1) << "setmetatable(obj,mt[" << idx << "]);" << std::endl;
			os << tabs(tab + 1) << "return obj;" << std::endl;
			os << tabs(tab) << "end" << std::endl << std::endl;
		}

		void gen_code(const descrip_define& define, const std::string& lua_file)
		{
			ofstream os(lua_file);
			os << lua_check_version;
			os << lua_require_define;
			gen_contex ctx;
			gen_filed_type_info(os, define, ctx);
			gen_filed_name_info(os, define, ctx);

      // Nous Xiong: add include gen
      gen_include(define, os);

			os << "local mt = {};" << std::endl << std::endl;
			os << "local m = {" << std::endl;
			for (auto& t_define : define.m_types)
			{
				os << tabs(1) << t_define.m_name << "," << std::endl;
			}
			os << "};" << std::endl << std::endl;
			int idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_proto(os, t_define, 0, idx);
				++idx;
			}
			os << "local mc = {};" << std::endl;
			idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_meta_imp(os, t_define, idx, ctx);
				++idx;
			}
      std::string ns = define.m_namespace.m_lua_fullname;
      ns.pop_back();

      // Nous Xiong: add ns table set
      os << "if ns." << ns << " == nil then" << std::endl;
      os << tabs(1) << "ns." << ns << " = m;" << std::endl;
      os << "else" << std::endl;
      for (auto& t_define : define.m_types)
      {
        os << tabs(1) << "ns." << ns << "." << t_define.m_name
          << " = m." << t_define.m_name << std::endl;
      }
      os << "end" << std::endl;

      //os << "ns." << ns << " = m;" << std::endl;
			os << "return m;" << std::endl;
			os.close();
		}
	}

	namespace lua_jit
	{
    const char * lua_require_define = R"(local adata_m = require'adata_core'
local ffi = require'ffi'
local ns = require'adata'
local new_buf = adata_m.new_buf;
local resize_buf = adata_m.resize_buf;
local clear_buf = adata_m.clear_buf;
local set_error = adata_m.set_error;
local trace_error = adata_m.trace_error;
local trace_info = adata_m.trace_info;
local get_write_data = adata_m.get_write_data;
local set_read_data = adata_m.set_read_data;
local get_rd_len = adata_m.get_rd_len;
local get_wt_len = adata_m.get_wt_len;
local skip_rd_len = adata_m.skip_rd_len;
local tablen = ns.tablen;

)";

		inline void gen_member_define(std::ostream& os, const member_define& m_define, int tab)
		{
			if (m_define.m_deleted)
			{
				return;
			}
			os << tabs(tab) << m_define.m_name << " = ";
			switch (m_define.m_type)
			{
			case e_base_type::uint8:
			case e_base_type::int8:
			case e_base_type::uint16:
			case e_base_type::int16:
			case e_base_type::uint32:
			case e_base_type::int32:
			case e_base_type::float32:
			case e_base_type::float64:
			{
				os << m_define.m_default_value;
				break;
			}
			case e_base_type::int64:
			{
				os << m_define.m_default_value << "ll";
				break;
			}
			case e_base_type::uint64:
			{
				os << m_define.m_default_value << "ull";
				break;
			}
			case e_base_type::string:
			{
				os << "''";
				break;
			}
			case e_base_type::list:
			case e_base_type::map:
			{
				os << "{}";
				break;
			}
			case e_base_type::type:
			{
        // Nous Xiong: change typename gen
        auto name = make_typename(m_define.m_typedef->m_name);
        os << name << "()";
			}
			default:
			{}
			}
			os << "," << std::endl;
		}

    void gen_meta_read_tag(std::ostream& os, int tab)
    {
      // Nous Xiong: add len tag
      os << tabs(tab) << "local offset = get_rd_len(buf);" << std::endl;

      os << tabs(tab) << "local ec,read_tag = rd_u64(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab) << "local len_tag = 0;" << std::endl;
      os << tabs(tab) << "ec,len_tag = rd_i32(buf);" << std::endl;
      os << tabs(tab) << "if ec > 0 then return ec; end;" << std::endl;
    }

		void gen_meta_skip_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "skip_read = function(o,buf)" << std::endl;

      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				os << tabs(tab + 1) << "if (read_tag % " << (read_mask << 1) << ") >= " << read_mask << " then" << std::endl;
				os << tabs(tab + 2) << "read_tag = read_tag - " << read_mask << ";" << std::endl;
				gen_meta_member_skip_read(os, m_define, tab + 2, ctx);
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_read(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "read = function(o,buf)" << std::endl;

      // Nous Xiong: add len tag
      gen_meta_read_tag(os, tab + 1);

			int64_t read_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				os << tabs(tab + 1) << "if (read_tag % " << (read_mask << 1) << ") >= " << read_mask << " then" << std::endl;
				os << tabs(tab + 2) << "read_tag = read_tag - " << read_mask << ";" << std::endl;
				if (!m_define.m_deleted)
				{
					gen_meta_member_read(os, m_define, tab + 2, ctx);
					os << tabs(tab + 2) << "if ec > 0 then return ec; end;" << std::endl;
					//os << tabs(tab + 1) << "else" << std::endl;
				}
        else
        {
				  gen_meta_member_skip_read(os, m_define, tab + 2, ctx);
        }
				os << tabs(tab + 1) << "end" << std::endl;
				read_mask <<= 1;
				++count;
			}

      // Nous Xiong: remove max mask check, for backward compat
			/*os << tabs(tab + 1) << "if read_tag > 0 then ";
			set_error(os, adata::error_code_t::undefined_member_protocol_not_compatible);
			os << "return " << adata::error_code_t::undefined_member_protocol_not_compatible << "; end; " << std::endl;*/

      gen_meta_len_tag_jump(os, tab + 1);

			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

		void gen_meta_write(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
		{
			os << tabs(tab) << "write = function(o,buf)" << std::endl;
			os << tabs(tab + 1) << "local write_tag = 0" << std::endl;
			int64_t write_mask = 1;
			int count = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					if (m_define.is_multi())
					{
            if (m_define.m_type == e_base_type::map)
            {
              os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then write_tag = write_tag + " << write_mask << "; end;" << std::endl;
            }
            else
            {
						  os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then write_tag = write_tag + " << write_mask << "; end;" << std::endl;
            }
					}
					else
					{
						os << tabs(tab + 1) << "write_tag = write_tag + " << write_mask << ";" << std::endl;
					}
				}
				write_mask <<= 1;
			}
			os << tabs(tab + 1) << "ec = wt_u64(buf,write_tag);" << std::endl;
			os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "ec = wt_i32(buf,o:size_of());" << std::endl;
      os << tabs(tab + 1) << "if ec >0 then return ec; end;" << std::endl;

			write_mask = 1;
			for (auto& m_define : t_define.m_members)
			{
				if (!m_define.m_deleted)
				{
					os << tabs(tab + 1) << "if (write_tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
					gen_meta_member_write(os, m_define, tab + 2, ctx);
					os << tabs(tab + 1) << "end" << std::endl;
				}
				write_mask <<= 1;
				++count;
			}
			os << tabs(tab + 1) << "return ec;" << std::endl;
			os << tabs(tab) << "end," << std::endl;
		}

    // Nous Xiong: add gen_meta_size_of
    void gen_meta_size_of(std::ostream& os, const type_define& t_define, int tab, gen_contex& ctx)
    {
      os << tabs(tab) << "size_of = function(o)" << std::endl;
      os << tabs(tab + 1) << "local size = 0" << std::endl;
      os << tabs(tab + 1) << "local tag = 0" << std::endl;
      int64_t write_mask = 1;
      int count = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (m_define.is_multi())
          {
            if (m_define.m_type == e_base_type::map)
            {
              os << tabs(tab + 1) << "if tablen(o." << m_define.m_name << ") > 0 then tag = tag + " << write_mask << "; end;" << std::endl;
            }
            else
            {
              os << tabs(tab + 1) << "if #o." << m_define.m_name << " > 0 then tag = tag + " << write_mask << "; end;" << std::endl;
            }
          }
          else
          {
            os << tabs(tab + 1) << "tag = tag + " << write_mask << ";" << std::endl;
          }
        }
        write_mask <<= 1;
      }

      write_mask = 1;
      for (auto& m_define : t_define.m_members)
      {
        if (!m_define.m_deleted)
        {
          if (count == 63)
          {
            os << tabs(tab + 1) << "if tag >= " << write_mask << " then" << std::endl;
          }
          else
          {
            os << tabs(tab + 1) << "if (tag % " << (write_mask << 1) << ") >= " << write_mask << " then" << std::endl;
          }
          gen_meta_member_size_of(os, m_define, tab + 2, ctx);
          os << tabs(tab + 1) << "end" << std::endl;
        }
        write_mask <<= 1;
        ++count;
      }
      os << tabs(tab + 1) << "size = size + szof_u64(tag);" << std::endl;

      // Nous Xiong: add len tag
      os << tabs(tab + 1) << "size = size + szof_i32(size + szof_i32(size));" << std::endl;

      os << tabs(tab + 1) << "return size;" << std::endl;
      os << tabs(tab) << "end," << std::endl;
    }

		inline void gen_meta_imp(std::ostream& os, const type_define& t_define, int idx, gen_contex& ctx)
		{
			os << "mc = {" << std::endl;
			os << tabs(1) << "adtype = function() return m." << t_define.m_name << " end," << std::endl;
      gen_meta_skip_read(os, t_define, 1, ctx);
      gen_meta_size_of(os, t_define, 1, ctx);
			gen_meta_read(os, t_define, 1, ctx);
			gen_meta_write(os, t_define, 1, ctx);
			os << "};" << std::endl;
			os << "mc.__index = mc;" << std::endl;
			os << "mt[" << idx << "] = mc;" << std::endl;
		}

		void gen_proto(std::ostream& os, const type_define& t_define, int tab, int idx)
		{
			os << tabs(tab) << "m." << t_define.m_name << " = function()" << std::endl;
			os << tabs(tab + 1) << "local obj = {" << std::endl;
			for (auto& m_define : t_define.m_members)
			{
				gen_member_define(os, m_define, tab + 2);
			}
			os << tabs(tab + 1) << "};" << std::endl;
			os << tabs(tab + 1) << "setmetatable(obj,mt[" << idx << "]);" << std::endl;
			os << tabs(tab + 1) << "return obj;" << std::endl;
			os << tabs(tab) << "end" << std::endl << std::endl;
		}

		void gen_code(const descrip_define& define, const std::string& lua_file)
		{
			ofstream os(lua_file);
			os << lua_require_define;
			gen_contex ctx;
			ctx.use_luajit = true;
			ctx.use_field_type = gen_contex::use_field_list;
			gen_filed_type_info(os, define,ctx);
			gen_filed_name_info(os, define, ctx);

      // Nous Xiong: add include gen
      gen_include(define, os);

			os << "local mt = {};" << std::endl << std::endl;
			os << "local m = {" << std::endl;
			for (auto& t_define : define.m_types)
			{
				os << tabs(1) << t_define.m_name << "," << std::endl;
			}
			os << "};" << std::endl << std::endl;
			int idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_proto(os, t_define, 0, idx);
				++idx;
			}
			os << "local mc = {};" << std::endl;
			idx = 1;
			for (auto& t_define : define.m_types)
			{
				gen_meta_imp(os, t_define, idx, ctx);
				++idx;
			}
			std::string ns = define.m_namespace.m_lua_fullname;
      ns.pop_back();

      // Nous Xiong: add ns table set
      os << "if ns." << ns << " == nil then" << std::endl;
      os << tabs(1) << "ns." << ns << " = m;" << std::endl;
      os << "else" << std::endl;
      for (auto& t_define : define.m_types)
      {
        os << tabs(1) << "ns." << ns << "." << t_define.m_name 
          << " = m." << t_define.m_name << std::endl;
      }
      os << "end" << std::endl;

      //os << "ns." << ns << " = m;" << std::endl;
			os << "return m;" << std::endl;
			os.close();
		}
	}
}
