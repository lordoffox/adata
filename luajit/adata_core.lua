--[[ only work with luajit without C adata function , usally work in client program woth luajit script ]]
local ffi = require("ffi")

local le = ffi.abi('le');

local p2_1 = 1;
local p2_2 = 0;
local p4_1 = 3;
local p4_2 = 2;
local p4_3 = 1;
local p4_4 = 0;
local p8_1 = 7;
local p8_2 = 6;
local p8_3 = 5;
local p8_4 = 4;
local p8_5 = 3;
local p8_6 = 2;
local p8_7 = 1;
local p8_8 = 0;

if le then
  p2_1 = 0;
  p2_2 = 1;
  p4_1 = 0;
  p4_2 = 1;
  p4_3 = 2;
  p4_4 = 3;
  p8_1 = 0;
  p8_2 = 1;
  p8_3 = 2;
  p8_4 = 3;
  p8_5 = 4;
  p8_6 = 5;
  p8_7 = 6;
  p8_8 = 7;
end

ffi.cdef[[
union uval_t{
  int8_t    i8;
  uint8_t   u8;
  int16_t   i16;
  uint16_t  u16;
  int32_t   i32;
  uint32_t  u32;
  int64_t   i64;
  uint64_t  u64;
  float     f;
  double    d;
  uint8_t   b[8];
  };
]]

local const_tag_as_value = 0x7f;
local const_tag_as_type = const_tag_as_value + 1;
local const_interger_byte_msak = 0x7;
local const_negative_bit_value = 0x20;
local const_negative_tag = const_tag_as_type + const_negative_bit_value;

local ec_success = 0;
local ec_negative_assign_to_unsigned_integer_number = 1;
local ec_value_outof_ranger_to_integer_number = 2;
local ec_sequence_length_overflow = 3;
local ec_stream_buffer_overflow = 4;
local ec_number_of_element_not_macth = 5;
local ec_undefined_member_protocol_not_compatible = 6;
local ec_value_not_match_number = 7;
local ec_error_value_fix_int8 = 8;
local ec_error_value_fix_uint8 = 9;
local ec_error_value_fix_int16 = 10;
local ec_error_value_fix_uint16 = 11;
local ec_error_value_fix_int32 = 12;
local ec_error_value_fix_uint32 = 13;
local ec_error_value_fix_int64 = 14;
local ec_error_value_fix_uint64 = 15;
local ec_error_value_int8 = 16;
local ec_error_value_uint8 = 17;
local ec_error_value_int16 = 18;
local ec_error_value_uint16 = 19;
local ec_error_value_int32 = 20;
local ec_error_value_uint32 = 21;
local ec_error_value_int64 = 22;
local ec_error_value_uint64 = 23;
local ec_error_value_float32 = 24;
local ec_error_value_double64 = 25;
local ec_error_value_string = 26;
local ec_error_value_list = 27;
local ec_error_value_map = 28;

local m = {};

local decode_tag = function(tag)
  local nbit = false;
  local tag_as_value = (tag <= const_tag_as_value);
  local bytes = 1;
  if tag_as_value == false then
    if tag >= const_negative_tag then
      nbit = true;
      bytes = tag - const_tag_as_type - const_negative_bit_value + 1;
    else
      bytes = tag - const_tag_as_type + 1;
    end
  end
  return tag_as_value , nbit , bytes
end

local encode_tag = function(v)
  local nbit = false;
  local tag_as_value = (v>=0 and v < const_tag_as_type);
  if tag_as_value then return tag_as_value , v , v; end
  local tag = const_tag_as_type;
  if v < 0 then
    v = -v;
    tag = const_negative_tag;
    nbit = true;
  end
  return tag_as_value , tag , v
end

local new_buf = function(n)
  local b = {};
  b.uv = ffi.new("union uval_t");
  b.e = 0;
  b.rlen = 0;
  b.wlen = 0;
  b.wcap = n;
  b.wbuf = ffi.new("uint8_t[?]", n);
  b.rcap = 0;
  b.rbuf = nil;
  b.trace = {};
  b.trace_count = 1;
   
  return b;
end

m.new_buf = new_buf;

local resize_buf = function(b , n)
  if(b.wcap < n) then
      b.wcap = n;
      b.wbuf = ffi.new("uint8_t[?]", n);
  end
end

m.resize_buf = resize_buf;

local clear_read = function(b)
  b.e = 0;
  b.rlen = 0;
  b.wlen = 0;
  b.trace = {};
  b.trace_count = 1;
end
m.clear_read = clear_read;

local clear_write = function(b)
  b.e = 0;
  b.rlen = 0;
  b.wlen = 0;
  b.trace = {};
  b.trace_count = 1;
end
m.clear_write = clear_write;

local clear_buf = function(b)
  b.e = 0;
  b.rlen = 0;
  b.wlen = 0;
  b.trace = {};
  b.trace_count = 1;
end

m.clear_buf = clear_buf;

local set_error = function(b,e)
  b.e = e;
end

m.set_error = set_error;

local trace_error = function(b,info,idx)
  b.trace[b.trace_count] = {info,idx};
  b.trace_count = b.trace_count +1;
end

m.trace_error = trace_error;

local tcat = table.concat;

local trace_info = function(b)
  local info = {};
  local c = 1;
  for _,v in ipairs(b.trace) do
    info[c] = v[1]; c = c + 1;
    if v[2] ~= -1 then
      info[c] = '['..v[1]..']'; c = c + 1;
    end
    info[c] = '.'; c = c + 1;
  end
  return tcat(info);
end

m.trace_info = trace_info;

local get_wt_data = function(b,dont_clear)
  local str = ffi.string(b.wbuf , b.wlen);
  if dont_clear ~= 1 then clear_buf(b); end
  return str;
end

m.get_write_data = get_wt_data;

local set_rd_data = function(b,s)
  b.rcap = #s;
  b.rbuf = ffi.cast("uint8_t *", s);
  b.rlen = 0;
  b.e = 0;
end
m.set_read_data = set_rd_data;

local loadbyte2p = function(b , n)
  if b.rlen + n > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p2_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p2_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  return 0;
end

local savebyte2p = function(b , n)
  if b.wlen + n > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1; end
  return 0;
end

local loadbyte4p = function(b , n)
  if b.rlen + n > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p4_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p4_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 2 then b.uv.b[p4_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 3 then b.uv.b[p4_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  return 0;
end

local savebyte4p = function(b , n)
  if b.wlen + n > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1; end
  if n > 2 then b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1; end
  if n > 3 then b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1; end
  return 0;
end

local loadbyte8p = function(b , n)
  if b.rlen + n > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p8_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p8_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 2 then b.uv.b[p8_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 3 then b.uv.b[p8_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 4 then b.uv.b[p8_5] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 5 then b.uv.b[p8_6] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 6 then b.uv.b[p8_7] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  if n > 7 then b.uv.b[p8_8] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1; end
  return 0;
end

local savebyte8p = function(b , n)
  if b.wlen + n > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p8_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p8_2];  b.wlen = b.wlen + 1; end
  if n > 2 then b.wbuf[b.wlen] = b.uv.b[p8_3];  b.wlen = b.wlen + 1; end
  if n > 3 then b.wbuf[b.wlen] = b.uv.b[p8_4];  b.wlen = b.wlen + 1; end
  if n > 4 then b.wbuf[b.wlen] = b.uv.b[p8_5];  b.wlen = b.wlen + 1; end
  if n > 5 then b.wbuf[b.wlen] = b.uv.b[p8_6];  b.wlen = b.wlen + 1; end
  if n > 6 then b.wbuf[b.wlen] = b.uv.b[p8_7];  b.wlen = b.wlen + 1; end
  if n > 7 then b.wbuf[b.wlen] = b.uv.b[p8_8];  b.wlen = b.wlen + 1; end
  return 0;
end

local rd_fixi8 = function(b)
  if b.rlen + 1 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0;
  end
  b.uv.u8 = b.rbuf[b.rlen];
  b.rlen = b.rlen + 1;
  return 0 , b.uv.i8;
end

m.rd_fixi8 = rd_fixi8;

local rd_fixu8 = function(b)
  if b.rlen + 1 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0;
  end
  local v  = b.rbuf[b.rlen];
  b.rlen = b.rlen + 1;
  return 0 , v;
end

m.rd_fixu8 = rd_fixu8;

local wt_fixi8 = function(b , v)
  if b.wlen + 1 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < -127 or v > 127 then b.e = ec_error_value_fix_int8; return b.e; end
  b.uv.i8 = v;
  b.wbuf[b.wlen] = b.uv.u8;
  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixi8 = wt_fixi8;

local wt_fixu8 = function(b , v)
  if b.wlen + 1 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < 0 or v > 256 then b.e = ec_error_value_fix_uint8; return b.e; end
  b.wbuf[b.wlen] = v;
  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixu8 = wt_fixu8;

local rd_fixi16 = function(b)
  if b.rlen + 2 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p2_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p2_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return  0 , b.uv.i16;
end

m.rd_fixi16 = rd_fixi16;

local rd_fixu16 = function(b)
  if b.rlen + 2 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p2_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p2_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return  0 , b.uv.u16;
end
 
m.rd_fixu16 = rd_fixu16;
 
local wt_fixi16 = function(b , v)
  if b.wlen + 2 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < -32767 or v > 32767 then b.e = ec_error_value_fix_int16; return b.e; end
  b.uv.i16 = v;
  b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixi16 = wt_fixi16;

local wt_fixu16 = function(b , v)
  if b.wlen + 2 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < 0 or v > 65535 then b.e = ec_error_value_fix_uint16; return b.e; end
  b.uv.u16 = v;
  b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixu16 = wt_fixu16;

local rd_fixi32 = function(b)
  if b.rlen + 4 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p4_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return  0 , b.uv.i32;
end

m.rd_fixi32 = rd_fixi32;

local rd_fixu32 = function(b)
  if b.rlen + 4 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p4_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return  0 , b.uv.u32;
end

m.rd_fixu32 = rd_fixu32;

local wt_fixi32 = function(b , v)
  if b.wlen + 4 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < -2147483647 or v > 2147483647 then b.e = ec_error_value_fix_int32; return b.e; end
  b.uv.i32 = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixi32 = wt_fixi32;

local wt_fixu32 = function(b , v)
  if b.wlen + 4 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  if v < 0 or v > 4294967295 then b.e = ec_error_value_fix_uint32; return; end
  b.uv.u32 = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixu32 = wt_fixu32;

local rd_fixi64 = function(b)
  if b.rlen + 8 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p8_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return 0 , b.uv.i64;
end

m.rd_fixi64 = rd_fixi64;

local rd_fixu64 = function(b)
  if b.rlen + 8 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p8_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return 0 , b.uv.u64;
end

m.rd_fixu64 = rd_fixu64;

local wt_fixi64 = function(b , v)
  if b.wlen + 8 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  b.uv.i64 = v;
  b.wbuf[b.wlen] = b.uv.b[p8_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_4];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_5];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_6];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_7];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_8];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixi64 = wt_fixi64;

local wt_fixu64 = function(b , v)
  if b.wlen + 8 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  b.uv.u64 = v;
  b.wbuf[b.wlen] = b.uv.b[p8_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_4];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_5];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_6];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_7];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_8];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_fixu64 = wt_fixu64;

local rd_f32 = function(b)
  if b.rlen + 4 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0.0
  end
  b.i64 = 0;
  b.uv.b[p4_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return  0 , b.uv.f;
end

m.rd_f32 = rd_f32;

local rd_f64 = function(b)
  if b.rlen + 8 > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e , 0
  end
  b.i64 = 0;
  b.uv.b[p8_1] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = b.rbuf[b.rlen];  b.rlen = b.rlen + 1;    
  return 0 , b.uv.d;
end

m.rd_f64 = rd_f64;

local wt_f32 = function(b,v)
  if b.wlen + 4 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  b.uv.f = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_f32 = wt_f32;

local wt_f64 = function(b,v)
  if b.wlen + 8 > b.wcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  b.uv.d = v;
  b.wbuf[b.wlen] = b.uv.b[p8_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_4];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_5];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_6];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_7];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p8_8];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_f64 = wt_f64;

local rd_i8 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 1 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0;
  end
  if tag_as_value == false then
    ec , tag = rd_fixu8(b);
    if ec > 0 then return ec , 0 ; end;
    if nbit == true then
      tag = tag * -1;
    end
  end
  return ec , tag;
end

m.rd_i8 = rd_i8;

local rd_u8 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 1 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec , tag = rd_fixu8(b);    
  end
  return ec , tag;
end

m.rd_u8 = rd_u8;

local rd_i16 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 2 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte2p(b,bytes);
    tag = b.uv.i16    
    if nbit == true then
      tag = tag * -1;
    end
  end
  return 0 , tag;
end

m.rd_i16 = rd_i16;

local rd_u16 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 2 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte2p(b,bytes);
    tag = b.uv.u16    
  end
  return ec , tag;
end

m.rd_u16 = rd_u16;

local rd_i32 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 4 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte4p(b,bytes);
    tag = b.uv.i32 
    if nbit == true then
      tag = tag * -1;
    end
  end
  return ec , tag;
end

m.rd_i32 = rd_i32;

local rd_u32 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 4 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte4p(b,bytes);
    tag = b.uv.u32 
  end
  return ec , tag;
end

m.rd_u32 = rd_u32;

local rd_i64 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 8 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte8p(b,bytes);
    tag = b.uv.i64
    if nbit == true then
      tag = tag * -1;
    end
  end
  return ec , tag;
end

m.rd_i64 = rd_i64;

local rd_u64 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec , 0 ; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 8 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e , 0
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e , 0
  end
  if tag_as_value == false then
    ec = loadbyte8p(b,bytes);
    tag = b.uv.u64
  end
  return ec , tag;
end

m.rd_u64 = rd_u64;

local rd_str = function(b , n)
  local ec , len = rd_u32(b);
  if ec > 0 then return ec; end
  if n > 0 then
    if len > n then b.e = ec_sequence_length_overflow; return b.e; end
  end
  if b.rlen + len > b.rcap then b.e = ec_stream_buffer_overflow; return b.e; end
  local str = ffi.string(b.rbuf + b.rlen , len);
  b.rlen = b.rlen + len;
  return  0 , str;
end

m.rd_str = rd_str;

local skip_rd = function(b , n)
  if b.rlen + n > b.rcap then
    b.e = ec_stream_buffer_overflow;
    return b.e;
  end
  b.rlen = b.rlen + n;
  return 0;
end

local skip_rd_fixi8 = function(b)
  return skip_rd(b,1);
end

m.skip_rd_fixi8 = skip_rd_fixi8;
m.skip_rd_fixu8 = skip_rd_fixi8;

local skip_rd_fixi16 = function(b)
  return skip_rd(b,2);
end

m.skip_rd_fixi16 = skip_rd_fixi16;
m.skip_rd_fixu16 = skip_rd_fixi16;

local skip_rd_fixi32 = function(b)
  return skip_rd(b,4);
end

m.skip_rd_fixi32 = skip_rd_fixi32;
m.skip_rd_fixu32 = skip_rd_fixi32;

local skip_rd_fixi64 = function(b)
  return skip_rd(b,8);
end

m.skip_rd_fixi64 = skip_rd_fixi64;
m.skip_rd_fixu64 = skip_rd_fixi64;

m.skip_rd_f32 = skip_rd_fixi32;
m.skip_rd_f64 = skip_rd_fixi64;

local skip_rd_i8 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , _ , bytes = decode_tag(tag);
  if bytes > 1 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_i8 = skip_rd_i8;

local skip_rd_u8 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 1 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_u8 = skip_rd_u8;

local skip_rd_i16 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , _ , bytes = decode_tag(tag);
  if bytes > 2 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_i16 = skip_rd_i16;

local skip_rd_u16 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 2 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_u16 = skip_rd_u16;

local skip_rd_i32 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , _ , bytes = decode_tag(tag);
  if bytes > 4 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_i32 = skip_rd_i32;

local skip_rd_u32 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 4 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_u32 = skip_rd_u32;

local skip_rd_i64 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , _ , bytes = decode_tag(tag);
  if bytes > 8 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_i64 = skip_rd_i64;

local skip_rd_u64 = function(b)
  local ec , tag = rd_fixu8(b);
  if ec > 0 then return ec; end;
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 8 then
    b.e = ec_value_outof_ranger_to_integer_number;
    return b.e;
  end
  if nbit == true then
    b.e = ec_negative_assign_to_unsigned_integer_number;
    return b.e;
  end
  if tag_as_value == false then
    ec = skip_rd(b,bytes);
  end
  return ec;
end

m.skip_rd_u64 = skip_rd_u64;

local skip_rd_str = function(b , n)
  local ec , len = rd_u32(b);
  if ec > 0 then return ec; end
  if n > 0 then
    if len > n then b.e = ec_sequence_length_overflow; return b.e; end
  end
  if b.rlen + len > b.rcap then b.e = ec_stream_buffer_overflow; return b.e; end
  b.rlen = b.rlen + len;
end

m.skip_rd_str = skip_rd_str;

local wt_i8 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    if v > 127 then b.e = ec_error_value_int8; return b.e; end
    ec = wt_fixu8(b,tag);
    if ec == 0 then ec = wt_fixu8(b,v); end
  end
  return ec;
end

m.wt_i8 = wt_i8;

local wt_u8 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    if v > 255 then b.e = ec_error_value_uint8; return; end
    ec = wt_fixu8(b,tag);
    if ec == 0 then ec = wt_fixu8(b,v);  end;
  end
  return ec;
end

m.wt_u8 = wt_u8;

local wt_i16 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x8000 then
      bytes = 1;
    else
      b.e = ec_error_value_int16;
      ec = b.e;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.i16 = v; ec = savebyte2p(b,bytes+1); end
  end
  return ec;
end

m.wt_i16 = wt_i16;

local wt_u16 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    else
      b.e = ec_error_value_uint16;
      ec = b.e;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.u16 = v; ec = savebyte2p(b,bytes+1); end
  end
  return ec;
end

m.wt_u16 = wt_u16;

local wt_i32 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    elseif v < 0x1000000 then
      bytes = 2;
    elseif v < 0x80000000 then
      bytes = 3;
    else
      b.e = ec_error_value_int32;
      ec = b.e;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.i32 = v; ec = savebyte4p(b,bytes+1); end
  end
  return ec;
end

m.wt_i32 = wt_i32;

local wt_u32 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    elseif v < 0x1000000 then
      bytes = 2;
    elseif v < 0x100000000 then
      bytes = 3;
    else
      b.e = ec_error_value_uint32;
      ec = b.e;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.u32 = v; ec = savebyte4p(b,bytes+1); end
  end
  return ec;
end

m.wt_u32 = wt_u32;

local wt_i64 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    elseif v < 0x1000000 then
      bytes = 2;
    elseif v < 0x100000000 then
      bytes = 3;
    elseif v < 0x10000000000 then
      bytes = 4;
    elseif v < 0x1000000000000LL then
      bytes = 5;
    elseif v < 0x100000000000000LL then
      bytes = 6;
    elseif v < 0x8000000000000000LL then
      bytes = 7;
    else
      b.e = ec_error_value_int64;
      ec = b.e;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.i64 = v; ec = savebyte8p(b,bytes+1); end
  end
  return ec;
end

m.wt_i64 = wt_i64;

local wt_u64 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  local ec = 0;
  if tag_as_value == true then
    ec = wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    elseif v < 0x1000000 then
      bytes = 2;
    elseif v < 0x100000000 then
      bytes = 3;
    elseif v < 0x10000000000 then
      bytes = 4;
    elseif v < 0x1000000000000ULL then
      bytes = 5;
    elseif v < 0x100000000000000ULL then
      bytes = 6;
    else
      bytes = 7;
    end
    ec = wt_fixu8(b,tag+bytes);
    if ec == 0 then b.uv.u64 = v; ec = savebyte8p(b,bytes+1); end
  end
  return ec;
end

m.wt_u64 = wt_u64;

local wt_str = function(b , s , n)
  local len = #s;
  if n > 0 then
    if len > n then b.e = ec_sequence_length_overflow; return b.e; end
  end
  local ec = wt_u32(b,len);
  if ec > 0 then return ec; end
  if b.wlen + len > b.wcap then b.e = ec_stream_buffer_overflow; return b.e; end
  ffi.copy(b.wbuf + b.wlen , s);
  b.wlen = b.wlen + len;
  return ec;
end

m.wt_str = wt_str;

m.regiest_metatable = function(name,mt)
end

return m;
