#pragma once

#include <string>

#include "descrip.h"

bool load_from_adl(
  descrip_define& define, 
  std::vector<std::string> const& include_paths, 
  const std::string& adl_file, 
  std::string& error_message
  );
