#include <vector>
#include <fstream>
#include <ctime>

#include "adata.hpp"
#include "descrip.h"
#include "util.h"

using namespace std;

namespace js_gen
{

  const std::string js_lang("js");

  const char * js_being_define = R"((function (global) {
  var namespace =')";

  const char * js_type_define = R"(
  var defs = [
)";

  const char * js_end_define = R"foo(
  var types = {};

  for (var i = 0; i < defs.length; ++i)
  {
    var def = defs[i];
    types[def[0]] = def;
    for (var mi = 0 ; mi < def[1].length; ++mi)
    {
      var m = def[1][mi];
      var di = m[4];
      if (di > 0) { m[4] = defs[di - 1]; }
      var pdefs = m[5];
      for (var pi = 0; pi < pdefs.length; ++pi) {
        var p = pdefs[pi];
        var pdi = p[2];
        if (pdi > 0) { p[2] = defs[pdi - 1]; }
      }
    }
  }

  if (adata) {
    adata.ns[namespace] = types;
  }
  else {
    throw "need global.adata = require('adata')";
  }

})(this);)foo";

  std::string no_size = "0";

  ADATA_INLINE uint32_t to_js_layout_type(int type, int fix)
  {
    if (fix > 0)
    {
      if (type >= int8 && type <= uint64)
      {
        type = type - (int8 - fix_int8);
      }
    }
    return type;
  }

  ADATA_INLINE std::string gen_size(const std::string& size)
  {
    return (size.empty() ? "0" : size);
  }

  ADATA_INLINE void gen_type_ref_info(const member_define& m, std::ostream& os)
  {
    if (NULL != m.m_typedef)
    {
      os << m.m_typedef->m_index+1 ;
    }
    else
    {
      os << "0";
    }
  }

  void gen_code(const descrip_define& define, const std::string& lua_file)
  {
    ofstream os(lua_file);
    os << js_being_define;
    os << define.m_namespace.m_js_fullname << "';" << std::endl;
    os << js_type_define;
    for (auto& t_define : define.m_types)
    {
      os << tabs(2) << "[" << std::endl;
      os << tabs(3) << "'" << t_define.m_name << "'," << std::endl;
      os << tabs(3) << "[" << std::endl;
      for (auto& m_define : t_define.m_members)
      {
        os << tabs(4) << "[" << to_js_layout_type(m_define.m_type,m_define.m_fixed) << "," << m_define.m_deleted << ","
          << gen_size(m_define.m_size) << ",'" << m_define.m_name << "',";
        gen_type_ref_info(m_define, os);
        os << ",[";
        for (auto p_define : m_define.m_template_parameters)
        {
          os << " [" << to_js_layout_type(p_define.m_type, p_define.m_fixed) << "," << gen_size(p_define.m_size) << ",";
          gen_type_ref_info(p_define, os);
          os << "],";
        }
        os << "] ]," << std::endl;
      }
      os << tabs(3) << "]," << std::endl;
      os << tabs(2) << "]," << std::endl;
    }
    os << tabs(1) << "];" << std::endl;
    os << tabs(0) << js_end_define << std::endl;
    os.close();
  }

}
