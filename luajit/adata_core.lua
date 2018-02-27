--[[ only work with luajit without C adata function , usally work in client program woth luajit script ]]
local tinsert = table.insert;

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

local adata_et_unknow     = 0;
local adata_et_fix_int8   = 1;
local adata_et_fix_uint8  = 2;
local adata_et_fix_int16  = 3;
local adata_et_fix_uint16 = 4;
local adata_et_fix_int32  = 5;
local adata_et_fix_uint32 = 6;
local adata_et_fix_int64  = 7;
local adata_et_fix_uint64 = 8;
local adata_et_int8       = 9;
local adata_et_uint8      =10;
local adata_et_int16      =11;
local adata_et_uint16     =12;
local adata_et_int32      =13;
local adata_et_uint32     =14;
local adata_et_int64      =15;
local adata_et_uint64     =16;
local adata_et_float32    =17;
local adata_et_float64    =18;
local adata_et_string     =19;
local adata_et_list       =20;
local adata_et_map        =21;
local adata_et_type       =22;
local adata_et_buffer     =23;

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

local buf_mt = {};
buf_mt.__index = buf_mt;

local new_buf = function(n)
  local b = {};
  b.uv = ffi.new("union uval_t");
  b.rlen = 0;
  b.wlen = 0;
  b.wcap = n;
  b.wbuf = ffi.new("uint8_t[?]", n);
  b.rcap = 0;
  b.rbuf = nil;
  setmetatable(b,buf_mt);
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
buf_mt.resize = resize_buf;

local clear_read = function(b)
  b.rlen = 0;
end
m.clear_read = clear_read;
buf_mt.clear_read = clear_read;

local clear_write = function(b)
  b.wlen = 0;
end
m.clear_write = clear_write;
buf_mt.clear_write = clear_write;

local clear_buf = function(b)
  b.rlen = 0;
  b.wlen = 0;
end

m.clear_buf = clear_buf;
buf_mt.clear = clear_buf;

local get_wt_data = function(b,dont_clear)
  local str = ffi.string(b.wbuf , b.wlen);
  if dont_clear ~= 1 then clear_buf(b); end
  return str;
end

m.get_write_data = get_wt_data;
buf_mt.get_write_data = get_wt_data;

local set_rd_data = function(b,s)
  b.rcap = #s;
  b.rbuf = s;
  b.rlen = 0;
end
m.set_read_data = set_rd_data;
buf_mt.set_read_data = set_rd_data;

local sbyte = string.byte;

local get_byte = function(b)
  return sbyte(b.rbuf,b.rlen+1)
end

local loadbyte2p = function(b , n)
  if b.rlen + n > b.rcap then
    error("stream buffer overflow");
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p2_1] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p2_2] = get_byte(b);  b.rlen = b.rlen + 1; end
end

local savebyte2p = function(b , n)
  if b.wlen + n > b.wcap then
    error("stream buffer overflow");
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1; end
end

local loadbyte4p = function(b , n)
  if b.rlen + n > b.rcap then
    error("stream buffer overflow");
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p4_1] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p4_2] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 2 then b.uv.b[p4_3] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 3 then b.uv.b[p4_4] = get_byte(b);  b.rlen = b.rlen + 1; end
end

local savebyte4p = function(b , n)
  if b.wlen + n > b.wcap then
    error("stream buffer overflow");
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1; end
  if n > 2 then b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1; end
  if n > 3 then b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1; end
end

local loadbyte8p = function(b , n)
  if b.rlen + n > b.rcap then
    error("stream buffer overflow");
  end
  b.uv.i64 = 0;
  if n > 0 then b.uv.b[p8_1] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 1 then b.uv.b[p8_2] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 2 then b.uv.b[p8_3] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 3 then b.uv.b[p8_4] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 4 then b.uv.b[p8_5] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 5 then b.uv.b[p8_6] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 6 then b.uv.b[p8_7] = get_byte(b);  b.rlen = b.rlen + 1; end
  if n > 7 then b.uv.b[p8_8] = get_byte(b);  b.rlen = b.rlen + 1; end
end

local savebyte8p = function(b , n)
  if b.wlen + n > b.wcap then
    error("stream buffer overflow");
  end
  if n > 0 then b.wbuf[b.wlen] = b.uv.b[p8_1];  b.wlen = b.wlen + 1; end
  if n > 1 then b.wbuf[b.wlen] = b.uv.b[p8_2];  b.wlen = b.wlen + 1; end
  if n > 2 then b.wbuf[b.wlen] = b.uv.b[p8_3];  b.wlen = b.wlen + 1; end
  if n > 3 then b.wbuf[b.wlen] = b.uv.b[p8_4];  b.wlen = b.wlen + 1; end
  if n > 4 then b.wbuf[b.wlen] = b.uv.b[p8_5];  b.wlen = b.wlen + 1; end
  if n > 5 then b.wbuf[b.wlen] = b.uv.b[p8_6];  b.wlen = b.wlen + 1; end
  if n > 6 then b.wbuf[b.wlen] = b.uv.b[p8_7];  b.wlen = b.wlen + 1; end
  if n > 7 then b.wbuf[b.wlen] = b.uv.b[p8_8];  b.wlen = b.wlen + 1; end
end

local rd_fixi8 = function(b)
  if b.rlen + 1 > b.rcap then
    error("stream buffer overflow");
  end
  b.uv.u8 = get_byte(b);
  b.rlen = b.rlen + 1;
  return b.uv.i8;
end

m.rd_fixi8 = rd_fixi8;
buf_mt.rd_fixi8 = rd_fixi8;

local rd_fixu8 = function(b)
  if b.rlen + 1 > b.rcap then
    error("stream buffer overflow");
  end
  local v  = get_byte(b);
  b.rlen = b.rlen + 1;
  return v;
end

m.rd_fixu8 = rd_fixu8;
buf_mt.rd_fixu8 = rd_fixu8;

local wt_fixi8 = function(b , v)
  if b.wlen + 1 > b.wcap then
    error("stream buffer overflow");
  end
  if v < -127 or v > 127 then
    error("error value fix int8");
  end
  b.uv.i8 = v;
  b.wbuf[b.wlen] = b.uv.u8;
  b.wlen = b.wlen + 1;
end

m.wt_fixi8 = wt_fixi8;
buf_mt.wt_fixi8 = wt_fixi8;

local wt_fixu8 = function(b , v)
  if b.wlen + 1 > b.wcap then
    error("stream buffer overflow");
  end
  if v < 0 or v > 256 then
    error("error value fix uint8");
  end
  b.wbuf[b.wlen] = v;
  b.wlen = b.wlen + 1;
end

m.wt_fixu8 = wt_fixu8;
buf_mt.wt_fixu8 = wt_fixu8;

local rd_fixi16 = function(b)
  if b.rlen + 2 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p2_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p2_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.i16;
end

m.rd_fixi16 = rd_fixi16;
buf_mt.rd_fixi16 = rd_fixi16;

local rd_fixu16 = function(b)
  if b.rlen + 2 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p2_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p2_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.u16;
end
 
m.rd_fixu16 = rd_fixu16;
buf_mt.rd_fixu16 = rd_fixu16;

local wt_fixi16 = function(b , v)
  if b.wlen + 2 > b.wcap then
    error("stream buffer overflow");
  end
  if v < -32767 or v > 32767 then
    error("error value fix int16");
  end  
  b.uv.i16 = v;
  b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1;
end

m.wt_fixi16 = wt_fixi16;
buf_mt.wt_fixi16 = wt_fixi16;

local wt_fixu16 = function(b , v)
  if b.wlen + 2 > b.wcap then
    error("stream buffer overflow");
  end
  if v < 0 or v > 65535 then
    error("error value fix uint16");
  end
  b.uv.u16 = v;
  b.wbuf[b.wlen] = b.uv.b[p2_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p2_2];  b.wlen = b.wlen + 1;
end

m.wt_fixu16 = wt_fixu16;
buf_mt.wt_fixu16 = wt_fixu16;

local rd_fixi32 = function(b)
  if b.rlen + 4 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p4_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.i32;
end

m.rd_fixi32 = rd_fixi32;
buf_mt.rd_fixi32 = rd_fixi32;

local rd_fixu32 = function(b)
  if b.rlen + 4 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p4_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.u32;
end

m.rd_fixu32 = rd_fixu32;
buf_mt.rd_fixu32 = rd_fixu32;

local wt_fixi32 = function(b , v)
  if b.wlen + 4 > b.wcap then
    error("stream buffer overflow");
  end
  if v < -2147483647 or v > 2147483647 then
    error("error value fix int32;");
  end
  b.uv.i32 = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
end

m.wt_fixi32 = wt_fixi32;
buf_mt.wt_fixi32 = wt_fixi32;

local wt_fixu32 = function(b , v)
  if b.wlen + 4 > b.wcap then
    error("stream buffer overflow");
  end
  if v < 0 or v > 4294967295 then
    error("error_value_fix_uint32");
  end
  b.uv.u32 = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
end

m.wt_fixu32 = wt_fixu32;
buf_mt.wt_fixu32 = wt_fixu32;

local rd_fixi64 = function(b)
  if b.rlen + 8 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p8_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.i64;
end

m.rd_fixi64 = rd_fixi64;
buf_mt.rd_fixi64 = rd_fixi64;

local rd_fixu64 = function(b)
  if b.rlen + 8 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p8_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.u64;
end

m.rd_fixu64 = rd_fixu64;
buf_mt.rd_fixu64 = rd_fixu64;

local wt_fixi64 = function(b , v)
  if b.wlen + 8 > b.wcap then
    error("stream buffer overflow");
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
end

m.wt_fixi64 = wt_fixi64;
buf_mt.wt_fixi64 = wt_fixi64;

local wt_fixu64 = function(b , v)
  if b.wlen + 8 > b.wcap then
    error("stream buffer overflow");
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
buf_mt.wt_fixu64 = wt_fixu64;

m.szof_fixi8 = function(v)
  return 1;
end

m.szof_fixu8 = function(v)
  return 1;
end

buf_mt.szof_fixu8 = m.szof_fixu8

m.szof_fixi16 = function(v)
  return 2;
end
buf_mt.szof_fixi16 = m.szof_fixi16

m.szof_fixu16 = function(v)
  return 2;
end
buf_mt.szof_fixu16 = m.szof_fixu16

m.szof_fixi32 = function(v)
  return 4;
end
buf_mt.szof_fixi32 = m.szof_fixi32

m.szof_fixu32 = function(v)
  return 4;
end
buf_mt.szof_fixu32 = m.szof_fixu32

m.szof_fixi64 = function(v)
  return 8;
end
buf_mt.szof_fixi64 = m.szof_fixi64

m.szof_fixu64 = function(v)
  return 8;
end
buf_mt.szof_fixu64 = m.szof_fixu64

local rd_f32 = function(b)
  if b.rlen + 4 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p4_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p4_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p4_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.f;
end

m.rd_f32 = rd_f32;
buf_mt.rd_f32 = rd_f32;

local rd_f64 = function(b)
  if b.rlen + 8 > b.rcap then
    error("stream buffer overflow");
  end
  b.i64 = 0;
  b.uv.b[p8_1] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_2] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_3] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_4] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_5] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_6] = get_byte(b);  b.rlen = b.rlen + 1;    
  b.uv.b[p8_7] = get_byte(b);  b.rlen = b.rlen + 1;
  b.uv.b[p8_8] = get_byte(b);  b.rlen = b.rlen + 1;    
  return b.uv.d;
end

m.rd_f64 = rd_f64;
buf_mt.rd_f64 = rd_f64;

local wt_f32 = function(b,v)
  if b.wlen + 4 > b.wcap then
    error("stream buffer overflow");
  end
  b.uv.f = v;
  b.wbuf[b.wlen] = b.uv.b[p4_1];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_2];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_3];  b.wlen = b.wlen + 1;
  b.wbuf[b.wlen] = b.uv.b[p4_4];  b.wlen = b.wlen + 1;
  return 0;
end

m.wt_f32 = wt_f32;
buf_mt.wt_f32 = wt_f32;

local wt_f64 = function(b,v)
  if b.wlen + 8 > b.wcap then
    error("stream buffer overflow");
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
buf_mt.wt_f64 = wt_f64;

local rd_i8 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 1 then
    error("value outof ranger to integer number");
  end
  if tag_as_value == false then
    tag = rd_fixu8(b);
    if nbit == true then
      tag = tag * -1;
    end
  end
  return tag;
end

m.rd_i8 = rd_i8;
buf_mt.rd_i8 = rd_i8;

local rd_u8 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 1 then
    error("value outof ranger to integer number");
  end
  if nbit == true then
    error("negative assign to unsigned integer number");
  end
  if tag_as_value == false then
    tag = rd_fixu8(b);    
  end
  return tag;
end

m.rd_u8 = rd_u8;
buf_mt.rd_u8 = rd_u8;

local rd_i16 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 2 then
    error("value outof ranger to integer number");
  end
  if tag_as_value == false then
    loadbyte2p(b,bytes);
    tag = b.uv.i16    
    if nbit == true then
      tag = tag * -1;
    end
  end
  return tag;
end

m.rd_i16 = rd_i16;
buf_mt.rd_i16 = rd_i16;

local rd_u16 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 2 then
    error("value outof ranger to integer number");
  end
  if nbit == true then
    error("negative assign to unsigned integer number");
  end
  if tag_as_value == false then
    loadbyte2p(b,bytes);
    tag = b.uv.u16    
  end
  return tag;
end

m.rd_u16 = rd_u16;
buf_mt.rd_u16 = rd_u16;

local rd_i32 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 4 then
    error("value outof ranger to integer number");
  end
  if tag_as_value == false then
    loadbyte4p(b,bytes);
    tag = b.uv.i32 
    if nbit == true then
      tag = tag * -1;
    end
  end
  return tag;
end

m.rd_i32 = rd_i32;
buf_mt.rd_i32 = rd_i32;

local rd_u32 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 4 then
    error("value outof ranger to integer number");
  end
  if nbit == true then
    error("negative assign to unsigned integer number");
  end
  if tag_as_value == false then
    loadbyte4p(b,bytes);
    tag = b.uv.u32 
  end
  return tag;
end

m.rd_u32 = rd_u32;
buf_mt.rd_u32 = rd_u32;

local rd_i64 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 8 then
    error("value outof ranger to integer number");
  end
  if tag_as_value == false then
    loadbyte8p(b,bytes);
    tag = b.uv.i64
    if nbit == true then
      tag = tag * -1;
    end
  end
  return tag;
end

m.rd_i64 = rd_i64;
buf_mt.rd_i64 = rd_i64;

local rd_u64 = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , nbit , bytes = decode_tag(tag);
  if bytes > 8 then
    error("value outof ranger to integer number");
  end
  if nbit == true then
    error("negative assign to unsigned integer number");
  end
  if tag_as_value == false then
    loadbyte8p(b,bytes);
    tag = b.uv.u64
  end
  return tag;
end

m.rd_u64 = rd_u64;
buf_mt.rd_u64 = rd_u64;

local ssbu = string.sub;

local rd_str = function(b , n)
  local len = rd_i32(b);
  if n ~= nill and n > 0 then
    if len > n then 
      error("sequence length overflow");
    end
  end
  if b.rlen + len > b.rcap then
    error("stream buffer overflow");
  end
  local str = ssbu(b.rbuf , b.rlen + 1 , b.rlen + len);
  b.rlen = b.rlen + len;
  return  str;
end

m.rd_str = rd_str;
buf_mt.rd_str = rd_str;

local skip_rd = function(b , n)
  if b.rlen + n > b.rcap then
    error("stream buffer overflow");
  end
  b.rlen = b.rlen + n;
end

local skip_rd_fixi8 = function(b)
  return skip_rd(b,1);
end

m.skip_rd_fixi8 = skip_rd_fixi8;
m.skip_rd_fixu8 = skip_rd_fixi8;
buf_mt.skip_rd_fixi8 = skip_rd_fixi8;
buf_mt.skip_rd_fixu8 = skip_rd_fixi8;

local skip_rd_fixi16 = function(b)
  return skip_rd(b,2);
end

m.skip_rd_fixi16 = skip_rd_fixi16;
m.skip_rd_fixu16 = skip_rd_fixi16;
buf_mt.skip_rd_fixi16 = skip_rd_fixi16;
buf_mt.skip_rd_fixu16 = skip_rd_fixi16;

local skip_rd_fixi32 = function(b)
  return skip_rd(b,4);
end

m.skip_rd_fixi32 = skip_rd_fixi32;
m.skip_rd_fixu32 = skip_rd_fixi32;
buf_mt.skip_rd_fixi32 = skip_rd_fixi32;
buf_mt.skip_rd_fixu32 = skip_rd_fixi32;

local skip_rd_fixi64 = function(b)
  return skip_rd(b,8);
end

m.skip_rd_fixi64 = skip_rd_fixi64;
m.skip_rd_fixu64 = skip_rd_fixi64;
buf_mt.skip_rd_fixi64 = skip_rd_fixi64;
buf_mt.skip_rd_fixu64 = skip_rd_fixi64;

m.skip_rd_f32 = skip_rd_fixi32;
m.skip_rd_f64 = skip_rd_fixi64;
buf_mt.skip_rd_f32 = skip_rd_fixi32;
buf_mt.skip_rd_f64 = skip_rd_fixi64;

local skip_rd_ints = function(b)
  local tag = rd_fixu8(b);
  local tag_as_value , _ , bytes = decode_tag(tag);
  if tag_as_value == false then
    skip_rd(b,bytes);
  end
end

m.skip_rd_i8 = skip_rd_ints;
buf_mt.skip_rd_i8 = skip_rd_ints;

m.skip_rd_u8 = skip_rd_ints;
buf_mt.skip_rd_u8 = skip_rd_ints;

m.skip_rd_i16 = skip_rd_ints;
buf_mt.skip_rd_i16 = skip_rd_ints;

m.skip_rd_u16 = skip_rd_ints;
buf_mt.skip_rd_u16 = skip_rd_ints;

m.skip_rd_i32 = skip_rd_ints;
buf_mt.skip_rd_i32 = skip_rd_ints;

m.skip_rd_u32 = skip_rd_ints;
buf_mt.skip_rd_u32 = skip_rd_ints;

m.skip_rd_i64 = skip_rd_ints;
buf_mt.skip_rd_i64 = skip_rd_ints;

m.skip_rd_u64 = skip_rd_ints;
buf_mt.skip_rd_u64 = skip_rd_ints;

local skip_rd_str = function(b , n)
  local len = rd_i32(b);
  if n > 0 then
    if len > n then
      error("sequence length overflow");
    end
  end
  if b.rlen + len > b.rcap then
    error("stream buffer overflow");
  end
  b.rlen = b.rlen + len;
end

m.skip_rd_str = skip_rd_str;
buf_mt.skip_rd_str = skip_rd_str;

local wt_i8 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
  else
    if v > 127 then
      error("error value int8");
    end
    wt_fixu8(b,tag);
    wt_fixu8(b,v);
  end
end

m.wt_i8 = wt_i8;
buf_mt.wt_i8 = wt_i8;

local wt_u8 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
  else
    if v > 255 then
      error("error value uint8");
    end
    wt_fixu8(b,tag);
    ec = wt_fixu8(b,v);
  end
end

m.wt_u8 = wt_u8;
buf_mt.wt_u8 = wt_u8;

local wt_i16 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x8000 then
      bytes = 1;
    else
      error("error value int16");
    end
    wt_fixu8(b,tag+bytes);
    b.uv.i16 = v;
    savebyte2p(b,bytes+1);
  end
end

m.wt_i16 = wt_i16;
buf_mt.wt_i16 = wt_i16;

local wt_u16 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
  else
    local bytes = 0;
    if v < 0x100 then
    elseif v < 0x10000 then
      bytes = 1;
    else
      error("error value uint16");
    end
    wt_fixu8(b,tag+bytes);
    b.uv.u16 = v;
    savebyte2p(b,bytes+1);
  end
end

m.wt_u16 = wt_u16;
buf_mt.wt_u16 = wt_u16;

local wt_i32 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)
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
      error("error value int32");
    end
    wt_fixu8(b,tag+bytes);
    b.uv.i32 = v;
    savebyte4p(b,bytes+1);
  end
end

m.wt_i32 = wt_i32;
buf_mt.wt_i32 = wt_i32;

local wt_u32 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
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
      error("error value uint32");
    end
    wt_fixu8(b,tag+bytes);
    b.uv.u32 = v;
    savebyte4p(b,bytes+1);
  end
end

m.wt_u32 = wt_u32;
buf_mt.wt_u32 = wt_u32;

local wt_i64 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
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
      error("error value int64");
    end
    wt_fixu8(b,tag+bytes);
    b.uv.i64 = v;
    savebyte8p(b,bytes+1);
  end
end

m.wt_i64 = wt_i64;
buf_mt.wt_i64 = wt_i64;

local wt_u64 = function(b , n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == true then
    wt_fixu8(b,v)    
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
    wt_fixu8(b,tag+bytes);
    b.uv.u64 = v;
    savebyte8p(b,bytes+1);
  end
end

m.wt_u64 = wt_u64;
buf_mt.wt_u64 = wt_u64;

local szof_i64 = function(n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == false then
    if v < 0x100 then
      return 2;
    elseif v < 0x10000 then
      return 3;
    elseif v < 0x1000000 then
      return 4;
    elseif v < 0x100000000 then
      return 5;
    elseif v < 0x10000000000 then
      return 6;
    elseif v < 0x1000000000000LL then
      return 7;
    elseif v < 0x100000000000000LL then
      return 8;
    elseif v < 0x8000000000000000LL then
      return 9;
    end
  end
  return 1;
end

local szof_u64 = function(n)
  local tag_as_value , tag , v = encode_tag(n);
  if tag_as_value == false then
    local bytes = 0;
    if v < 0x100 then
      return 2;
    elseif v < 0x10000 then
      return 3;
    elseif v < 0x1000000 then
      return 4;
    elseif v < 0x100000000 then
      return 5;
    elseif v < 0x10000000000 then
      return 6;
    elseif v < 0x1000000000000ULL then
      return 7;
    elseif v < 0x100000000000000ULL then
      return 8;
    else
      return 9;
    end
  end
  return 1;
end

m.szof_i8 = szof_i64;
m.szof_u8 = szof_u64;
m.szof_i16 = szof_i64;
m.szof_u16 = szof_u64;
m.szof_i32 = szof_i64;
m.szof_u32 = szof_u64;
m.szof_i64 = szof_i64;
m.szof_u64 = szof_u64;

buf_mt.szof_i8 = szof_i64;
buf_mt.szof_u8 = szof_u64;
buf_mt.szof_i16 = szof_i64;
buf_mt.szof_u16 = szof_u64;
buf_mt.szof_i32 = szof_i64;
buf_mt.szof_u32 = szof_u64;
buf_mt.szof_i64 = szof_i64;
buf_mt.szof_u64 = szof_u64;


m.szof_f32 = function(v)
  return 4;
end
buf_mt.szof_f32 = m.szof_f32

m.szof_f64 = function(v)
  return 8;
end
buf_mt.szof_f64 = m.szof_f64

local wt_str = function(b , s , n)
  local len = #s;
  if n ~= nil and n > 0 then
    if len > n then
      error("sequence ength overflow");
    end
  end
  wt_i32(b,len);
  if b.wlen + len > b.wcap then
    error("stream buffer overflow");
  end
  ffi.copy(b.wbuf + b.wlen , s);
  b.wlen = b.wlen + len;
end

m.szof_str = function( s )
  local len = #s;
  len = len + szof_i64(len);
end
buf_mt.szof_str = m.szof_str

m.wt_str = wt_str;
buf_mt.wt_str = wt_str;

local make_zbuf_with_file = function(filename)
  local fp = io.open(filename,"rb");
  local buff = fp:read('*a');
  fp:close();
  local zbuf = new_buf(1);
  set_rd_data(zbuf,buff);
  return zbuf
end

local register_str_pool = function(s,pool,idx)
  local idx_len = #idx;
  local sidx = pool[s];
  if sidx == nil then
    idx_len = idx_len + 1;
    pool[s] = idx_len;
    idx[idx_len] = s;
    sidx = idx_len
  end
  return sidx;
end

local tablen = function(tab)
	if tab == nil then return 0 end
	local count = 0
  for _ in pairs(tab) do count = count + 1 end
  return count
end

local def_pos_type =1;
local def_pos_size  = 2;
local def_pos_typename =3;
local def_pos_type_def = 4;
local def_pos_name =5;
local def_pos_del =6;
local def_pos_params = 7;

local rehash = function( ns_str_pool_idx , idx )
  return ns_str_pool_idx[idx+1];
end

local decode_default_value = function(buf,t,tname)
  local v = nil;
  if t >= adata_et_fix_int8 and t <= adata_et_int64 then
    v = rd_i64(buf);
  elseif t == adata_et_fix_uint64 then
    v = rd_u64(buf);
  elseif t == adata_et_float32 then
    v = rd_f32(buf);
  elseif t == adata_et_float64 then
    v = rd_f64(buf);
  elseif t == adata_et_string then
    v = "";
  elseif t == adata_et_type then
    v = tname;
  elseif t == adata_et_buffer then
    v = "";
  end
  return v;
end

local load_namespace = function(buf,str_pool,str_idx,types,mts,mt_list)
  local ns_name = rd_str(buf,0);
  local str_pool_count = rd_i32(buf);
  local ns_str_pool_idx = {};
  for i = 1 , str_pool_count do
    local str = rd_str(buf,0);
    ns_str_pool_idx[i] = register_str_pool(str,str_pool,str_idx);
  end
  local type_count = rd_i32(buf);
  local ns_types = {};
  for i = 1 , type_count do
    local type_name = nil;
    local field_list = {};
    local construct_list = {};
    local mt = {};
    local type_members = {};
    
    local member_count = rd_i32(buf);
    local param_type_count = rd_i32(buf);
    
    local type_name_sid = rd_i32(buf);
    type_name_sid = rehash(ns_str_pool_idx,type_name_sid);
    type_name = str_idx[type_name_sid];
    local type_def = {type_name , type_members};
    local process_count = 1;
    for m = 1, member_count do
      local member_name_sid = rd_i32(buf);
      member_name_sid = rehash(ns_str_pool_idx,member_name_sid);     
      local member_name = str_idx[member_name_sid];     
      local member_type = rd_i32(buf);
      local member_type_def = nil;
      local member_type_name = nil;
      
      if member_type == adata_et_type then
        local member_typename_sid = rd_i32(buf);
        member_typename_sid = rehash(ns_str_pool_idx,member_typename_sid);
        member_typename = str_idx[member_typename_sid];
        local namespace_idx = rd_i32(buf);
        
        if namespace_idx == -1 then
          member_type_def = types[member_typename];
        else
          member_type_def = ns_types[namespace_idx+1];
        end        
      end
      
      local member_del = rd_i32(buf);
      if member_del == 0 then
        field_list[process_count] = member_name;
        local construct_value = decode_default_value(buf,member_type,member_type_def);
        construct_list[process_count] = construct_value;
        process_count = process_count + 1;
      end
      
      local member_size = rd_i32(buf);
      
      member_params = {};
      local param_count = rd_i32(buf);
      for p = 1,param_count do
        local p_type = rd_i32(buf);
        local ptype_type = p_type;
        local ptype_typename = nil;
        local ptype_typedef = nil;
        if p_type == adata_et_type then
          local ptype_typename_sid = rd_i32(buf);
          ptype_typename_sid = rehash(ns_str_pool_idx,ptype_typename_sid);
          ptype_typname = str_pool[ptype_typename_sid];
          local namespace_idx = rd_i32(buf);
          if namespace_idx == -1 then
            ptype_type_def = types[ptype.typename];
          else
            ptype_type_def = ns_types[namespace_idx+1];
          end            
        end
        local ptype_size = rd_i32(buf);
        local ptype = {ptype_type,ptype_size,ptype_typename,ptype_typedef};
        tinsert(member_params,ptype);
      end
      local member = { member_type , member_size , member_type_name , member_type_def , member_name , member_del ,  member_params};

      tinsert(type_members , member);
    end
    
    local obj_def = { type_def , type_name , field_list , construct_list , mt};
    types[ns_name .. '.' .. type_name] = type_def;
    ns_types[i] = obj_def;
  end
  return {ns_name,ns_types};
end

m.load = function(adt_file,str_pool,str_idx,types,mts,mt_list)
  local buf = make_zbuf_with_file(adt_file);
  local count = rd_i32(buf);
  local nss = {};
  for i = 1, count do
    local ns = load_namespace(buf,str_pool,str_idx,types,mts,mt_list);
    tinsert(nss,ns);
  end
  return nss;
end

local skip_rd_type = function(str_idx , mt_list , buf , obj_type , o)
  local offset = get_rd_len(buf);
  local read_tag = rd_i64(buf);
  local len_tag = rd_i32(buf);
  if len_tag >= 0 then
    local read_len = get_rd_len(buf) - offset;
    if len_tag > read_len then skip_rd_len(buf, len_tag - read_len); end;
  end    
end

m.skip_read = skip_rd_type;

local skip_rd_value = function(buf , def , o)
  if ty >= adata_et_int8 and ty <= adata_et_uint64 then
    skip_rd_ints(buf)
  elseif ty == adata_et_fix_int8 or ty == adata_et_fix_uint8 then
    skip_rd_fixi8(buf)
  elseif ty == adata_et_fix_int16 or ty == adata_et_fix_uint16 then
    skip_rd_fixi16(buf)
  elseif ty == adata_et_fix_int32 or ty == adata_et_fix_uint32 or ty ==adata_et_float64  then
    skip_rd_fixi32(buf)
  elseif ty == adata_et_fix_int64 or ty == adata_et_fix_uint64 or ty == adata_et_float64 then
    skip_rd_fixi64(buf)
  elseif ty == adata_et_string then
    skip_rd_str(buf)
  elseif ty == adata_et_buffer then
    skip_rd_str(buf)
  else
   skip_rd_type(0,0,buf)
  end
end

local skip_rd_member = function( buf , mb , o)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local param = mb[def_pos_params][1];
    for i = 1 , count do
      local val = construct_value(param);
      skip_rd_value(buf,param , val);
    end
  elseif ty == adata_et_map then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    for i = 1 , count do
      skip_rd_value(buf,kparam,kval);
      skip_rd_value(buf,vparam,vval);
    end
  else
    skip_rd_value(buf,mb,o);
  end
end

local read_type;

local read_value = function(buf , def , o)
  local ty = def[def_pos_type];
  if ty == adata_et_fix_int8 then
    return rd_fixi8(buf);
  elseif ty == adata_et_fix_uint8 then
    return rd_fixu8(buf);
  elseif ty == adata_et_fix_int16 then
    return rd_fixi16(buf);
  elseif ty == adata_et_fix_uint16 then
    return rd_fixu16(buf);
  elseif ty == adata_et_fix_int32 then
    return rd_fixi32(buf);
  elseif ty == adata_et_fix_uint32 then
    return rd_fixu32(buf);
  elseif ty == adata_et_fix_int64 then
    return rd_fixi64(buf);
  elseif ty == adata_et_fix_uint64 then
    return rd_fixu64(buf);
  elseif ty == adata_et_int8 then
    return rd_i8(buf);
  elseif ty == adata_et_uint8 then
    return rd_u8(buf);
  elseif ty == adata_et_int16 then
    return rd_i16(buf);
  elseif ty == adata_et_uint16 then
    return rd_u16(buf);
  elseif ty == adata_et_int32 then
    return rd_i32(buf);
  elseif ty == adata_et_uint32 then
    return rd_u32(buf);
  elseif ty == adata_et_int64 then
    return rd_i64(buf);
  elseif ty == adata_et_uint64 then
    return rd_u64(buf);
  elseif ty == adata_et_float32 then
    return rd_f32(buf);
  elseif ty == adata_et_float64 then
    return rd_f64(buf);
  elseif ty == adata_et_string then
    return rd_str(buf,def[def_pos_size]);
  elseif ty == adata_et_type then
    return read_type(buf,def[def_pos_type_def],o)
  elseif ty == adata_et_buffer then
    return rd_str(buf,def[def_pos_size]);
  end
end

local construct_value = function(def)
  if def[def_pos_type] == adata_et_type then return def.constructor(); end;
end

local read_member = function( buf , mb , o)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local param = mb[def_pos_params][1];
    for i = 1 , count do
      local val = construct_value(param);
      val = read_value(buf,param , val);
      o[i] = val;
    end
    return o;
  elseif ty == adata_et_map then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    for i = 1 , count do
      local kval = construct_value(kparam);
      kval = read_value(buf,kparam,kval);
      local vval = construct_value(vparam);
      vval = read_value(buf,vparam,vval);
      o[kval] = vval;
    end
    return o;
  else
    return read_value(buf,mb,o);
  end
end

local masks = {
  1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 , 256 , 512 , 1024 , 2048 , 4096 , 8192 ,
  16384 , 32768, 65536 , 131072 , 262144 , 524288 , 1048576 , 2097152 , 4194304 ,
  8388608 , 16777216 , 33554432 , 67108864 , 134217728 , 268435456,536870912 ,
  1073741824 , 2147483648 , 4294967296 , 8589934592 , 17179869184 , 34359738368 ,
  68719476736 , 137438953472 , 274877906944 , 549755813888 , 1099511627776 ,
  2199023255552 , 4398046511104 , 8796093022208 , 17592186044416 , 35184372088832 ,
  70368744177664 , 140737488355328ll , 281474976710656ll , 562949953421312ll ,
  1125899906842624ll , 2251799813685248ll , 4503599627370496ll ,
  9007199254740992ll , 18014398509481984ll , 36028797018963968ll ,
  72057594037927936ll , 144115188075855872ll , 288230376151711744ll ,
  576460752303423488ll , 1152921504606846976ll , 2305843009213693952ll ,
  4611686018427387904ll , 9223372036854775808ll , 0
};

local get_rd_len = function(buf)
  return buf.rlen;
end
buf_mt.read_len = get_rd_len;

local get_wt_len = function(buf)
  return buf.wlen;
end
buf_mt.write_len = get_wt_len;

local skip_rd_len = function(b,n)
  b.rlen = b.rlen + n;
end

read_type = function(buf,type_def,o)
  local offset = get_rd_len(buf);
  local read_tag = rd_i64(buf);
  local len_tag = rd_i32(buf);
  
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if read_tag % masks[i+1] >= masks[i] then
      if mb[def_pos_del] > 0 then
        skip_rd_member();
      else
        local mname = mb[def_pos_name];
        o[mname] = read_member(buf,mb,o[mname]);        
      end;     
    end  
  end
  
  if len_tag >= 0 then
    local read_len = get_rd_len(buf) - offset;
    if len_tag > read_len then
      skip_rd_len(buf, len_tag - read_len);
    end;
  end
  return o;
end

m.read = function(str_idx , mt_list , buf , type_def , o)
  return read_type(buf,type_def,o);
end

local sizeof_fixs ={1,1,2,2,4,4,8,8};

local sizeof_type;

local sizeof_value = function(def , o , ctx)
  local ty = def[def_pos_type];
  if ty >= adata_et_fix_int8 and ty <= adata_et_fix_uint64 then
    return sizeof_fixs[ty];
  elseif ty >= adata_et_int8 and ty <= adata_et_int64 then
    return szof_i64(o);
  elseif ty == adata_et_uint64 then
    return szof_u64(o);
  elseif ty == adata_et_float32 then
    return 4;
  elseif ty == adata_et_float64 then
    return 8;
  elseif ty == adata_et_string then
    local len = #o;
    return szof_i64(len) + len;
  elseif ty == adata_et_type then
    return sizeof_type(def[def_pos_type_def],o,ctx)
  elseif ty == adata_et_buffer then
    local len = #o;
    return szof_i64(len) + len;
  end
end

local sizeof_member = function(mb , o , ctx)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local len = #o;
    if len == 0 then return true,0; end;
    local sz = szof_i64(len);
    local param = mb[def_pos_params][1];
    for i = 1 , len do 
      sz = sz + sizeof_value(param,o[i],ctx);
    end;
    return false , sz;
  elseif ty == adata_et_map then
    local len = tablen(o);
    if len == 0 then return true,0; end;
    local sz = szof_i64(len);
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    for k,v in pairs(o) do
      sz = sz + sizeof_value(kparam,k,ctx);
      sz = sz + sizeof_value(vparam,v,ctx);
    end
    return false,sz;
  else
    return false , sizeof_value(mb,o,ctx);
  end
end

sizeof_type = function(type_def,o,ctx) 
  local mask = 0;
  local sz = 0;
  local idx = 0;
  if ctx ~= nill then
    idx = ctx[1];
    idx = idx + 1;
    ctx[1] = idx;
    info  = {};
    tinsert(ctx,{});
  end;
  
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if mb[def_pos_del] == 0 then
      local name = mb[def_pos_name];
      local skip , msz = sizeof_member(mb,o[name],ctx);
      if skip == false then
        mask = mask + masks[i];
        sz = sz + msz;
      end
    end
  end
  local len_tag = sz + szof_i64(mask);
  len_tag = len_tag + szof_i64(len_tag);
  if ctx ~= nill then
    ctx[idx] = {mask , len_tag};
  end;
  return len_tag;
end

m.size_of = function(str_idx , mt_list , type_def , o)
  return sizeof_type(type_def,o);
end

local pop_ctx = function(ctx)
  local widx = ctx[1];
  widx = widx + 1;
  ctx[1] = widx;
  return ctx[widx];
end

local write_type;

local write_value = function(buf , def , o , ctx)
  local ty = def[def_pos_type];
  if ty == adata_et_fix_int8 then
    return wt_fixi8(buf,o);
  elseif ty == adata_et_fix_uint8 then
    return wt_fixu8(buf,o);
  elseif ty == adata_et_fix_int16 then
    return wt_fixi16(buf,o);
  elseif ty == adata_et_fix_uint16 then
    return wt_fixu16(buf,o);
  elseif ty == adata_et_fix_int32 then
    return wt_fixi32(buf,o);
  elseif ty == adata_et_fix_uint32 then
    return wt_fixu32(buf,o);
  elseif ty == adata_et_fix_int64 then
    return wt_fixi64(buf,o);
  elseif ty == adata_et_fix_uint64 then
    return wt_fixu64(buf,o);
  elseif ty == adata_et_int8 then
    return wt_i8(buf,o);
  elseif ty == adata_et_uint8 then
    return wt_u8(buf,o);
  elseif ty == adata_et_int16 then
    return wt_i16(buf,o);
  elseif ty == adata_et_uint16 then
    return wt_u16(buf,o);
  elseif ty == adata_et_int32 then
    return wt_i32(buf,o);
  elseif ty == adata_et_uint32 then
    return wt_u32(buf,o);
  elseif ty == adata_et_int64 then
    return wt_i64(buf,o);
  elseif ty == adata_et_uint64 then
    return wt_u64(buf,o);
  elseif ty == adata_et_float32 then
    return wt_f32(buf,o);
  elseif ty == adata_et_float64 then
    return wt_f64(buf,o);
  elseif ty == adata_et_string then
    return wt_str(buf,o,def[def_pos_size]);
  elseif ty == adata_et_type then
    return write_type(buf,def[def_pos_type_def],o,ctx)
  elseif ty == adata_et_buffer then
    return wt_str(buf,o,def[def_pos_size]);
  end
end

local write_member = function( buf , mb , o , ctx)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local count = #o;
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    wt_i32(buf,count);
    local param = mb[def_pos_params][1];
    for i = 1 , count do
      local val = o[i];
      write_value(buf,param , val , ctx);
    end
  elseif ty == adata_et_map then
    local count = tablen(o);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    wt_i32(buf,count);
    for k,v in pairs(o) do
      write_value(buf,kparam,k,ctx);
      write_value(buf,vparam,v,ctx);
    end
  else
    return write_value(buf,mb,o,ctx);
  end
end

write_type = function(buf,type_def,o,ctx)
  local info = pop_ctx(ctx);
  local write_tag = info[1];
  local len_tag = info[2];
  wt_i64(buf,write_tag);
  wt_i32(buf,len_tag);
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if write_tag % masks[i+1] >= masks[i] then
      write_member(buf,mb,o[mb[def_pos_name]],ctx);
    end  
  end
end

m.write = function(str_idx , mt_list , buf , type_def , o)
  local ctx = {1};
  local sz = sizeof_type(type_def,o,ctx);
  ctx[1] = 1;
  return write_type(buf,type_def,o,ctx);
end

local raw_read_type;

local raw_read_value = function(buf , def , o)
  local ty = def[def_pos_type];
  if ty == adata_et_fix_int8 then
    return rd_fixi8(buf);
  elseif ty == adata_et_fix_uint8 then
    return rd_fixu8(buf);
  elseif ty == adata_et_fix_int16 then
    return rd_fixi16(buf);
  elseif ty == adata_et_fix_uint16 then
    return rd_fixu16(buf);
  elseif ty == adata_et_fix_int32 then
    return rd_fixi32(buf);
  elseif ty == adata_et_fix_uint32 then
    return rd_fixu32(buf);
  elseif ty == adata_et_fix_int64 then
    return rd_fixi64(buf);
  elseif ty == adata_et_fix_uint64 then
    return rd_fixu64(buf);
  elseif ty == adata_et_int8 then
    return rd_i8(buf);
  elseif ty == adata_et_uint8 then
    return rd_u8(buf);
  elseif ty == adata_et_int16 then
    return rd_i16(buf);
  elseif ty == adata_et_uint16 then
    return rd_u16(buf);
  elseif ty == adata_et_int32 then
    return rd_i32(buf);
  elseif ty == adata_et_uint32 then
    return rd_u32(buf);
  elseif ty == adata_et_int64 then
    return rd_i64(buf);
  elseif ty == adata_et_uint64 then
    return rd_u64(buf);
  elseif ty == adata_et_float32 then
    return rd_f32(buf);
  elseif ty == adata_et_float64 then
    return rd_f64(buf);
  elseif ty == adata_et_string then
    return rd_str(buf,def[def_pos_size]);
  elseif ty == adata_et_type then
    return raw_read_type(buf,def[def_pos_type_def],o)
  elseif ty == adata_et_buffer then
    return rd_str(buf,def[def_pos_size]);
  end
end

local raw_read_member = function( buf , mb , o)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local param = mb[def_pos_params][1];
    for i = 1 , count do
      local val = construct_value(param);
      val = raw_read_value(buf,param , val);
      o[i] = val;
    end
    return o;
  elseif ty == adata_et_map then
    local count = rd_i32(buf);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    for i = 1 , count do
      local kval = construct_value(kparam);
      kval = raw_read_value(buf,kparam,kval);
      local vval = construct_value(vparam);
      vval = raw_read_value(buf,vparam,vval);
      o[kval] = vval;
    end
    return o;
  else
    return raw_read_value(buf,mb,o);
  end
end

raw_read_type = function(buf,type_def,o) 
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if mb[def_pos_del] == 0 then
      local mname = mb[def_pos_name];
      o[mname] = read_member(buf,mb,o[mname]);        
    end  
  end
  return o;
end

m.raw_read = function(str_idx , mt_list , buf , type_def , o)
  return raw_read_type(buf,type_def,o);
end

local raw_sizeof_type;

local raw_sizeof_value = function(def , o)
  local ty = def[def_pos_type];
  if ty >= adata_et_fix_int8 and ty <= adata_et_fix_uint64 then
    return sizeof_fixs[ty];
  elseif ty >= adata_et_int8 and ty <= adata_et_int64 then
    return szof_i64(o);
  elseif ty == adata_et_uint64 then
    return szof_u64(o);
  elseif ty == adata_et_float32 then
    return 4;
  elseif ty == adata_et_float64 then
    return 8;
  elseif ty == adata_et_string then
    local len = #o;
    return szof_i64(len) + len;
  elseif ty == adata_et_type then
    return raw_sizeof_type(def[def_pos_type_def],o)
  elseif ty == adata_et_buffer then
    local len = #o;
    return szof_i64(len) + len;
  end
end

local raw_sizeof_member = function(mb , o)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local len = #o;
    if len == 0 then return true,0; end;
    local sz = szof_u32(len);
    local param = mb[def_pos_params][1];
    for i = 1 , len do 
      sz = sz + raw_sizeof_value(param,o[i]);
    end;
    return sz;
  elseif ty == adata_et_map then
    local len = tablen(o);
    if len == 0 then return true,0; end;
    local sz = szof_u32(len);
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    for k,v in pairs(o) do
      sz = sz + raw_sizeof_value(kparam,k);
      sz = sz + raw_sizeof_value(vparam,v);
    end
    return sz;
  else
    return raw_sizeof_value(mb,o);
  end
end

raw_sizeof_type = function(type_def,o) 
  local sz = 0;
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if mb[def_pos_del] == 0 then
      local name = mb[def_pos_name];
      sz = sz + sizeof_member(mb,o[name],ctx);
    end
  end
  return sz;
end

m.raw_size_of = function(str_idx , mt_list , type_def , o)
  return raw_sizeof_type(type_def,o);
end

local raw_write_type;

local raw_write_value = function(buf , def , o)
  local ty = def[def_pos_type];
  if ty == adata_et_fix_int8 then
    return wt_fixi8(buf,o);
  elseif ty == adata_et_fix_uint8 then
    return wt_fixu8(buf,o);
  elseif ty == adata_et_fix_int16 then
    return wt_fixi16(buf,o);
  elseif ty == adata_et_fix_uint16 then
    return wt_fixu16(buf,o);
  elseif ty == adata_et_fix_int32 then
    return wt_fixi32(buf,o);
  elseif ty == adata_et_fix_uint32 then
    return wt_fixu32(buf,o);
  elseif ty == adata_et_fix_int64 then
    return wt_fixi64(buf,o);
  elseif ty == adata_et_fix_uint64 then
    return wt_fixu64(buf,o);
  elseif ty == adata_et_int8 then
    return wt_i8(buf,o);
  elseif ty == adata_et_uint8 then
    return wt_u8(buf,o);
  elseif ty == adata_et_int16 then
    return wt_i16(buf,o);
  elseif ty == adata_et_uint16 then
    return wt_u16(buf,o);
  elseif ty == adata_et_int32 then
    return wt_i32(buf,o);
  elseif ty == adata_et_uint32 then
    return wt_u32(buf,o);
  elseif ty == adata_et_int64 then
    return wt_i64(buf,o);
  elseif ty == adata_et_uint64 then
    return wt_u64(buf,o);
  elseif ty == adata_et_float32 then
    return wt_f32(buf,o);
  elseif ty == adata_et_float64 then
    return wt_f64(buf,o);
  elseif ty == adata_et_string then
    return wt_str(buf,o,def[def_pos_size]);
  elseif ty == adata_et_type then
    return raw_write_type(buf,def[def_pos_type_def],o)
  elseif ty == adata_et_buffer then
    return wt_str(buf,o,def[def_pos_size]);
  end
end

local raw_write_member = function( buf , mb , o)
  local ty = mb[def_pos_type];
  if ty == adata_et_list then
    local count = #o;
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    wt_i32(buf,count);
    local param = mb[def_pos_params][1];
    for i = 1 , count do
      local val = o[i];
      raw_write_value(buf,param , val , ctx);
    end
  elseif ty == adata_et_map then
    local count = tablen(o);
    local size = mb[def_pos_size];
    if size > 0 and count > size then
      error("sequence length overflow");
    end;
    local kparam = mb[def_pos_params][1];
    local vparam = mb[def_pos_params][2];
    wt_i32(buf,count);
    for k,v in pairs(o) do
      raw_write_value(buf,kparam,k,ctx);
      raw_write_value(buf,vparam,v,ctx);
    end
  else
    return raw_write_value(buf,mb,o,ctx);
  end
end

raw_write_type = function(buf,type_def,o)
  local members = type_def[2];
  for i = 1 , #members do
    local mb = members[i];
    if mb[def_pos_del] == 0 then
      raw_write_member(buf,mb,o[mb[def_pos_name]]);
    end  
  end
end

m.raw_write = function(str_idx , mt_list , buf , type_def , o)
  return raw_write_type(buf,type_def,o);
end

return m;
