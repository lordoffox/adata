#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

#include "descrip.h"

int member_cmp(member_define* const a, member_define* const b);

void split_string(const std::string& value, std::vector<std::string>& list, char separator);

extern std::string tab_strings[10];

inline std::string& tabs(int indent)
{
	return tab_strings[indent];
}

extern const char * data_type_name[];

inline const char * type_name(int type)
{
	if (type >= e_base_type::fix_int8 && type <= e_base_type::string)
	{
		return data_type_name[type];
	}
	return data_type_name[0];
}

inline const char * type_name(int type, bool fix)
{
	if (type >=e_base_type::int8 && fix)
	{
		type -= (int8 - fix_int8);
	}
	if (type >= e_base_type::fix_int8 && type <= e_base_type::string)
	{
		return data_type_name[type];
	}
	return data_type_name[0];
}

#ifndef _MSC_VER
errno_t static inline fopen_s(FILE **f, const char *name, const char *mode) {
	errno_t ret = 0;
	assert(f);
  *f = fopen(name, mode);
	/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
	if (!*f)
		ret = errno;
	return ret;
}
#endif

