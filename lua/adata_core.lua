--[[ import C adata function , only work with luajit with C binding ]]
local ffi = require'ffi';
local jcore = require'adata_jit_core';

ffi.cdef[[
  void free (void *ptr);
  typedef void * (__stdcall *adata_new_buf_ft)(size_t size);
  typedef void (__stdcall *adata_buf_opt_ft)(void * buf);
  typedef int (__stdcall *adata_resize_buf_ft)(void * zbuf, size_t size);
  typedef int (__stdcall *adata_clear_buf_ft)(void * zbuf);
  typedef int (__stdcall *adata_set_error_ft)(void * zbuf , int ec);
  typedef int (__stdcall *adata_error_ft)(void * zbuf);
  typedef int (__stdcall *adata_trace_error_ft)(void * zbuf, const char * info, int idx);
  typedef char * (__stdcall *adata_get_trace_info_ft)(void * buf);
  typedef int (__stdcall *adata_set_read_buf_ft)(void * zbuf, const char * data, size_t len);
  typedef void * (__stdcall *adata_get_ptr_ft)(void * buf);
  typedef void * (__stdcall *adata_seek_ptr_ft)(void * buf , size_t len);
  typedef void * (__stdcall *adata_get_write_data_ft)(void * buf);
  typedef size_t (__stdcall *adata_get_read_length_ft)(void * buf);
  typedef size_t (__stdcall *adata_get_write_length_ft)(void * buf);
  typedef int8_t (__stdcall *adata_read_int8_ft)(void * buf);
  typedef uint8_t (__stdcall *adata_read_uint8_ft)(void * buf);
  typedef int16_t (__stdcall *adata_read_int16_ft)(void * buf);
  typedef uint16_t (__stdcall *adata_read_uint16_ft)(void * buf);
  typedef int32_t (__stdcall *adata_read_int32_ft)(void * buf);
  typedef uint32_t (__stdcall *adata_read_uint32_ft)(void * buf);
  typedef int64_t (__stdcall *adata_read_int64_ft)(void * buf);
  typedef uint64_t (__stdcall *adata_read_uint64_ft)(void * buf);
  typedef float (__stdcall *adata_read_float32_ft)(void * buf);
  typedef double (__stdcall *adata_read_float64_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_int8_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_uint8_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_int16_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_uint16_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_int32_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_uint32_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_int64_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_uint64_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_float32_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_float64_ft)(void * buf);
  typedef int (__stdcall *adata_skip_read_str_ft)(void * buf, size_t len);
  typedef int (__stdcall *adata_write_int8_ft)(void * buf, int8_t v);
  typedef int (__stdcall *adata_write_uint8_ft)(void * buf, uint8_t v);
  typedef int (__stdcall *adata_write_int16_ft)(void * buf, int16_t v);
  typedef int (__stdcall *adata_write_uint16_ft)(void * buf, uint16_t v);
  typedef int (__stdcall *adata_write_int32_ft)(void * buf, int32_t v);
  typedef int (__stdcall *adata_write_uint32_ft)(void * buf, uint32_t v);
  typedef int (__stdcall *adata_write_int64_ft)(void * buf, int64_t v);
  typedef int (__stdcall *adata_write_uint64_ft)(void * buf, uint64_t v);
  typedef int (__stdcall *adata_write_float32_ft)(void * buf, float v);
  typedef int (__stdcall *adata_write_float64_ft)(void * buf, double v);
  typedef int (__stdcall *adata_size_of_int8_ft)(int8_t v);
  typedef int (__stdcall *adata_size_of_uint8_ft)(uint8_t v);
  typedef int (__stdcall *adata_size_of_int16_ft)(int16_t v);
  typedef int (__stdcall *adata_size_of_uint16_ft)(uint16_t v);
  typedef int (__stdcall *adata_size_of_int32_ft)(int32_t v);
  typedef int (__stdcall *adata_size_of_uint32_ft)(uint32_t v);
  typedef int (__stdcall *adata_size_of_int64_ft)(int64_t v);
  typedef int (__stdcall *adata_size_of_uint64_ft)(uint64_t v);
  typedef int (__stdcall *adata_size_of_float32_ft)(float v);
  typedef int (__stdcall *adata_size_of_float64_ft)(double v);
  typedef int (__stdcall *adata_size_of_str_ft)(size_t len);
]]

local adata_new_buf,
  adata_del_buf,
  adata_resize_buf,
  adata_clear_buf,
  adata_set_error,
  adata_error,
  adata_trace_error,
  adata_get_read_ptr,
  adata_skip_read,
  adata_get_write_ptr,
  adata_append_write,
  adata_get_trace_info,
  adata_get_write_data,
  adata_get_read_length,
  adata_get_write_length,
  adata_set_read_buf,
  adata_read_fix_int8,
  adata_read_fix_uint8,
  adata_read_fix_int16,
  adata_read_fix_uint16,
  adata_read_fix_int32,
  adata_read_fix_uint32,
  adata_read_fix_int64,
  adata_read_fix_uint64,
  adata_read_int8,
  adata_read_uint8,
  adata_read_int16,
  adata_read_uint16,
  adata_read_int32,
  adata_read_uint32,
  adata_read_int64,
  adata_read_uint64,
  adata_read_float32,
  adata_read_float64,
  adata_skip_read_fix_int8,
  adata_skip_read_fix_uint8,
  adata_skip_read_fix_int16,
  adata_skip_read_fix_uint16,
  adata_skip_read_fix_int32,
  adata_skip_read_fix_uint32,
  adata_skip_read_fix_int64,
  adata_skip_read_fix_uint64,
  adata_skip_read_int8,
  adata_skip_read_uint8,
  adata_skip_read_int16,
  adata_skip_read_uint16,
  adata_skip_read_int32,
  adata_skip_read_uint32,
  adata_skip_read_int64,
  adata_skip_read_uint64,
  adata_skip_read_float32,
  adata_skip_read_float64,
  adata_skip_read_str,
  adata_write_fix_int8,
  adata_write_fix_uint8,
  adata_write_fix_int16,
  adata_write_fix_uint16,
  adata_write_fix_int32,
  adata_write_fix_uint32,
  adata_write_fix_int64,
  adata_write_fix_uint64,
  adata_write_int8,
  adata_write_uint8,
  adata_write_int16,
  adata_write_uint16,
  adata_write_int32,
  adata_write_uint32,
  adata_write_int64,
  adata_write_uint64,
  adata_write_float32,
  adata_write_float64,
  adata_size_of_fix_int8,
  adata_size_of_fix_uint8,
  adata_size_of_fix_int16,
  adata_size_of_fix_uint16,
  adata_size_of_fix_int32,
  adata_size_of_fix_uint32,
  adata_size_of_fix_int64,
  adata_size_of_fix_uint64,
  adata_size_of_int8,
  adata_size_of_uint8,
  adata_size_of_int16,
  adata_size_of_uint16,
  adata_size_of_int32,
  adata_size_of_uint32,
  adata_size_of_int64,
  adata_size_of_uint64,
  adata_size_of_float32,
  adata_size_of_float64,
  adata_size_of_str= jcore.get_functions();

  adata_new_buf = ffi.cast("adata_new_buf_ft",adata_new_buf);
  adata_del_buf = ffi.cast("adata_buf_opt_ft",adata_del_buf);
  adata_resize_buf = ffi.cast("adata_resize_buf_ft",adata_resize_buf);
  adata_clear_buf = ffi.cast("adata_clear_buf_ft",adata_clear_buf);
  adata_set_error = ffi.cast("adata_set_error_ft",adata_set_error);
  adata_error = ffi.cast("adata_error_ft",adata_error);
  adata_trace_error = ffi.cast("adata_trace_error_ft",adata_trace_error);
  adata_get_read_ptr = ffi.cast("adata_get_ptr_ft",adata_get_read_ptr);
  adata_skip_read = ffi.cast("adata_seek_ptr_ft",adata_skip_read);
  adata_get_write_ptr = ffi.cast("adata_get_ptr_ft",adata_get_write_ptr);
  adata_append_write = ffi.cast("adata_seek_ptr_ft",adata_append_write);
  adata_get_trace_info = ffi.cast("adata_get_trace_info_ft",adata_get_trace_info);
  adata_get_write_data = ffi.cast("adata_get_write_data_ft",adata_get_write_data);
  adata_get_read_length = ffi.cast("adata_get_read_length_ft",adata_get_read_length);
  adata_get_write_length = ffi.cast("adata_get_write_length_ft",adata_get_write_length);
  adata_set_read_buf = ffi.cast("adata_set_read_buf_ft",adata_set_read_buf);
  adata_read_fix_int8 = ffi.cast("adata_read_int8_ft",adata_read_fix_int8);
  adata_read_fix_uint8 = ffi.cast("adata_read_uint8_ft",adata_read_fix_uint8);
  adata_read_fix_int16 = ffi.cast("adata_read_int16_ft",adata_read_fix_int16);
  adata_read_fix_uint16 = ffi.cast("adata_read_uint16_ft",adata_read_fix_uint16);
  adata_read_fix_int32 = ffi.cast("adata_read_int32_ft",adata_read_fix_int32);
  adata_read_fix_uint32 = ffi.cast("adata_read_uint32_ft",adata_read_fix_uint32);
  adata_read_fix_int64 = ffi.cast("adata_read_int64_ft",adata_read_fix_int64);
  adata_read_fix_uint64 = ffi.cast("adata_read_uint64_ft",adata_read_fix_uint64);
  adata_read_int8 = ffi.cast("adata_read_int8_ft",adata_read_int8);
  adata_read_uint8 = ffi.cast("adata_read_uint8_ft",adata_read_uint8);
  adata_read_int16 = ffi.cast("adata_read_int16_ft",adata_read_int16);
  adata_read_uint16 = ffi.cast("adata_read_uint16_ft",adata_read_uint16);
  adata_read_int32 = ffi.cast("adata_read_int32_ft",adata_read_int32);
  adata_read_uint32 = ffi.cast("adata_read_uint32_ft",adata_read_uint32);
  adata_read_int64 = ffi.cast("adata_read_int64_ft",adata_read_int64);
  adata_read_uint64 = ffi.cast("adata_read_uint64_ft",adata_read_uint64);
  adata_read_float32 = ffi.cast("adata_read_float32_ft",adata_read_float32);
  adata_read_float64 = ffi.cast("adata_read_float64_ft",adata_read_float64);
  adata_skip_read_fix_int8 = ffi.cast("adata_skip_read_int8_ft",adata_skip_read_fix_int8);
  adata_skip_read_fix_uint8 = ffi.cast("adata_skip_read_uint8_ft",adata_skip_read_fix_uint8);
  adata_skip_read_fix_int16 = ffi.cast("adata_skip_read_int16_ft",adata_skip_read_fix_int16);
  adata_skip_read_fix_uint16 = ffi.cast("adata_skip_read_uint16_ft",adata_skip_read_fix_uint16);
  adata_skip_read_fix_int32 = ffi.cast("adata_skip_read_int32_ft",adata_skip_read_fix_int32);
  adata_skip_read_fix_uint32 = ffi.cast("adata_skip_read_uint32_ft",adata_skip_read_fix_uint32);
  adata_skip_read_fix_int64 = ffi.cast("adata_skip_read_int64_ft",adata_skip_read_fix_int64);
  adata_skip_read_fix_uint64 = ffi.cast("adata_skip_read_uint64_ft",adata_skip_read_fix_uint64);
  adata_skip_read_int8 = ffi.cast("adata_skip_read_int8_ft",adata_skip_read_int8);
  adata_skip_read_uint8 = ffi.cast("adata_skip_read_uint8_ft",adata_skip_read_uint8);
  adata_skip_read_int16 = ffi.cast("adata_skip_read_int16_ft",adata_skip_read_int16);
  adata_skip_read_uint16 = ffi.cast("adata_skip_read_uint16_ft",adata_skip_read_uint16);
  adata_skip_read_int32 = ffi.cast("adata_skip_read_int32_ft",adata_skip_read_int32);
  adata_skip_read_uint32 = ffi.cast("adata_skip_read_uint32_ft",adata_skip_read_uint32);
  adata_skip_read_int64 = ffi.cast("adata_skip_read_int64_ft",adata_skip_read_int64);
  adata_skip_read_uint64 = ffi.cast("adata_skip_read_uint64_ft",adata_skip_read_uint64);
  adata_skip_read_float32 = ffi.cast("adata_skip_read_float32_ft",adata_skip_read_float32);
  adata_skip_read_float64 = ffi.cast("adata_skip_read_float64_ft",adata_skip_read_float64);
  adata_skip_read_str = ffi.cast("adata_skip_read_str_ft",adata_skip_read_str);
  adata_write_fix_int8 = ffi.cast("adata_write_int8_ft",adata_write_fix_int8);
  adata_write_fix_uint8 = ffi.cast("adata_write_uint8_ft",adata_write_fix_uint8);
  adata_write_fix_int16 = ffi.cast("adata_write_int16_ft",adata_write_fix_int16);
  adata_write_fix_uint16 = ffi.cast("adata_write_uint16_ft",adata_write_fix_uint16);
  adata_write_fix_int32 = ffi.cast("adata_write_int32_ft",adata_write_fix_int32);
  adata_write_fix_uint32 = ffi.cast("adata_write_uint32_ft",adata_write_fix_uint32);
  adata_write_fix_int64 = ffi.cast("adata_write_int64_ft",adata_write_fix_int64);
  adata_write_fix_uint64 = ffi.cast("adata_write_uint64_ft",adata_write_fix_uint64);
  adata_write_int8 = ffi.cast("adata_write_int8_ft",adata_write_int8);
  adata_write_uint8 = ffi.cast("adata_write_uint8_ft",adata_write_uint8);
  adata_write_int16 = ffi.cast("adata_write_int16_ft",adata_write_int16);
  adata_write_uint16 = ffi.cast("adata_write_uint16_ft",adata_write_uint16);
  adata_write_int32 = ffi.cast("adata_write_int32_ft",adata_write_int32);
  adata_write_uint32 = ffi.cast("adata_write_uint32_ft",adata_write_uint32);
  adata_write_int64 = ffi.cast("adata_write_int64_ft",adata_write_int64);
  adata_write_uint64 = ffi.cast("adata_write_uint64_ft",adata_write_uint64);
  adata_write_float32 = ffi.cast("adata_write_float32_ft",adata_write_float32);
  adata_write_float64 = ffi.cast("adata_write_float64_ft",adata_write_float64);
  adata_size_of_fix_int8 = ffi.cast("adata_size_of_int8_ft",adata_size_of_fix_int8);
  adata_size_of_fix_uint8 = ffi.cast("adata_size_of_uint8_ft",adata_size_of_fix_uint8);
  adata_size_of_fix_int16 = ffi.cast("adata_size_of_int16_ft",adata_size_of_fix_int16);
  adata_size_of_fix_uint16 = ffi.cast("adata_size_of_uint16_ft",adata_size_of_fix_uint16);
  adata_size_of_fix_int32 = ffi.cast("adata_size_of_int32_ft",adata_size_of_fix_int32);
  adata_size_of_fix_uint32 = ffi.cast("adata_size_of_uint32_ft",adata_size_of_fix_uint32);
  adata_size_of_fix_int64 = ffi.cast("adata_size_of_int64_ft",adata_size_of_fix_int64);
  adata_size_of_fix_uint64 = ffi.cast("adata_size_of_uint64_ft",adata_size_of_fix_uint64);
  adata_size_of_int8 = ffi.cast("adata_size_of_int8_ft",adata_size_of_int8);
  adata_size_of_uint8 = ffi.cast("adata_size_of_uint8_ft",adata_size_of_uint8);
  adata_size_of_int16 = ffi.cast("adata_size_of_int16_ft",adata_size_of_int16);
  adata_size_of_uint16 = ffi.cast("adata_size_of_uint16_ft",adata_size_of_uint16);
  adata_size_of_int32 = ffi.cast("adata_size_of_int32_ft",adata_size_of_int32);
  adata_size_of_uint32 = ffi.cast("adata_size_of_uint32_ft",adata_size_of_uint32);
  adata_size_of_int64 = ffi.cast("adata_size_of_int64_ft",adata_size_of_int64);
  adata_size_of_uint64 = ffi.cast("adata_size_of_uint64_ft",adata_size_of_uint64);
  adata_size_of_float32 = ffi.cast("adata_size_of_float32_ft",adata_size_of_float32);
  adata_size_of_float64 = ffi.cast("adata_size_of_float64_ft",adata_size_of_float64);
  adata_size_of_str = ffi.cast("adata_size_of_str_ft",adata_size_of_str);

m = {
  new_buf = function(s)
    local buf = adata_new_buf(s);
    ffi.gc(buf,adata_del_buf);
    return buf;
  end,
  resize_buf = function(buf,s)
    return adata_resize_buf(buf,s);
  end,
  clear_buf = function(buf)
    return adata.clear_buf(buf);
  end,
  set_error = function(buf,e)
    return adata_set_error(buf,e);
  end,
  trace_error = function(buf,info,idx)
    return adata_trace_error(buf,info,idx);
  end,
  trace_info = function(buf)
    local data = adata_get_trace_info(buf);
    local str = ffi.string(data);
    ffi.C.free(data);
    return str;
  end,
  get_write_data = function(buf)
    local ptr = adata_get_write_data(buf);
    local len = adata_get_write_length(buf);
    return ffi.string(ptr,len);
  end,
  get_rd_len = function(buf)
    return adata_get_read_length(buf);
  end,
  get_wt_len = function(buf)
    return adata_get_write_length(buf);
  end,
  set_read_data = function(buf,data,len)
    if len == nil then len = #data; end;
    local cdata = ffi.cast("const char*",data,len);
    adata_set_read_buf(buf,cdata,len);
  end,
  rd_fixi8 = function(buf)
    local v = adata_read_fix_int8(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixu8 = function(buf)
    local v = adata_read_fix_uint8(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixi16 = function(buf)
    local v = adata_read_fix_int16(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixu16 = function(buf)
    local v = adata_read_fix_uint16(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixi32 = function(buf)
    local v = adata_read_fix_int32(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixu32 = function(buf)
    local v = adata_read_fix_uint32(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixi64 = function(buf)
    local v = adata_read_fix_int64(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_fixu64 = function(buf)
    local v = adata_read_fix_uint64(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_i8 = function(buf)
    local v = adata_read_int8(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_u8 = function(buf)
    local v = adata_read_uint8(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_i16 = function(buf)
    local v = adata_read_int16(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_u16 = function(buf)
    local v = adata_read_uint16(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_i32 = function(buf)
    local v = adata_read_int32(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_u32 = function(buf)
    local v = adata_read_uint32(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_i64 = function(buf)
    local v = adata_read_int64(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_u64 = function(buf)
    local v = adata_read_uint64(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_f32 = function(buf)
    local v = adata_read_float32(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_f64 = function(buf)
    local v = adata_read_float64(buf);
    local ec = adata_error(buf);
    return ec,v;
  end,
  rd_str = function(buf,len)
    local slen = adata_read_uint32(buf);
    local ec = adata_error(buf);
    if ec > 0 then return ec , nil; end;
    if len > 0 and slen > len then adata_set_error(3); return 3 , nil; end;
    local rptr = adata_skip_read(buf,slen);
    local ec = adata_error(buf);
    if ec > 0 then return ec , nil; end;
    return 0,ffi.string(rptr,slen);
  end,
  skip_rd_fixi8 = function(buf)
    adata_skip_read_fix_int8(buf);
    local ec = adata_error(buf);
    return ec;
  end,	
  skip_rd_fixu8 = function(buf)
    adata_skip_read_fix_uint8(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixi16 = function(buf)
    adata_skip_read_fix_int16(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixu16 = function(buf)
    adata_skip_read_fix_uint16(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixi32 = function(buf)
    adata_skip_read_fix_int32(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixu32 = function(buf)
    adata_skip_read_fix_uint32(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixi64 = function(buf)
    adata_skip_read_fix_int64(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_fixu64 = function(buf)
    adata_skip_read_fix_uint64(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_i8 = function(buf)
    adata_skip_read_int8(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_u8 = function(buf)
    adata_skip_read_uint8(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_i16 = function(buf)
    adata_skip_read_int16(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_u16 = function(buf)
    adata_skip_read_uint16(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_i32 = function(buf)
    adata_skip_read_int32(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_u32 = function(buf)
    adata_skip_read_uint32(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_i64 = function(buf)
    adata_skip_read_int64(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_u64 = function(buf)
    adata_skip_read_uint64(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_f32 = function(buf)
    adata_skip_read_float32(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_f64 = function(buf)
    adata_skip_read_float64(buf);
    local ec = adata_error(buf);
    return ec;
  end,
  skip_rd_str = function(buf,len)
    adata_skip_read_str(buf,len);
    local ec = adata_error(buf);
    return ec;
  end,
  wt_fixi8 = function(buf,v)
    return adata_write_fix_int8(buf,v);
  end,
  wt_fixu8 = function(buf,v)
    return adata_write_fix_uint8(buf,v);
  end,
  wt_fixi16 = function(buf,v)
    return adata_write_fix_int16(buf,v);
  end,
  wt_fixu16 = function(buf,v)
    return adata_write_fix_uint16(buf,v);
  end,
  wt_fixi32 = function(buf,v)
    return adata_write_fix_int32(buf,v);
  end,
  wt_fixu32 = function(buf,v)
    return adata_write_fix_uint32(buf,v);
  end,
  wt_fixi64 = function(buf,v)
    return adata_write_fix_int64(buf,v);
  end,
  wt_fixu64 = function(buf,v)
    return adata_write_fix_uint64(buf,v);
  end,
  wt_i8 = function(buf,v)
    return adata_write_int8(buf,v);
  end,
  wt_u8 = function(buf,v)
    return adata_write_uint8(buf,v);
  end,
  wt_i16 = function(buf,v)
    return adata_write_int16(buf,v);
  end,
  wt_u16 = function(buf,v)
    return adata_write_uint16(buf,v);
  end,
  wt_i32 = function(buf,v)
    return adata_write_int32(buf,v);
  end,
  wt_u32 = function(buf,v)
    return adata_write_uint32(buf,v);
  end,
  wt_i64 = function(buf,v)
    return adata_write_int64(buf,v);
  end,
  wt_u64 = function(buf,v)
    return adata_write_uint64(buf,v);
  end,
  wt_f32 = function(buf,v)
    return adata_write_float32(buf,v);
  end,
  wt_f64 = function(buf,v)
    return adata_write_float64(buf,v);
  end,
  wt_str = function(buf,s,len)
    local slen = #s;
    if len > 0 and slen > len then return 3; end;
    local ec = adata_write_uint32(buf,slen);
    if ec > 0 then return ec; end;
    local wptr = adata_append_write(buf,slen);
    if ec > 0 then return ec; end;
    ffi.copy(wptr,s,slen);
    return 0;
  end,
  szof_fixi8 = function(v)
    return adata_size_of_fix_int8(v);
  end,
  szof_fixu8 = function(v)
    return adata_size_of_fix_uint8(v);
  end,
  szof_fixi16 = function(v)
    return adata_size_of_fix_int16(v);
  end,
  szof_fixu16 = function(v)
    return adata_size_of_fix_uint16(v);
  end,
  szof_fixi32 = function(v)
    return adata_size_of_fix_int32(v);
  end,
  szof_fixu32 = function(v)
    return adata_size_of_fix_uint32(v);
  end,
  szof_fixi64 = function(v)
    return adata_size_of_fix_int64(v);
  end,
  szof_fixu64 = function(v)
    return adata_size_of_fix_uint64(v);
  end,
  szof_i8 = function(v)
    return adata_size_of_int8(v);
  end,
  szof_u8 = function(v)
    return adata_size_of_uint8(v);
  end,
  szof_i16 = function(v)
    return adata_size_of_int16(v);
  end,
  szof_u16 = function(v)
    return adata_size_of_uint16(v);
  end,
  szof_i32 = function(v)
    return adata_size_of_int32(v);
  end,
  szof_u32 = function(v)
    return adata_size_of_uint32(v);
  end,
  szof_i64 = function(v)
    return adata_size_of_int64(v);
  end,
  szof_u64 = function(v)
    return adata_size_of_uint64(v);
  end,
  szof_f32 = function(v)
    return adata_size_of_float32(v);
  end,
  szof_f64 = function(v)
    return adata_size_of_float64(v);
  end,
  szof_str = function(s)
    return adata_size_of_str(#s);
  end,
 
}
return m
