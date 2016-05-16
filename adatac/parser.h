///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#pragma once

#include "descrip.h"
#include <string>

bool parse_adl_file(
  descrip_define& define,
  std::vector<std::string> const& include_paths,
  const std::string& file
  );

