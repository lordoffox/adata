// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef l_adata_corec_hpp
#define l_adata_corec_hpp

#include <cstdint>
#include "adata.hpp"
#include "adata_int64.hpp"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace adata { namespace lua{

	const char * zbuffer_metatable = "zbuf_meta";

	static int regiest_field_info(lua_State * L)
	{
		size_t len;
		const char * str = lua_tolstring(L, 1, &len);
		if (len <= 0)
		{
			return luaL_error(L, "error info size %d", len);
		}
		void * info = lua_newuserdata(L, len);
		memcpy(info, str, len);
		return 1;
	}

	static int new_zbuf(lua_State * L)
	{
		lua_Integer size = lua_tointeger(L, 1);
		if (size < 0)
		{
      return luaL_error(L, "error buffer size %d", size);
		}
		if (size == 0)
		{
			size = 65535;
		}

    // Nous Xiong: change to malloc, for resize realloc
    uint8_t* buffer = (uint8_t*)std::malloc(size);
    if (buffer == 0)
    {
      return luaL_error(L, "buffer alloc failed");
    }

		void * obj = lua_newuserdata(L, sizeof(adata::zero_copy_buffer));
		adata::zero_copy_buffer * zbuf = new (obj) adata::zero_copy_buffer;
		zbuf->set_write(buffer, size);
#if LUA_VERSION_NUM == 501
		luaL_newmetatable(L, zbuffer_metatable);
		lua_setmetatable(L, -2);
#else
		luaL_setmetatable(L, zbuffer_metatable);
#endif
		return 1;
	}

	ADATA_INLINE void _reset_buf(adata::zero_copy_buffer& zbuf)
	{
		if (zbuf.write_data() != 0)
		{
      // Nous Xiong: change to free, for resize realloc
			std::free((void*)zbuf.write_data());
			zbuf.set_write((uint8_t *)NULL, 0);
		}
	}

	static int del_zbuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		_reset_buf(*zbuf);
		return 1;
	}

	static int resize_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		lua_Integer size = lua_tointeger(L, 2);
		if (size <= 0)
		{
      return luaL_error(L, "error buffer size %d", size);
		}
		
    // Nous Xiong: change to realloc to improvment
    void* p = (void*)zbuf->write_data();
    uint8_t* buffer = (uint8_t*)std::realloc(p, size);
    if (buffer == 0)
    {
      return luaL_error(L, "buffer alloc failed");
    }

		zbuf->set_write(buffer, size);
		return 1;
	}

	static int clear_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
    zbuf->clear();
		return 1;
	}

	static int set_error_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
		lua_Integer ec = lua_tointeger(L, 2);
		zbuf->set_error_code((adata::error_code_t)ec);
		return 1;
	}

	static int trace_error_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
		const char * info = (const char *)lua_touserdata(L, 2);
		lua_Integer offset = lua_tointeger(L, 3);
		lua_Integer idx = lua_tointeger(L, 4);
		if (idx == 0)
		{
			idx = -1;
		}
		zbuf->trace_error(info + offset, (int)idx);
		return 1;
	}

	static int trace_info_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
		std::string info;
		zbuf->get_trace_error_info(info);
		lua_pushlstring(L, info.data(), info.length());
		return 1;
	}

  static int get_read_length(lua_State * L)
  {
    adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
    if (NULL == zbuf)
    {
      return luaL_error(L, "arg 1 must be zbuf!");
    }
    lua_pushinteger(L, zbuf->read_length());
    return 1;
  }

  static int get_write_length(lua_State * L)
  {
    adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
    if (NULL == zbuf)
    {
      return luaL_error(L, "arg 1 must be zbuf!");
    }
    lua_pushinteger(L, zbuf->write_length());
    return 1;
  }

	static int get_write_buf_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
		lua_pushlstring(L, zbuf->write_data(), zbuf->write_length());
		lua_Integer dont_clear = lua_tointeger(L, 2);
		if (!dont_clear)
		{
			zbuf->clear();
		}
		return 1;
	}

	static int set_read_buf_zuf(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, 1);
		if (NULL == zbuf)
		{
      return luaL_error(L, "arg 1 must be zbuf!");
		}
		size_t len;
		const char * str = lua_tolstring(L, 2, &len);
		size_t nlen = lua_tointeger(L, 3);
		if (nlen > 0)
		{
			len = nlen;
		}
		zbuf->set_read(str, len);
		return 1;
	}

	ADATA_INLINE adata::zero_copy_buffer * _get_zbuf_arg(lua_State * L, int idx)
	{
		adata::zero_copy_buffer * zbuf = (adata::zero_copy_buffer *)lua_touserdata(L, idx);
		if (NULL == zbuf)
		{
      luaL_error(L, "arg 1 must be zbuf!");
		}
		return zbuf;
	}

	template <typename T>
	ADATA_INLINE int read_fix_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		T v;
		adata::fix_read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushinteger(L, v);
		return 2;
	}

	template <typename T>
	ADATA_INLINE int read_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		T v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushinteger(L, v);
		return 2;
	}

	static int read_fix_int8(lua_State * L)
	{
		return read_fix_value<int8_t>(L);
	}

	static int read_fix_uint8(lua_State * L)
	{
		return read_fix_value<uint8_t>(L);
	}

	static int read_fix_int16(lua_State * L)
	{
		return read_fix_value<int16_t>(L);
	}

	static int read_fix_uint16(lua_State * L)
	{
		return read_fix_value<uint16_t>(L);
	}

	static int read_fix_int32(lua_State * L)
	{
		return read_fix_value<int32_t>(L);
	}

	static int read_fix_uint32(lua_State * L)
	{
		return read_fix_value<uint32_t>(L);
	}

#if LUA_VERSION_NUM < 503
	static int read_fix_int64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v;
		adata::fix_read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushint64(L, v);
		return 2;
	}
#else
	static int read_fix_int64(lua_State * L)
	{
		return read_fix_value<int64_t>(L);
	}
#endif

	static int read_fix_uint64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint64_t v;
		adata::fix_read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushuint64(L, v);
		return 2;
	}

	static int read_int8(lua_State * L)
	{
		return read_value<int8_t>(L);
	}

	static int read_uint8(lua_State * L)
	{
		return read_value<uint8_t>(L);
	}

	static int read_int16(lua_State * L)
	{
		return read_value<int16_t>(L);
	}

	static int read_uint16(lua_State * L)
	{
		return read_value<uint16_t>(L);
	}

	static int read_int32(lua_State * L)
	{
		return read_value<int32_t>(L);
	}

	static int read_uint32(lua_State * L)
	{
		return read_value<uint32_t>(L);
	}

#if LUA_VERSION_NUM < 503
	static int read_int64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushint64(L, v);
		return 2;
	}
#else
	static int read_int64(lua_State * L)
	{
		return read_value<int64_t>(L);
	}
#endif

	static int read_uint64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint64_t v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushuint64(L, v);
		return 2;
	}

	static int read_tag(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint64_t v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushint64(L, (int64_t)v);
		return 2;
	}

	static int read_float32(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		float v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushnumber(L, v);
		return 2;
	}

	static int read_float64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		double v;
		adata::read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		lua_pushnumber(L, v);
		return 2;
	}

	static int read_str(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint32_t slen = 0;
		adata::read(*zbuf, slen);
		lua_Integer len = lua_tointeger(L, 2);
		if (len > 0 && len < (lua_Integer)slen)
		{
			zbuf->set_error_code(sequence_length_overflow);
			slen = 0;
		}
		const char * str = (const char *)zbuf->skip_read(slen);
		if (zbuf->error())
		{
			slen = 0;
		}
		lua_pushinteger(L, zbuf->error_code());
		lua_pushlstring(L, str, slen);
		return 2;
	}

	template <typename T>
	ADATA_INLINE int skip_read_fix_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		zbuf->skip_read(sizeof(T));
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	template <typename T>
	ADATA_INLINE int skip_read_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		T* v = 0;
		adata::skip_read(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int skip_read_fix_int8(lua_State * L)
	{
		return skip_read_fix_value<int8_t>(L);
	}

	static int skip_read_fix_uint8(lua_State * L)
	{
		return skip_read_fix_value<uint8_t>(L);
	}

	static int skip_read_fix_int16(lua_State * L)
	{
		return skip_read_fix_value<int16_t>(L);
	}

	static int skip_read_fix_uint16(lua_State * L)
	{
		return skip_read_fix_value<uint16_t>(L);
	}

	static int skip_read_fix_int32(lua_State * L)
	{
		return skip_read_fix_value<int32_t>(L);
	}

	static int skip_read_fix_uint32(lua_State * L)
	{
		return skip_read_fix_value<uint32_t>(L);
	}

	static int skip_read_fix_int64(lua_State * L)
	{
		return skip_read_fix_value<int64_t>(L);
	}

	static int skip_read_fix_uint64(lua_State * L)
	{
		return skip_read_fix_value<uint64_t>(L);
	}

	static int skip_read_int8(lua_State * L)
	{
		return skip_read_value<int8_t>(L);
	}

	static int skip_read_uint8(lua_State * L)
	{
		return skip_read_value<uint8_t>(L);
	}

	static int skip_read_int16(lua_State * L)
	{
		return skip_read_value<int16_t>(L);
	}

	static int skip_read_uint16(lua_State * L)
	{
		return skip_read_value<uint16_t>(L);
	}

	static int skip_read_int32(lua_State * L)
	{
		return skip_read_value<int32_t>(L);
	}

	static int skip_read_uint32(lua_State * L)
	{
		return skip_read_value<uint32_t>(L);
	}

	static int skip_read_int64(lua_State * L)
	{
		return skip_read_value<int64_t>(L);
	}

	static int skip_read_uint64(lua_State * L)
	{
		return skip_read_value<uint64_t>(L);
	}

	static int skip_read_float32(lua_State * L)
	{
		return skip_read_value<float>(L);
	}

	static int skip_read_float64(lua_State * L)
	{
		return skip_read_value<double>(L);
	}

	static int skip_read_str(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint32_t slen = 0;
		adata::read(*zbuf, slen);
		lua_Integer len = lua_tointeger(L, 2);
		if (len > 0 && len < (lua_Integer)slen)
		{
			zbuf->set_error_code(sequence_length_overflow);
			slen = 0;
		}
		zbuf->skip_read(slen);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	template<typename T>
	ADATA_INLINE void get_lua_number64(lua_State * L, int idx , T& v)
	{
		number64_type it;
		lua_tonumber64(L, idx, &it);
		switch (it.type)
		{
		case et_int64_lua_number:
		{
			v = (T)it.value.d64;
			break;
		}
		case et_int64_int64:
		{
			v = (T)it.value.i64;
			break;
		}
		case et_int64_uint64:
		{
			v = (T)it.value.u64;
			break;
		}
		default: v = 0;
		}
	}

	template <typename T>
	ADATA_INLINE int write_fix_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		T v = (T)lua_tointeger(L, 2);
		adata::fix_write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	template <typename T>
	ADATA_INLINE int write_value(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		T v = (T)lua_tointeger(L, 2);
		adata::write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int write_fix_int8(lua_State * L)
	{
		return write_fix_value<int8_t>(L);
	}

	static int write_fix_uint8(lua_State * L)
	{
		return write_fix_value<uint8_t>(L);
	}

	static int write_fix_int16(lua_State * L)
	{
		return write_fix_value<int16_t>(L);
	}

	static int write_fix_uint16(lua_State * L)
	{
		return write_fix_value<uint16_t>(L);
	}

	static int write_fix_int32(lua_State * L)
	{
		return write_fix_value<int32_t>(L);
	}

	static int write_fix_uint32(lua_State * L)
	{
		return write_fix_value<uint32_t>(L);
	}

#if LUA_VERSION_NUM < 503
	static int write_fix_int64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v;
		get_lua_number64(L, 2, v);
		adata::fix_write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}
#else
	static int write_fix_int64(lua_State * L)
	{
		return write_fix_value<int64_t>(L);
	}
#endif

	static int write_fix_uint64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint64_t v;
		get_lua_number64(L, 2, v);
		adata::fix_write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int write_int8(lua_State * L)
	{
		return write_value<int8_t>(L);
	}

	static int write_uint8(lua_State * L)
	{
		return write_value<uint8_t>(L);
	}

	static int write_int16(lua_State * L)
	{
		return write_value<int16_t>(L);
	}

	static int write_uint16(lua_State * L)
	{
		return write_value<uint16_t>(L);
	}

	static int write_int32(lua_State * L)
	{
		return write_value<int32_t>(L);
	}

	static int write_uint32(lua_State * L)
	{
		return write_value<uint32_t>(L);
	}

#if LUA_VERSION_NUM < 503
	static int write_int64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v;
		get_lua_number64(L, 2, v);
		adata::write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}
#else
	static int write_int64(lua_State * L)
	{
		return write_value<int64_t>(L);
	}
#endif

#if LUA_VERSION_NUM < 503
	static int write_tag(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v;
		get_lua_number64(L, 2, v);
		adata::write(*zbuf, (uint64_t)v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}
#else
	static int write_tag(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		int64_t v = lua_tointeger(L,2);
		adata::write(*zbuf, (uint64_t)v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}
#endif

	static int write_uint64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		uint64_t v;
		get_lua_number64(L, 2, v);
		adata::write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int write_float32(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		float v = (float)lua_tonumber(L, 2);
		adata::write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int write_float64(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		double v = lua_tonumber(L, 2);
		adata::write(*zbuf, v);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	static int write_str(lua_State * L)
	{
		adata::zero_copy_buffer * zbuf = _get_zbuf_arg(L, 1);
		size_t slen = 0;
		const char * str = lua_tolstring(L, 2, &slen);
		lua_Integer len = lua_tointeger(L, 3);
		if (len > 0 && len < (lua_Integer)slen)
		{
			zbuf->set_error_code(sequence_length_overflow);
			slen = 0;
		}
		adata::write(*zbuf, (uint32_t)slen);
		zbuf->write(str, slen);
		lua_pushinteger(L, zbuf->error_code());
		return 1;
	}

	template <typename T>
	ADATA_INLINE int size_of_fix_value(lua_State * L)
	{
    lua_pushinteger(L, adata::fix_size_of(T()));
		return 1;
	}

	template <typename T>
	ADATA_INLINE int size_of_value(lua_State * L)
	{
		T v = (T)lua_tointeger(L, 1);
		int32_t s = adata::size_of(v);
    lua_pushinteger(L, s);
		return 1;
	}

	static int size_of_fix_int8(lua_State * L)
	{
		return size_of_fix_value<int8_t>(L);
	}

	static int size_of_fix_uint8(lua_State * L)
	{
		return size_of_fix_value<uint8_t>(L);
	}

	static int size_of_fix_int16(lua_State * L)
	{
		return size_of_fix_value<int16_t>(L);
	}

	static int size_of_fix_uint16(lua_State * L)
	{
		return size_of_fix_value<uint16_t>(L);
	}

	static int size_of_fix_int32(lua_State * L)
	{
		return size_of_fix_value<int32_t>(L);
	}

	static int size_of_fix_uint32(lua_State * L)
	{
		return size_of_fix_value<uint32_t>(L);
	}

	static int size_of_fix_int64(lua_State * L)
	{
		return size_of_fix_value<int64_t>(L);
	}

	static int size_of_fix_uint64(lua_State * L)
	{
    return size_of_fix_value<uint64_t>(L);
	}

	static int size_of_int8(lua_State * L)
	{
		return size_of_value<int8_t>(L);
	}

	static int size_of_uint8(lua_State * L)
	{
		return size_of_value<uint8_t>(L);
	}

	static int size_of_int16(lua_State * L)
	{
		return size_of_value<int16_t>(L);
	}

	static int size_of_uint16(lua_State * L)
	{
		return size_of_value<uint16_t>(L);
	}

	static int size_of_int32(lua_State * L)
	{
		return size_of_value<int32_t>(L);
	}

	static int size_of_uint32(lua_State * L)
	{
		return size_of_value<uint32_t>(L);
	}

#if LUA_VERSION_NUM < 503
	static int size_of_int64(lua_State * L)
	{
		int64_t v;
		get_lua_number64(L, 1, v);
		int32_t s = adata::size_of(v);
    lua_pushinteger(L, s);
		return 1;
	}
#else
	static int size_of_int64(lua_State * L)
	{
		return size_of_value<int64_t>(L);
	}
#endif

	static int size_of_uint64(lua_State * L)
	{
		uint64_t v;
		get_lua_number64(L, 1, v);
		int32_t s = adata::size_of(v);
    lua_pushinteger(L, s);
		return 1;
	}

	static int size_of_float32(lua_State * L)
	{
    int32_t s = adata::size_of(float());
    lua_pushinteger(L, s);
		return 1;
	}

	static int size_of_float64(lua_State * L)
	{
    int32_t s = adata::size_of(double());
    lua_pushinteger(L, s);
		return 1;
	}

	static int size_of_str(lua_State * L)
	{
    size_t len;
    char const* str = lua_tolstring(L, 1, &len);
    int32_t s = adata::size_of((int32_t)len);
    s += (int32_t)len;
    lua_pushinteger(L, s);
		return 1;
	}

#if LUA_VERSION_NUM == 501
	ADATA_INLINE int init_adata_corec(lua_State *L)
	{
		init_lua_int64(L);

		static const luaL_Reg buf_meta_table[] =
		{
			{ "__gc", del_zbuf },
			{ NULL, NULL }
		};

		luaL_newmetatable(L, zbuffer_metatable);
		luaL_setfuncs(L, buf_meta_table, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		static const luaL_Reg lib[] =
		{
			{ "regiest_field_info", regiest_field_info },
			{ "new_buf", new_zbuf },
			{ "del_buf", del_zbuf },
			{ "resize_buf", resize_zuf },
			{ "clear_buf", clear_zuf },
			{ "set_error", set_error_zuf },
			{ "trace_error", trace_error_zuf },
      { "trace_info", trace_info_zuf },
      { "get_rd_len", get_read_length },
      { "get_wt_len", get_write_length },
			{ "get_write_data", get_write_buf_zuf },
			{ "set_read_data", set_read_buf_zuf },
			{ "rd_tag", read_tag },
			{ "wt_tag", write_tag },
			{ "rd_fixi8", read_fix_int8 },
			{ "rd_fixu8", read_fix_uint8 },
			{ "rd_fixi16", read_fix_int16 },
			{ "rd_fixu16", read_fix_uint16 },
			{ "rd_fixi32", read_fix_int32 },
			{ "rd_fixu32", read_fix_uint32 },
			{ "rd_fixi64", read_fix_int64 },
			{ "rd_fixu64", read_fix_uint64 },
			{ "rd_i8", read_int8 },
			{ "rd_u8", read_uint8 },
			{ "rd_i16", read_int16 },
			{ "rd_u16", read_uint16 },
			{ "rd_i32", read_int32 },
			{ "rd_u32", read_uint32 },
			{ "rd_i64", read_int64 },
			{ "rd_u64", read_uint64 },
			{ "rd_f32", read_float32 },
			{ "rd_f64", read_float64 },
			{ "rd_str", read_str },
			{ "skip_rd_fixi8", skip_read_fix_int8 },
			{ "skip_rd_fixu8", skip_read_fix_uint8 },
			{ "skip_rd_fixi16", skip_read_fix_int16 },
			{ "skip_rd_fixu16", skip_read_fix_uint16 },
			{ "skip_rd_fixi32", skip_read_fix_int32 },
			{ "skip_rd_fixu32", skip_read_fix_uint32 },
			{ "skip_rd_fixi64", skip_read_fix_int64 },
			{ "skip_rd_fixu64", skip_read_fix_uint64 },
			{ "skip_rd_i8", skip_read_int8 },
			{ "skip_rd_u8", skip_read_uint8 },
			{ "skip_rd_i16", skip_read_int16 },
			{ "skip_rd_u16", skip_read_uint16 },
			{ "skip_rd_i32", skip_read_int32 },
			{ "skip_rd_u32", skip_read_uint32 },
			{ "skip_rd_i64", skip_read_int64 },
			{ "skip_rd_u64", skip_read_uint64 },
			{ "skip_rd_f32", skip_read_float32 },
			{ "skip_rd_f64", skip_read_float64 },
			{ "skip_rd_str", skip_read_str },
			{ "wt_fixi8", write_fix_int8 },
			{ "wt_fixu8", write_fix_uint8 },
			{ "wt_fixi16", write_fix_int16 },
			{ "wt_fixu16", write_fix_uint16 },
			{ "wt_fixi32", write_fix_int32 },
			{ "wt_fixu32", write_fix_uint32 },
			{ "wt_fixi64", write_fix_int64 },
			{ "wt_fixu64", write_fix_uint64 },
			{ "wt_i8", write_int8 },
			{ "wt_u8", write_uint8 },
			{ "wt_i16", write_int16 },
			{ "wt_u16", write_uint16 },
			{ "wt_i32", write_int32 },
			{ "wt_u32", write_uint32 },
			{ "wt_i64", write_int64 },
			{ "wt_u64", write_uint64 },
			{ "wt_f32", write_float32 },
			{ "wt_f64", write_float64 },
			{ "wt_str", write_str },
			{ "szof_fixi8", size_of_fix_int8 },
			{ "szof_fixu8", size_of_fix_uint8 },
			{ "szof_fixi16", size_of_fix_int16 },
			{ "szof_fixu16", size_of_fix_uint16 },
			{ "szof_fixi32", size_of_fix_int32 },
			{ "szof_fixu32", size_of_fix_uint32 },
			{ "szof_fixi64", size_of_fix_int64 },
			{ "szof_fixu64", size_of_fix_uint64 },
			{ "szof_i8", size_of_int8 },
			{ "szof_u8", size_of_uint8 },
			{ "szof_i16", size_of_int16 },
			{ "szof_u16", size_of_uint16 },
			{ "szof_i32", size_of_int32 },
			{ "szof_u32", size_of_uint32 },
			{ "szof_i64", size_of_int64 },
			{ "szof_u64", size_of_uint64 },
			{ "szof_f32", size_of_float32 },
			{ "szof_f64", size_of_float64 },
			{ "szof_str", size_of_str },

			{ NULL, NULL }
		};

		luaL_register(L, "adata_core", lib);
		return 1;
	}

#else

	ADATA_INLINE int regist_adata_core(lua_State *L)
	{
		luaL_checkversion(L);
		static const luaL_Reg buf_meta_table[] =
		{
			{ "__gc", del_zbuf },
			{ NULL, NULL }
		};

		luaL_newmetatable(L, zbuffer_metatable);
		luaL_setfuncs(L, buf_meta_table, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");

		static const luaL_Reg lib[] =
		{
			{ "regiest_field_info", regiest_field_info },
			{ "new_buf"				, new_zbuf },
			{ "del_buf"				, del_zbuf },
			{ "resize_buf"		, resize_zuf },
			{ "clear_buf"			, clear_zuf },
			{ "set_error"			, set_error_zuf },
			{ "trace_error"		,	trace_error_zuf} ,
      { "trace_info", trace_info_zuf },
      { "get_rd_len", get_read_length },
      { "get_wt_len", get_write_length },
			{ "get_write_data", get_write_buf_zuf },
			{ "set_read_data"	, set_read_buf_zuf },
			{ "rd_tag"				, read_tag },
			{ "wt_tag"				, write_tag },
			{ "rd_fixi8"			, read_fix_int8 },
			{ "rd_fixu8"			, read_fix_uint8 },
			{ "rd_fixi16"			, read_fix_int16 },
			{ "rd_fixu16"			, read_fix_uint16 },
			{ "rd_fixi32"			, read_fix_int32 },
			{ "rd_fixu32"			, read_fix_uint32 },
			{ "rd_fixi64"			, read_fix_int64 },
			{ "rd_fixu64"			, read_fix_uint64 },
			{ "rd_i8"					, read_int8 },
			{ "rd_u8"					, read_uint8 },
			{ "rd_i16"				, read_int16 },
			{ "rd_u16"				, read_uint16 },
			{ "rd_i32"				, read_int32 },
			{ "rd_u32"				, read_uint32 },
			{ "rd_i64"				, read_int64 },
			{ "rd_u64"				, read_uint64 },
			{ "rd_f32"				, read_float32 },
			{ "rd_f64"				, read_float64 },
			{ "rd_str"				, read_str },
			{ "skip_rd_fixi8"	, skip_read_fix_int8 },
			{ "skip_rd_fixu8"	, skip_read_fix_uint8 },
			{ "skip_rd_fixi16", skip_read_fix_int16 },
			{ "skip_rd_fixu16", skip_read_fix_uint16 },
			{ "skip_rd_fixi32", skip_read_fix_int32 },
			{ "skip_rd_fixu32", skip_read_fix_uint32 },
			{ "skip_rd_fixi64", skip_read_fix_int64 },
			{ "skip_rd_fixu64", skip_read_fix_uint64 },
			{ "skip_rd_i8"		, skip_read_int8 },
			{ "skip_rd_u8"		, skip_read_uint8 },
			{ "skip_rd_i16"		, skip_read_int16 },
			{ "skip_rd_u16"		, skip_read_uint16 },
			{ "skip_rd_i32"		, skip_read_int32 },
			{ "skip_rd_u32"		, skip_read_uint32 },
			{ "skip_rd_i64"		, skip_read_int64 },
			{ "skip_rd_u64"		, skip_read_uint64 },
			{ "skip_rd_f32"		, skip_read_float32 },
			{ "skip_rd_f64"		, skip_read_float64 },
			{ "skip_rd_str"		, skip_read_str },
			{ "wt_fixi8"			, write_fix_int8 },
			{ "wt_fixu8"			, write_fix_uint8 },
			{ "wt_fixi16"			, write_fix_int16 },
			{ "wt_fixu16"			, write_fix_uint16 },
			{ "wt_fixi32"			, write_fix_int32 },
			{ "wt_fixu32"			, write_fix_uint32 },
			{ "wt_fixi64"			, write_fix_int64 },
			{ "wt_fixu64"			, write_fix_uint64 },
			{ "wt_i8"					, write_int8 },
			{ "wt_u8"					, write_uint8 },
			{ "wt_i16"				, write_int16 },
			{ "wt_u16"				, write_uint16 },
			{ "wt_i32"				, write_int32 },
			{ "wt_u32"				, write_uint32 },
			{ "wt_i64"				, write_int64 },
			{ "wt_u64"				, write_uint64 },
			{ "wt_f32"				, write_float32 },
			{ "wt_f64"				, write_float64 },
			{ "wt_str"				, write_str },
			{ "szof_fixi8"	, size_of_fix_int8 },
			{ "szof_fixu8"	, size_of_fix_uint8 },
			{ "szof_fixi16"	, size_of_fix_int16 },
			{ "szof_fixu16"	, size_of_fix_uint16 },
			{ "szof_fixi32"	, size_of_fix_int32 },
			{ "szof_fixu32"	, size_of_fix_uint32 },
			{ "szof_fixi64"	, size_of_fix_int64 },
			{ "szof_fixu64"	, size_of_fix_uint64 },
			{ "szof_i8"			, size_of_int8 },
			{ "szof_u8"			, size_of_uint8 },
			{ "szof_i16"		, size_of_int16 },
			{ "szof_u16"		, size_of_uint16 },
			{ "szof_i32"		, size_of_int32 },
			{ "szof_u32"		, size_of_uint32 },
			{ "szof_i64"		, size_of_int64 },
			{ "szof_u64"		, size_of_uint64 },
			{ "szof_f32"		, size_of_float32 },
			{ "szof_f64"		, size_of_float64 },
			{ "szof_str"		, size_of_str },

			{ NULL, NULL }
		};
		luaL_newlib(L, lib);
		return 1;
	}

	ADATA_INLINE int init_adata_corec(lua_State *L)
	{
		init_lua_int64(L);
		luaL_requiref(L, "adata_core", regist_adata_core, 1);
		return 1;
	}

#endif

}}


#endif
