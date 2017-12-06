///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include <string>
#include <iostream>

#include "string_view.h"

#include "program.h"
#include "parser.h"

options opt;
options& get_options()
{
  return opt;
}
namespace cpp_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace cpp2lua_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace csharp_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace kotlin_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace java_gen
{
  void gen_code(const descrip_define&, const std::string&);
}

namespace adt_gen
{
  void gen_code(const descrip_define&, const std::string&);
  void pack_adt(const std::vector<std::string>& adt_list, const std::string& adp_file);
}

void run_program()
{
  const options& opt = get_options();
  if (!opt.pack_files.empty())
  {
    adt_gen::pack_adt(opt.pack_files, opt.output_path + opt.pack_output_file);
    return;
  }

  auto ext_pos = opt.input_file.rfind('.');
  if (ext_pos == std::string::npos)
  {
    std::cerr << "error input file name , should be xxx.adl" << std::endl;
    return;
  }

  std::string_view input_file(opt.input_file);
  std::string_view input_file_ext = input_file.substr(ext_pos + 1);
  std::string_view output_file_name = input_file.substr(0, ext_pos);

  auto path_pos = ::std::string::npos;
  auto end_pos = path_pos;
  for (int i = (int)output_file_name.length() - 1; i >= 0; --i)
  {
    auto c = output_file_name[i];
    if (c == '\\' || c == '/')
    {
      path_pos = i;
      break;
    }
  }
  if (path_pos != end_pos)
  {
    output_file_name = output_file_name.substr(path_pos + 1);
  }
  std::string filename = output_file_name.to_string();

  std::string outputfile = opt.output_path;
  
  // Nous Xiong: add '/','\' and none of them compatibility
  if (outputfile.back() != '/' && outputfile.back() != '\\')
  {
    outputfile += '/';
  }
  std::string outputpath = outputfile;
  outputfile.append(filename);

  descrip_define define;
  define.m_filename = std::move(filename);
  define.adata_header = opt.adata_header;

  static std::string_view adl_str = "adl";

  if (input_file_ext == adl_str)
  {
    bool ret = parse_adl_file(define, opt.include_paths, opt.input_file);
    if (ret == false)
    {
      return;
    }
  }
  else
  {
    std::cerr << "error input file name , should be xxx.adl" << std::endl;
    return;
  }

  for (auto& gen_type : opt.gen)
  {
    if (gen_type == "cpp")
    {
      cpp_gen::gen_code(define, outputfile + ".adl.h");
    }
    if (gen_type == "cpp2lua")
    {
      cpp2lua_gen::gen_code(define, outputfile + ".adl");
    }
    else if (gen_type == "csharp")
    {
      csharp_gen::gen_code(define, outputfile + ".adl.cs");
    }
    else if (gen_type == "java")
    {
      java_gen::gen_code(define, outputpath + "java/");
    }
    else if (gen_type == "kt")
    {
      kotlin_gen::gen_code(define, outputfile + ".adl.kt");
    }
    else if (gen_type == "adt")
    {
      adt_gen::gen_code(define, outputfile + ".adt");
    }
  }
}