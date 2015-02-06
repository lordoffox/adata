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
#include <boost/program_options.hpp>

#include "program.h"

using namespace boost;
namespace po = boost::program_options;

int main(int ac, char* av[])
{
	try
	{
		options opt;

		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("input_file,I", po::value<std::string>(&opt.input_file), "idl file(json,adl)")
			("output_path,O", po::value< std::string >(&opt.output_path)->default_value("./"), "output source path")
      ("include_paths,P", po::value<std::vector<std::string> >(&opt.include_paths), "include paths for other adl")
			("gen,G", po::value<std::vector<std::string> >(&opt.gen), "gen source cpp,csharp,lua(lua51,lua52,lua53,luajit),js")
			;

		po::variables_map vm;
		po::store(po::command_line_parser(ac, av).options(desc).run(), vm);
		po::notify(vm);

		if (vm.count("help") || ac == 1)
		{
			std::cout << "Usage: options_description [options]\n";
			std::cout << desc;
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
