#include <string>
#include <vector>

#pragma once

struct options
{
	std::string input_file;
	std::string output_path;
  std::vector<std::string> include_paths;
	std::vector<std::string> gen;
};

void run_program(const options& opt);
