///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#ifndef ADL_PROGRAM_H
#define ADL_PROGRAM_H

#include <string>
#include <vector>

struct options
{
	std::string input_file;
	std::string output_path;
  std::vector<std::string> include_paths;
	std::vector<std::string> gen;
};

void run_program(const options& opt);

#endif /// ADL_PROGRAM_H
