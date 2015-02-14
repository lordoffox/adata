// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef l_adata_jit_corec_hpp
#define l_adata_jit_corec_hpp

#include <cstdint>
#include "adata.hpp"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"
}

#define ADATA_API static

extern "C" {

	ADATA_API adata::zero_copy_buffer * adata_new_buf(size_t size)
	{
		if (size == 0)
		{
			size = 65535;
		}
	  unsigned char * buffer = new unsigned char[size];
		adata::zero_copy_buffer * zbuf = new adata::zero_copy_buffer;
		zbuf->set_write(buffer, size);
		return zbuf;
	}

	ADATA_INLINE void _adata_reset_write_buf(adata::zero_copy_buffer& zbuf)
	{
		if (zbuf.write_data() != 0)
		{
			delete[] zbuf.write_data();
			zbuf.set_write((uint8_t *)NULL, 0);
		}
	}

	ADATA_API void adata_del_buf(adata::zero_copy_buffer* zbuf)
	{
		_adata_reset_write_buf(*zbuf);
		delete zbuf;
		return;
	}

	ADATA_API int adata_resize_buf(adata::zero_copy_buffer* zbuf, size_t size)
	{
		if (size <= 0)
		{
			return -1;
		}
		_adata_reset_write_buf(*zbuf);
		unsigned char * buffer = new unsigned char[size];
		zbuf->set_write(buffer, size);
		return 1;
	}

	ADATA_API int adata_clear_buf(adata::zero_copy_buffer* zbuf)
	{
		zbuf->clear();
		return 1;
	}

	ADATA_API int adata_set_error(adata::zero_copy_buffer* zbuf, int ec)
	{
		zbuf->set_error_code((adata::error_code_t)ec);
		return 1;
	}

	ADATA_API int adata_error(adata::zero_copy_buffer* zbuf)
	{
		return zbuf->m_error_code;
	}

	ADATA_API int adata_trace_error(adata::zero_copy_buffer* zbuf, char const* info, int idx)
	{
		zbuf->trace_error(info, idx);
		return 1;
	}

	ADATA_API void * adata_get_read_ptr(adata::zero_copy_buffer * buf)
	{
		return (void*)buf->read_ptr();
	}

	ADATA_API void * adata_skip_read(adata::zero_copy_buffer * buf , size_t len)
	{
		return (void*)buf->skip_read(len);
	}

	ADATA_API void * adata_get_write_ptr(adata::zero_copy_buffer * buf)
	{
		return (void*)buf->write_ptr();
	}

	ADATA_API void * adata_append_write(adata::zero_copy_buffer * buf, size_t len)
	{
		return (void*)buf->append_write(len);
	}

	ADATA_API char * adata_get_trace_info(adata::zero_copy_buffer * buf)
	{
		std::string info;
		buf->get_trace_error_info(info);
		char * data = (char *)malloc(info.length()+1);
		memcpy(data, info.data(), info.length());
		data[info.length()] = 0;
		return data;
	}

	ADATA_API void * adata_get_write_data(adata::zero_copy_buffer * buf)
	{
		return (void*)buf->write_data();
	}

  ADATA_API size_t adata_get_read_length(adata::zero_copy_buffer * buf)
  {
    return buf->read_length();
  }

	ADATA_API size_t adata_get_write_length(adata::zero_copy_buffer * buf)
	{
		return buf->write_length();
	}

	ADATA_API int adata_set_read_buf(adata::zero_copy_buffer* zbuf, char const* data, size_t len)
	{
		zbuf->set_read(data, len);
		return 1;
	}

	ADATA_API int8_t adata_read_fix_int8(adata::zero_copy_buffer * buf)
	{
		int8_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API uint8_t adata_read_fix_uint8(adata::zero_copy_buffer * buf)
	{
		uint8_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API int16_t adata_read_fix_int16(adata::zero_copy_buffer * buf)
	{
		int16_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API uint16_t adata_read_fix_uint16(adata::zero_copy_buffer * buf)
	{
		uint16_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API int32_t adata_read_fix_int32(adata::zero_copy_buffer * buf)
	{
		int32_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API uint32_t adata_read_fix_uint32(adata::zero_copy_buffer * buf)
	{
		uint32_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API int64_t adata_read_fix_int64(adata::zero_copy_buffer * buf)
	{
		int64_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API uint64_t adata_read_fix_uint64(adata::zero_copy_buffer * buf)
	{
		uint64_t v;
		adata::fix_stream_read(*buf, v);
		return v;
	}

	ADATA_API int8_t adata_read_int8(adata::zero_copy_buffer * buf)
	{
		int8_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API uint8_t adata_read_uint8(adata::zero_copy_buffer * buf)
	{
		uint8_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API int16_t adata_read_int16(adata::zero_copy_buffer * buf)
	{
		int16_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API uint16_t adata_read_uint16(adata::zero_copy_buffer * buf)
	{
		uint16_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API int32_t adata_read_int32(adata::zero_copy_buffer * buf)
	{
		int32_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API uint32_t adata_read_uint32(adata::zero_copy_buffer * buf)
	{
		uint32_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API int64_t adata_read_int64(adata::zero_copy_buffer * buf)
	{
		int64_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API uint64_t adata_read_uint64(adata::zero_copy_buffer * buf)
	{
		uint64_t v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API float adata_read_float32(adata::zero_copy_buffer * buf)
	{
		float v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API double adata_read_float64(adata::zero_copy_buffer * buf)
	{
		double v;
		adata::stream_read(*buf, v);
		return v;
	}

	ADATA_API int adata_skip_read_fix_int8(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(1);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_uint8(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(1);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_int16(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(2);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_uint16(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(2);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_int32(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(4);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_uint32(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(4);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_int64(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(8);
		return 0;
	}

	ADATA_API int adata_skip_read_fix_uint64(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(8);
		return 0;
	}

	ADATA_API int adata_skip_read_int8(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (int8_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_uint8(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (uint8_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_int16(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (int16_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_uint16(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (uint16_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_int32(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (int32_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_uint32(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (uint32_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_int64(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (int64_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_uint64(adata::zero_copy_buffer * buf)
	{
		adata::skip_read(*buf, (uint64_t*)0);
		return 0;
	}

	ADATA_API int adata_skip_read_float32(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(4);
		return 0;
	}

	ADATA_API int adata_skip_read_float64(adata::zero_copy_buffer * buf)
	{
		buf->skip_read(8);
		return 0;
	}

	ADATA_API int adata_skip_read_str(adata::zero_copy_buffer * buf, size_t len)
	{
		uint32_t slen = 0;
		adata::stream_read(*buf, slen);
		if (len > 0 && len < slen)
		{
			buf->set_error_code(adata::sequence_length_overflow);
			slen = 0;
		}
		buf->skip_read(slen);
		return 0;
	}

	ADATA_API int adata_write_fix_int8(adata::zero_copy_buffer * buf, int8_t v)
	{
		adata::fix_stream_write(*buf, v);
		return 0;
	}

	ADATA_API int adata_write_fix_uint8(adata::zero_copy_buffer * buf, uint8_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_int16(adata::zero_copy_buffer * buf, int16_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_uint16(adata::zero_copy_buffer * buf, uint16_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_int32(adata::zero_copy_buffer * buf, int32_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_uint32(adata::zero_copy_buffer * buf, uint32_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_int64(adata::zero_copy_buffer * buf, int64_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_fix_uint64(adata::zero_copy_buffer * buf, uint64_t v)
	{
		adata::fix_stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_int8(adata::zero_copy_buffer * buf, int8_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_uint8(adata::zero_copy_buffer * buf, uint8_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_int16(adata::zero_copy_buffer * buf, int16_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_uint16(adata::zero_copy_buffer * buf, uint16_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_int32(adata::zero_copy_buffer * buf, int32_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_uint32(adata::zero_copy_buffer * buf, uint32_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_int64(adata::zero_copy_buffer * buf, int64_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_uint64(adata::zero_copy_buffer * buf, uint64_t v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}


	ADATA_API int adata_write_float32(adata::zero_copy_buffer * buf, float v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_write_float64(adata::zero_copy_buffer * buf, double v)
	{
		adata::stream_write(*buf, v);
		return buf->m_error_code;
	}

	ADATA_API int adata_size_of_fix_int8(int8_t v)
	{
		return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_uint8(uint8_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_int16(int16_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_uint16(uint16_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_int32(int32_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_uint32(uint32_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_int64(int64_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_fix_uint64(uint64_t v)
	{
    return (int)adata::fix_size_of(v);
	}

	ADATA_API int adata_size_of_int8(int8_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_uint8(uint8_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_int16(int16_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_uint16(uint16_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_int32(int32_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_uint32(uint32_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_int64(int64_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_uint64(uint64_t v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_float32(float v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_float64(double v)
	{
    return (int)adata::size_of(v);
	}

	ADATA_API int adata_size_of_str(size_t len)
	{
    int32_t s = adata::size_of((int32_t)len);
    s += (int32_t)len;
		return (int)s;
	}

	ADATA_API int adata_get_functions(lua_State * L)
	{
		intptr_t function_addres[] = {
			(intptr_t)adata_new_buf,
			(intptr_t)adata_del_buf,
			(intptr_t)adata_resize_buf,
			(intptr_t)adata_clear_buf,
			(intptr_t)adata_set_error,
			(intptr_t)adata_error,
			(intptr_t)adata_trace_error,
			(intptr_t)adata_get_read_ptr,
			(intptr_t)adata_skip_read,
			(intptr_t)adata_get_write_ptr,
			(intptr_t)adata_append_write,
			(intptr_t)adata_get_trace_info,
      (intptr_t)adata_get_write_data,
      (intptr_t)adata_get_read_length,
			(intptr_t)adata_get_write_length,
			(intptr_t)adata_set_read_buf,
			(intptr_t)adata_read_fix_int8,
			(intptr_t)adata_read_fix_uint8,
			(intptr_t)adata_read_fix_int16,
			(intptr_t)adata_read_fix_uint16,
			(intptr_t)adata_read_fix_int32,
			(intptr_t)adata_read_fix_uint32,
			(intptr_t)adata_read_fix_int64,
			(intptr_t)adata_read_fix_uint64,
			(intptr_t)adata_read_int8,
			(intptr_t)adata_read_uint8,
			(intptr_t)adata_read_int16,
			(intptr_t)adata_read_uint16,
			(intptr_t)adata_read_int32,
			(intptr_t)adata_read_uint32,
			(intptr_t)adata_read_int64,
			(intptr_t)adata_read_uint64,
			(intptr_t)adata_read_float32,
			(intptr_t)adata_read_float64,
			(intptr_t)adata_skip_read_fix_int8,
			(intptr_t)adata_skip_read_fix_uint8,
			(intptr_t)adata_skip_read_fix_int16,
			(intptr_t)adata_skip_read_fix_uint16,
			(intptr_t)adata_skip_read_fix_int32,
			(intptr_t)adata_skip_read_fix_uint32,
			(intptr_t)adata_skip_read_fix_int64,
			(intptr_t)adata_skip_read_fix_uint64,
			(intptr_t)adata_skip_read_int8,
			(intptr_t)adata_skip_read_uint8,
			(intptr_t)adata_skip_read_int16,
			(intptr_t)adata_skip_read_uint16,
			(intptr_t)adata_skip_read_int32,
			(intptr_t)adata_skip_read_uint32,
			(intptr_t)adata_skip_read_int64,
			(intptr_t)adata_skip_read_uint64,
			(intptr_t)adata_skip_read_float32,
			(intptr_t)adata_skip_read_float64,
			(intptr_t)adata_skip_read_str,
			(intptr_t)adata_write_fix_int8,
			(intptr_t)adata_write_fix_uint8,
			(intptr_t)adata_write_fix_int16,
			(intptr_t)adata_write_fix_uint16,
			(intptr_t)adata_write_fix_int32,
			(intptr_t)adata_write_fix_uint32,
			(intptr_t)adata_write_fix_int64,
			(intptr_t)adata_write_fix_uint64,
			(intptr_t)adata_write_int8,
			(intptr_t)adata_write_uint8,
			(intptr_t)adata_write_int16,
			(intptr_t)adata_write_uint16,
			(intptr_t)adata_write_int32,
			(intptr_t)adata_write_uint32,
			(intptr_t)adata_write_int64,
			(intptr_t)adata_write_uint64,
			(intptr_t)adata_write_float32,
			(intptr_t)adata_write_float64,
			(intptr_t)adata_size_of_fix_int8,
			(intptr_t)adata_size_of_fix_uint8,
			(intptr_t)adata_size_of_fix_int16,
			(intptr_t)adata_size_of_fix_uint16,
			(intptr_t)adata_size_of_fix_int32,
			(intptr_t)adata_size_of_fix_uint32,
			(intptr_t)adata_size_of_fix_int64,
			(intptr_t)adata_size_of_fix_uint64,
			(intptr_t)adata_size_of_int8,
			(intptr_t)adata_size_of_uint8,
			(intptr_t)adata_size_of_int16,
			(intptr_t)adata_size_of_uint16,
			(intptr_t)adata_size_of_int32,
			(intptr_t)adata_size_of_uint32,
			(intptr_t)adata_size_of_int64,
			(intptr_t)adata_size_of_uint64,
			(intptr_t)adata_size_of_float32,
			(intptr_t)adata_size_of_float64,
			(intptr_t)adata_size_of_str,
		};
		int count = sizeof(function_addres) / sizeof(function_addres[0]);
		for (int i = 0; i < count; ++i)
		{
			lua_pushinteger(L, function_addres[i]);
		}
		return count;
	}

}

namespace adata{
	namespace lua {

		ADATA_INLINE int init_adata_corec(lua_State *L)
		{
			static const luaL_Reg lib[] =
			{
				{ "get_functions", adata_get_functions },

				{ NULL, NULL }
			};

			luaL_register(L, "adata_jit_core", lib);
			return 1;
		}

	}
}

#endif
