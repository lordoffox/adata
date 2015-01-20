#pragma once

#include "descrip.h"
#include <string>

bool parse_adl_file(
  descrip_define& define,
  std::vector<std::string> const& include_paths,
  const std::string& file,
  std::string& error_message
  );
