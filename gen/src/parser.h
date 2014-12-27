#pragma once

#include <string>

#include "descrip.h"

bool load_from_adl(descrip_define& define, const std::string& adl_file, std::string& error_message);
