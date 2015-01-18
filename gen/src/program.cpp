#include <string>
#include <iostream>

#include <boost/utility/string_ref.hpp>

#include "program.h"
#include "parser.h"

namespace cpp_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace csharp_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace lua_gen
{
	namespace lua_jit
	{
		void gen_code(const descrip_define&, const std::string&);
	}
	namespace lua_5_2
	{
		void gen_code(const descrip_define&, const std::string& , int min_ver = 2);
	}
	namespace lua_5_3
	{
		void gen_code(const descrip_define&, const std::string&);
	}
}

namespace js_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

void run_program(const options& opt)
{
	auto ext_pos = opt.input_file.rfind('.');
	if(ext_pos == std::string::npos)
	{
		std::cerr << "error input file name , should be xxx.adl" << std::endl;
		return;
	}

	boost::string_ref input_file(opt.input_file);
	boost::string_ref input_file_ext = input_file.substr(ext_pos + 1);
	boost::string_ref output_file_name = input_file.substr(0,ext_pos);
	
  auto path_pos = ::std::string::npos;
  auto end_pos = path_pos;
	for (int i = (int)output_file_name.length() - 1; i >= 0; --i)
	{
		auto c = output_file_name[i];
		if(c == '\\' || c == '/')
		{
			path_pos = i;
			break;
		}
	}
  if (path_pos != end_pos)
	{
		output_file_name = output_file_name.substr(path_pos + 1);
	}

	std::string outputfile = opt.output_path;
	outputfile.append(output_file_name.to_string());

	descrip_define define;

	if (input_file_ext == "adl")
	{
		std::string error;
		bool ret = load_from_adl(define, opt.include_paths, opt.input_file, error);
		if( ret == false )
		{
			return;
		}
	}
	else
	{
		std::cerr << "error input file name , should be xxx.adl" << std::endl;
		return;
	}

	for( auto& gen_type : opt.gen)
	{
		if( gen_type == "cpp")
		{
      cpp_gen::gen_code(define, outputfile + ".adl.h");
		}
    else if (gen_type == "csharp")
    {
      csharp_gen::gen_code(define, outputfile + ".adl.cs");
    }
    else if (gen_type == "lua")
    {
			lua_gen::lua_5_3::gen_code(define, outputfile + "_adl.lua");
    }
		else if (gen_type == "lua51")
		{
			lua_gen::lua_5_2::gen_code(define, outputfile + "_adl.lua" , 1);
		}
		else if (gen_type == "lua52")
		{
			lua_gen::lua_5_2::gen_code(define, outputfile + "_adl.lua");
		}
		else if (gen_type == "lua53")
		{
			lua_gen::lua_5_3::gen_code(define, outputfile + "_adl.lua");
		}
		else if (gen_type == "luajit")
		{
			lua_gen::lua_jit::gen_code(define, outputfile + "_adl.lua");
		}
		else if (gen_type == "js")
    {
      js_gen::gen_code(define, outputfile + "_adl.js");
    }
  }
}