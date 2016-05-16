///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include <iostream>
#include "aarg.hpp"
#include "program.h"

int main(int ac, char* av[])
{
  try
  {
    options opt;
    aarg::argopt ao;

    ao.add('I', "input_file", "adl file", opt.input_file);
    ao.add('O', "output_path", "output source path", opt.output_path);
    ao.add('P', "include_paths", "include paths for other adl", opt.include_paths);
    ao.add('G', "gen", "gen source cpp,csharp,adt,cpp2lua", opt.gen);
    ao.add('H', "adata_header", "include adata.hpp path", opt.adata_header);
    ao.add('p', "pack_file", "pack adt files", opt.pack_files);
    ao.add('o', "pack_output_file", "output package path", opt.pack_output_file);

    ao.parse(ac, av);

    if (ac == 1)
    {
      std::cout << "Usage: options_description [options]\n";
      std::cout << ao.desc();
    }
    else
    {
      // default include path is curr dir
      opt.include_paths.push_back(".");
      run_program(opt);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << "\n";
    return 1;
  }
  return 0;
}
