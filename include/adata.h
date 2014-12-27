// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ADATA_H_HEADER_
#define ADATA_H_HEADER_

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdint.h>

#ifndef ADATA_INLINE
# ifdef __cplusplus
#  ifdef _MSC_VER
#   pragma inline_recursion(on)
#   define ADATA_INLINE __forceinline
#  elif defined(__GNUC__)
#   define ADATA_INLINE inline __attribute__((always_inline))
#  else
#   define ADATA_INLINE inline
#  endif
# else
#  define ADATA_INLINE
# endif
#endif

//define endian check macro

#ifdef _WIN32
#else
#include <arpa/inet.h>  /* __BYTE_ORDER */
#endif

#	if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#		if __BYTE_ORDER == __LITTLE_ENDIAN
#			define __LITTLE_ENDIAN__
#		elif __BYTE_ORDER == __BIG_ENDIAN
#			define __BIG_ENDIAN__
#		elif _WIN32
#			define __LITTLE_ENDIAN__
#		endif
#	endif

#if defined(__LITTLE_ENDIAN__)
#define ADATA_LEPOS2_0 0
#define ADATA_LEPOS2_1 1
#define ADATA_LEPOS4_0 0
#define ADATA_LEPOS4_1 1
#define ADATA_LEPOS4_2 2
#define ADATA_LEPOS4_3 3
#define ADATA_LEPOS8_0 0
#define ADATA_LEPOS8_1 1
#define ADATA_LEPOS8_2 2
#define ADATA_LEPOS8_3 3
#define ADATA_LEPOS8_4 4
#define ADATA_LEPOS8_5 5
#define ADATA_LEPOS8_6 6
#define ADATA_LEPOS8_7 7
#else
#define ADATA_LEPOS2_0 1
#define ADATA_LEPOS2_1 0
#define ADATA_LEPOS4_0 3
#define ADATA_LEPOS4_1 2
#define ADATA_LEPOS4_2 1
#define ADATA_LEPOS4_3 0
#define ADATA_LEPOS8_0 7
#define ADATA_LEPOS8_1 6
#define ADATA_LEPOS8_2 5
#define ADATA_LEPOS8_3 4
#define ADATA_LEPOS8_4 3
#define ADATA_LEPOS8_5 2
#define ADATA_LEPOS8_6 1
#define ADATA_LEPOS8_7 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

  enum
  {
    adata_const_tag_as_value = 0x7f,
    adata_const_tag_as_type,
    adata_const_interger_byte_msak = 0x7,
    adata_const_negative_bit_value = 0x20,
    adata_const_max_trace_info_level = 64
  };

enum
{
  adata_ec_success = 0,
  adata_ec_negative_assign_to_unsigned_integer_number,
  adata_ec_value_outof_ranger_to_integer_number,
  adata_ec_sequence_length_overflow,
  adata_ec_stream_buffer_overflow,
  adata_ec_number_of_element_not_macth,
  adata_ec_undefined_member_protocol_not_compatible,
  adata_ec_value_not_match_number,
  adata_ec_error_value_fix_int8,
  adata_ec_error_value_fix_uint8,
  adata_ec_error_value_fix_int16,
  adata_ec_error_value_fix_uint16,
  adata_ec_error_value_fix_int32,
  adata_ec_error_value_fix_uint32,
  adata_ec_error_value_fix_int64,
  adata_ec_error_value_fix_uint64,
  adata_ec_error_value_int8,
  adata_ec_error_value_uint8,
  adata_ec_error_value_int16,
  adata_ec_error_value_uint16,
  adata_ec_error_value_int32,
  adata_ec_error_value_uint32,
  adata_ec_error_value_int64,
  adata_ec_error_value_uint64,
  adata_ec_error_value_float32,
  adata_ec_error_value_double64,
  adata_ec_error_value_string,
  adata_ec_error_value_list,
  adata_ec_error_value_map,
};

typedef struct adata_trace_info
{
  char *	m_tag_name;
  int			m_sub_index;
}adata_trace_info;

static ADATA_INLINE void adata_trace_info_init(adata_trace_info * info)
{
  info->m_tag_name = 0;
  info->m_sub_index = 0;
}

typedef struct adata_stream_buffer
{
  size_t  m_read_len;
  size_t  m_write_len;
	char*   m_data;
	size_t  m_len;
  adata_trace_info  m_trace_infos[adata_const_max_trace_info_level];
  size_t  m_trace_info_count;
  size_t  m_write_bytes;
  int     m_error_code;
}adata_stream_buffer;

static ADATA_INLINE void adata_stream_buffer_init(adata_stream_buffer * sbuf)
{
  sbuf->m_read_len = 0;
  sbuf->m_write_len = 0;
  sbuf->m_data = NULL;
  sbuf->m_len = 0;
  sbuf->m_trace_info_count = 0;
  sbuf->m_write_bytes = 0;
  sbuf->m_error_code = adata_ec_success;
}

static ADATA_INLINE char * adata_stream_buffer_error_msg(int type)
{
  switch(type)
  {
  case adata_ec_success:  return "";
  case adata_ec_negative_assign_to_unsigned_integer_number: return "assign negative value to unsigned.";
  case adata_ec_value_outof_ranger_to_integer_number: return "integer value out of range.";
  case adata_ec_sequence_length_overflow: return "size out of range.";
  case adata_ec_stream_buffer_overflow: return "buffer overflow.";
  case adata_ec_number_of_element_not_macth: return "size too large.";
  case adata_ec_undefined_member_protocol_not_compatible: return "undefined member, protocol not compatible.";
  case adata_ec_value_not_match_number: return "value not match number.";
  case adata_ec_error_value_fix_int8: return "value is not fixed int8";
  case adata_ec_error_value_fix_uint8: return "value is not fixed uint8";
  case adata_ec_error_value_fix_int16: return "value is not fixed int16";
  case adata_ec_error_value_fix_uint16: return "value is not fixed uint16";
  case adata_ec_error_value_fix_int32: return "value is not fixed int32";
  case adata_ec_error_value_fix_uint32: return "value is not fixed uint32";
  case adata_ec_error_value_fix_int64: return "value is not fixed int64";
  case adata_ec_error_value_fix_uint64: return "value is not fixed uint64";
  case adata_ec_error_value_int8: return "value is not int8";
  case adata_ec_error_value_uint8: return "value is not uint8";
  case adata_ec_error_value_int16: return "value is not int16";
  case adata_ec_error_value_uint16: return "value is not uint16";
  case adata_ec_error_value_int32: return "value is not int32";
  case adata_ec_error_value_uint32: return "value is not uint32";
  case adata_ec_error_value_int64: return "value is not int64";
  case adata_ec_error_value_uint64: return "value is not uint64";
  case adata_ec_error_value_float32: return "value is not float32";
  case adata_ec_error_value_double64: return "value is not double64";
  case adata_ec_error_value_string: return "value is not string";
  case adata_ec_error_value_list: return "value is not list";
  case adata_ec_error_value_map: return "value is not map";
  }
  return "";
}

static ADATA_INLINE void adata_stream_buffer_trace_error(adata_stream_buffer * sbuf, char * error_info, int sub_index)
{
  if (sbuf->m_trace_info_count < adata_const_max_trace_info_level-1)
  {
    sbuf->m_trace_infos[sbuf->m_trace_info_count].m_tag_name = error_info;
    sbuf->m_trace_infos[sbuf->m_trace_info_count++].m_sub_index = sub_index;
  }
}

static ADATA_INLINE int adata_stream_buffer_error(adata_stream_buffer * sbuf)
{
  return sbuf->m_error_code > adata_ec_success;
}

static ADATA_INLINE void adata_stream_buffer_error_code(adata_stream_buffer * sbuf , int error_code)
{
  sbuf->m_error_code = error_code;
}

static ADATA_INLINE void u32toa_naive(uint32_t value, char* buffer)
{
  char temp[10];
  char *p = temp;
  do
  {
    *p++ = (char)(value % 10) + '0';
    value /= 10;
  } while (value > 0);

  do
  {
    *buffer++ = *--p;
  } while (p != temp);

  *buffer = '\0';
}

char * adata_stream_buffer_get_trace_info(adata_stream_buffer * sbuf)
{
  size_t len = 0;
  char num_buffer[32];
  for (int32_t i = 0; i < sbuf->m_trace_info_count; ++i)
  {
    len += (strlen(sbuf->m_trace_infos[i].m_tag_name) + 1);
    if (sbuf->m_trace_infos[i].m_sub_index > -1)
    {
      len += 16;
    }
  }
  char * emsg = adata_stream_buffer_error_msg(sbuf->m_error_code);
  size_t elen = strlen(emsg);
  if (elen > 0)
  {
    len += elen + 2;
  }
  char * buffer = (char*)calloc(1, len);
  char * ptr = buffer;
  for (int32_t i = 0; i < sbuf->m_trace_info_count; ++i)
  {
    size_t slen = strlen(sbuf->m_trace_infos[i].m_tag_name);
    memcpy(ptr, sbuf->m_trace_infos[i].m_tag_name, len);
    ptr += slen;
    if (sbuf->m_trace_infos[i].m_sub_index > -1)
    {
      *ptr++ = '[';
      u32toa_naive(sbuf->m_trace_infos[i].m_sub_index, num_buffer);
      size_t slen = strlen(num_buffer);
      memcpy(ptr, num_buffer, len);
      ptr += slen;
      *ptr++ = ']';
    }
    if (i < sbuf->m_trace_info_count - 1)
    {
      *ptr++ = '.';
    }
  }
  if(elen)
  {
    *ptr++ = ' ';
    *ptr++ = ':';
    memcpy(ptr, emsg, elen);
    ptr += elen;
  }
  *ptr = 0;
  return buffer;
}

static ADATA_INLINE void adata_stream_buffer_free(adata_stream_buffer * sbuf)
{
  if(sbuf!=NULL)
  {
    if (sbuf->m_data)
      free(sbuf->m_data);
  }
}

static ADATA_INLINE adata_stream_buffer * adata_stream_buffer_new()
{
	adata_stream_buffer * buf = (adata_stream_buffer*)calloc(1, sizeof(adata_stream_buffer));
  adata_stream_buffer_init(buf);
  return buf;
}

static ADATA_INLINE void adata_stream_buffer_delete(adata_stream_buffer * sbuf)
{
  if (NULL == sbuf)
  {
    return;
  }
  if (sbuf->m_data)
    free(sbuf->m_data);
  free(sbuf);
}

static ADATA_INLINE void adata_stream_buffer_set(adata_stream_buffer * sbuf, char * buf, size_t len)
{
	sbuf->m_data = buf;
  sbuf->m_read_len = 0;
  sbuf->m_write_len = 0;
  sbuf->m_len = len;
  sbuf->m_trace_info_count = 0;
  sbuf->m_write_bytes = 0;
  sbuf->m_error_code = adata_ec_success;
}

static ADATA_INLINE void adata_stream_buffer_clear(adata_stream_buffer * sbuf)
{
  sbuf->m_read_len = 0;
  sbuf->m_write_len = 0;
  sbuf->m_trace_info_count = 0;
  sbuf->m_error_code = adata_ec_success;
}

static ADATA_INLINE int adata_stream_buffer_write(adata_stream_buffer* sbuf, char* buf, size_t len)
{
  if (sbuf->m_len - sbuf->m_write_len < len)
  {
    adata_stream_buffer_error_code(sbuf, adata_ec_stream_buffer_overflow);
    return 1;
  }
  memcpy(sbuf->m_data + sbuf->m_write_len, buf, len);
  sbuf->m_write_len += len;
  return 0;
}

static ADATA_INLINE int adata_stream_buffer_read(adata_stream_buffer* sbuf, char* buf, size_t len)
{
  if( sbuf->m_len - sbuf->m_read_len < len)
  {
    adata_stream_buffer_error_code(sbuf,adata_ec_stream_buffer_overflow);
  }
  memcpy(buf, sbuf->m_data + sbuf->m_read_len, len);
  sbuf->m_read_len += len;
  return 0;
}

static ADATA_INLINE char * adata_stream_buffer_fix_skip_read(adata_stream_buffer* sbuf, size_t len)
{
  if (sbuf->m_len - sbuf->m_read_len < len)
  {
    adata_stream_buffer_error_code(sbuf, adata_ec_stream_buffer_overflow);
    return 0;
  }
  char * pos = sbuf->m_data + sbuf->m_read_len;
  sbuf->m_read_len += len;
  return pos;
}

static ADATA_INLINE unsigned char adata_stream_buffer_get_char(adata_stream_buffer* sbuf)
{
  if (sbuf->m_read_len + 1 > sbuf->m_len)
  {
    adata_stream_buffer_error_code(sbuf, adata_ec_stream_buffer_overflow);
    return 0;
  }
  unsigned char ret = *(sbuf->m_data + sbuf->m_read_len);
  ++sbuf->m_read_len;
  return ret;
}

static ADATA_INLINE int adata_stream_buffer_skip_read_int(adata_stream_buffer* sbuf, size_t len)
{
  uint8_t tag = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return 0;
  }
  if (tag > adata_const_tag_as_value)
  {
    size_t read_bytes = (size_t)(tag & adata_const_interger_byte_msak) + 1;
    if (read_bytes > len)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_stream_buffer_overflow);
      return 0;
    }
    adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return 0;
    }
  }
  return 1;
}

static ADATA_INLINE unsigned char * adata_stream_buffer_append_write(adata_stream_buffer* sbuf, size_t len)
{
  size_t writed_len = (size_t)(sbuf->m_write_len + len);
  if (writed_len > sbuf->m_len)
  {
    adata_stream_buffer_error_code(sbuf,adata_ec_stream_buffer_overflow);
    return 0;
  }
  unsigned char * append_ptr = (unsigned char *)sbuf->m_data + sbuf->m_write_len;
  sbuf->m_write_len += len;
  return append_ptr;
}

static ADATA_INLINE void adata_stream_buffer_pre_write_uint8(adata_stream_buffer * sbuf, uint8_t value)
{
  if (value & adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 2;
  }
  else
  {
    sbuf->m_write_bytes += 1;
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_int8(adata_stream_buffer * sbuf, int8_t value)
{
  if (value & adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 2;
  }
  else
  {
    sbuf->m_write_bytes += 1;
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_uint16(adata_stream_buffer * sbuf, uint16_t value)
{
  if (value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else if (value < 0x100)
  {
    sbuf->m_write_bytes += 2;
  }
  else
  {
    sbuf->m_write_bytes += 3;
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_int16(adata_stream_buffer * sbuf, int16_t value)
{
  if (0 <= value && value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else
  {
    int16_t temp = value;
    if (value < 0)
    {
      temp = -temp;
    }
    if (temp < 0x100)
    {
      sbuf->m_write_bytes += 2;
    }
    else
    {
      sbuf->m_write_bytes += 3;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_uint32(adata_stream_buffer * sbuf, uint32_t value)
{
  if (value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else if (value < 0x100)
  {
    sbuf->m_write_bytes += 2;
  }
  else if (value < 0x10000)
  {
    sbuf->m_write_bytes += 3;
  }
  else if (value < 0x1000000)
  {
    sbuf->m_write_bytes += 4;
  }
  else
  {
    sbuf->m_write_bytes += 5;
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_int32(adata_stream_buffer * sbuf, int32_t value)
{
  if (0 <= value && value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else
  {
    int32_t temp = value;
    if (value < 0)
    {
      temp = -value;
    }
    if (temp < 0x100)
    {
      sbuf->m_write_bytes += 2;
    }
    else if (temp < 0x10000)
    {
      sbuf->m_write_bytes += 3;
    }
    else if (temp < 0x1000000)
    {
      sbuf->m_write_bytes += 4;
    }
    else
    {
      sbuf->m_write_bytes += 5;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_uint64(adata_stream_buffer * sbuf, uint64_t value)
{
  if (value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else if (value < 0x100)
  {
    sbuf->m_write_bytes += 2;
  }
  else if (value < 0x10000)
  {
    sbuf->m_write_bytes += 3;
  }
  else if (value < 0x1000000)
  {
    sbuf->m_write_bytes += 4;
  }
  else if (value < 0x100000000)
  {
    sbuf->m_write_bytes += 5;
  }
  else if (value < 0x10000000000LL)
  {
    sbuf->m_write_bytes += 6;
  }
  else if (value < 0x1000000000000LL)
  {
    sbuf->m_write_bytes += 7;
  }
  else if (value < 0x100000000000000LL)
  {
    sbuf->m_write_bytes += 8;
  }
  else
  {
    sbuf->m_write_bytes += 9;
  }
}

static ADATA_INLINE void adata_stream_buffer_pre_write_int64(adata_stream_buffer * sbuf, int64_t value)
{
  if (0 <= value && value < adata_const_tag_as_type)
  {
    sbuf->m_write_bytes += 1;
  }
  else
  {
    int64_t temp = value;
    if (value < 0)
    {
      temp = -value;
    }
    if (temp < 0x100)
    {
      sbuf->m_write_bytes += 2;
    }
    else if (temp < 0x10000)
    {
      sbuf->m_write_bytes += 3;
    }
    else if (temp < 0x1000000)
    {
      sbuf->m_write_bytes += 4;
    }
    else if (temp < 0x100000000)
    {
      sbuf->m_write_bytes += 5;
    }
    else if (temp < 0x10000000000LL)
    {
      sbuf->m_write_bytes += 6;
    }
    else if (temp < 0x1000000000000LL)
    {
      sbuf->m_write_bytes += 7;
    }
    else if (temp < 0x100000000000000LL)
    {
      sbuf->m_write_bytes += 8;
    }
    else
    {
      sbuf->m_write_bytes += 9;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_fix_read_int8(adata_stream_buffer * sbuf, void* value)
{
  adata_stream_buffer_read(sbuf, (char*)value, 1);
}

static ADATA_INLINE void adata_stream_buffer_fix_read_int16(adata_stream_buffer * sbuf, void* value)
{
  char * value_ptr = (char*)value;
  char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, 2);
  value_ptr[0] = read_ptr[ADATA_LEPOS2_0];
  value_ptr[1] = read_ptr[ADATA_LEPOS2_1];
}

static ADATA_INLINE void adata_stream_buffer_fix_read_int32(adata_stream_buffer * sbuf, void* value)
{
  char * value_ptr = (char*)value;
  char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, 4);
  value_ptr[0] = read_ptr[ADATA_LEPOS4_0];
  value_ptr[1] = read_ptr[ADATA_LEPOS4_1];
  value_ptr[2] = read_ptr[ADATA_LEPOS4_2];
  value_ptr[3] = read_ptr[ADATA_LEPOS4_3];
}

static ADATA_INLINE void adata_stream_buffer_fix_read_int64(adata_stream_buffer * sbuf, void* value)
{
  char * value_ptr = (char*)value;
  char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, 8);
  value_ptr[0] = read_ptr[ADATA_LEPOS8_0];
  value_ptr[1] = read_ptr[ADATA_LEPOS8_1];
  value_ptr[2] = read_ptr[ADATA_LEPOS8_2];
  value_ptr[3] = read_ptr[ADATA_LEPOS8_3];
  value_ptr[4] = read_ptr[ADATA_LEPOS8_4];
  value_ptr[5] = read_ptr[ADATA_LEPOS8_5];
  value_ptr[6] = read_ptr[ADATA_LEPOS8_6];
  value_ptr[7] = read_ptr[ADATA_LEPOS8_7];
}

static ADATA_INLINE void adata_stream_buffer_fix_write_int8(adata_stream_buffer * sbuf, void* value)
{
  adata_stream_buffer_write(sbuf, (char*)value, 1);
}

static ADATA_INLINE void adata_stream_buffer_fix_write_int16(adata_stream_buffer * sbuf, void* value)
{
  unsigned char * value_ptr = (unsigned char*)value;
  unsigned char * write_ptr = adata_stream_buffer_append_write(sbuf, 2);
  write_ptr[0] = value_ptr[ADATA_LEPOS2_0];
  write_ptr[1] = value_ptr[ADATA_LEPOS2_1];
}

static ADATA_INLINE void adata_stream_buffer_fix_write_int32(adata_stream_buffer * sbuf, void* value)
{
  unsigned char * value_ptr = (unsigned char*)value;
  unsigned char * write_ptr = adata_stream_buffer_append_write(sbuf, 4);
  write_ptr[0] = value_ptr[ADATA_LEPOS4_0];
  write_ptr[1] = value_ptr[ADATA_LEPOS4_1];
  write_ptr[2] = value_ptr[ADATA_LEPOS4_2];
  write_ptr[3] = value_ptr[ADATA_LEPOS4_3];
}

static ADATA_INLINE void adata_stream_buffer_fix_write_int64(adata_stream_buffer * sbuf, void* value)
{
  unsigned char * value_ptr = (unsigned char*)value;
  unsigned char * write_ptr = adata_stream_buffer_append_write(sbuf, 8);
  write_ptr[0] = value_ptr[ADATA_LEPOS8_0];
  write_ptr[1] = value_ptr[ADATA_LEPOS8_1];
  write_ptr[2] = value_ptr[ADATA_LEPOS8_2];
  write_ptr[3] = value_ptr[ADATA_LEPOS8_3];
  write_ptr[4] = value_ptr[ADATA_LEPOS8_4];
  write_ptr[5] = value_ptr[ADATA_LEPOS8_5];
  write_ptr[6] = value_ptr[ADATA_LEPOS8_6];
  write_ptr[7] = value_ptr[ADATA_LEPOS8_7];
}

#define adata_stream_buffer_read_float32 adata_stream_buffer_fix_read_int32
#define adata_stream_buffer_read_float64 adata_stream_buffer_fix_read_int64
#define adata_stream_buffer_write_float32 adata_stream_buffer_fix_write_int32
#define adata_stream_buffer_write_float64 adata_stream_buffer_fix_write_int64

static ADATA_INLINE void adata_stream_buffer_read_uint8(adata_stream_buffer * sbuf, uint8_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    if ((long)*value & adata_const_negative_bit_value)
    {
      adata_stream_buffer_error_code(sbuf,adata_ec_negative_assign_to_unsigned_integer_number);
      return;
    }
    int read_bytes = (*value & adata_const_interger_byte_msak) + 1;
    if (1 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = adata_stream_buffer_get_char(sbuf);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_int8(adata_stream_buffer * sbuf, int8_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    int sign = 1;
    if (*value & adata_const_negative_bit_value)
    {
      sign = -1;
    }
    int read_bytes = (*value & adata_const_interger_byte_msak) + 1;
    if (1 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf,adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = adata_stream_buffer_get_char(sbuf);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    if (sign < 0)
    {
      *value = -*value;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_uint16(adata_stream_buffer * sbuf, uint16_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    if (*value & adata_const_negative_bit_value)
    {
      adata_stream_buffer_error_code(sbuf,adata_ec_negative_assign_to_unsigned_integer_number);
      return;
    }
    int read_bytes = (int)(*value & adata_const_interger_byte_msak) + 1;
    if (2 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf,adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS2_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS2_0];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_int16(adata_stream_buffer * sbuf, int16_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    int sign = 1;
    if (*value & adata_const_negative_bit_value)
    {
      sign = -1;
    }
    int read_bytes = (int)(*value & adata_const_interger_byte_msak) + 1;
    if (2 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS2_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS2_0];
    }
    if (sign < 0)
    {
      *value = -*value;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_uint32(adata_stream_buffer * sbuf, uint32_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    if ((long)*value & adata_const_negative_bit_value)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_negative_assign_to_unsigned_integer_number);
      return;
    }
    size_t read_bytes = (size_t)(*value & adata_const_interger_byte_msak) + 1;
    if (4 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 4:value_ptr[3] = read_ptr[ADATA_LEPOS4_3];
    case 3:value_ptr[2] = read_ptr[ADATA_LEPOS4_2];
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS4_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS4_0];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_int32(adata_stream_buffer * sbuf, int32_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    int sign = 1;
    if (*value & adata_const_negative_bit_value)
    {
      sign = -1;
    }
    int read_bytes = (int)(*value & adata_const_interger_byte_msak) + 1;
    if (4 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 4:value_ptr[3] = read_ptr[ADATA_LEPOS4_3];
    case 3:value_ptr[2] = read_ptr[ADATA_LEPOS4_2];
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS4_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS4_0];
    }
    if (sign < 0)
    {
      *value = -*value;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_uint64(adata_stream_buffer * sbuf, uint64_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    if ((long)*value & adata_const_negative_bit_value)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_negative_assign_to_unsigned_integer_number);
      return;
    }
    int read_bytes = (int)(*value & adata_const_interger_byte_msak) + 1;
    if (8 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 8:value_ptr[7] = read_ptr[ADATA_LEPOS8_7];
    case 7:value_ptr[6] = read_ptr[ADATA_LEPOS8_6];
    case 6:value_ptr[5] = read_ptr[ADATA_LEPOS8_5];
    case 5:value_ptr[4] = read_ptr[ADATA_LEPOS8_4];
    case 4:value_ptr[3] = read_ptr[ADATA_LEPOS8_3];
    case 3:value_ptr[2] = read_ptr[ADATA_LEPOS8_2];
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS8_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS8_0];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_read_int64(adata_stream_buffer * sbuf, int64_t* value)
{
  *value = adata_stream_buffer_get_char(sbuf);
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
  if (*value > adata_const_tag_as_value)
  {
    int sign = 1;
    if (*value& adata_const_negative_bit_value)
    {
      sign = -1;
    }
    int read_bytes = (int)(*value & adata_const_interger_byte_msak) + 1;
    if (8 < read_bytes)
    {
      adata_stream_buffer_error_code(sbuf, adata_ec_value_outof_ranger_to_integer_number);
      return;
    }
    *value = 0;
    char * value_ptr = (char*)value;
    char * read_ptr = adata_stream_buffer_fix_skip_read(sbuf, read_bytes);
    if (adata_stream_buffer_error(sbuf))
    {
      return;
    }
    switch (read_bytes)
    {
    case 8:value_ptr[7] = read_ptr[ADATA_LEPOS8_7];
    case 7:value_ptr[6] = read_ptr[ADATA_LEPOS8_6];
    case 6:value_ptr[5] = read_ptr[ADATA_LEPOS8_5];
    case 5:value_ptr[4] = read_ptr[ADATA_LEPOS8_4];
    case 4:value_ptr[3] = read_ptr[ADATA_LEPOS8_3];
    case 3:value_ptr[2] = read_ptr[ADATA_LEPOS8_2];
    case 2:value_ptr[1] = read_ptr[ADATA_LEPOS8_1];
    case 1:value_ptr[0] = read_ptr[ADATA_LEPOS8_0];
    }
    if (sign < 0)
    {
      *value = -*value;
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_write_uint8(adata_stream_buffer * sbuf, uint8_t* value)
{
  if (*value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf, 1);
    *ptr = *value;
  }
  else
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf, 2);
    ptr[0] = 0x80;
    ptr[1] = *value;
  }
}

static ADATA_INLINE void adata_stream_buffer_write_int8(adata_stream_buffer * sbuf, int8_t* value)
{
  if (0 <= *value && *value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf, 1);
    *ptr = *value;
  }
  else
  {
    uint8_t negative_bit = 0;
    int8_t temp = *value;
    if (*value < 0)
    {
      negative_bit = adata_const_negative_bit_value;
      temp = -temp;
    }
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf, 2);
    ptr[0] = 0x80 | negative_bit;
    ptr[1] = temp;
  }
}

ADATA_INLINE void adata_stream_buffer_write_uint16(adata_stream_buffer * sbuf, uint16_t* value)
{
  if (*value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t * ptr = (uint8_t *)(value);
    if (*value < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80;
      wptr[1] = ptr[ADATA_LEPOS2_0];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81;
      wptr[1] = ptr[ADATA_LEPOS2_0];
      wptr[2] = ptr[ADATA_LEPOS2_1];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_write_int16(adata_stream_buffer * sbuf, int16_t* value)
{
  if (0 <= *value && *value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t negative_bit = 0;
    int16_t temp = *value;
    if (*value < 0)
    {
      negative_bit = adata_const_negative_bit_value;
      temp = -temp;
    }
    uint8_t * ptr = (uint8_t *)(&temp);
    if (temp < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS2_0];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS2_0];
      wptr[2] = ptr[ADATA_LEPOS2_1];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_write_uint32(adata_stream_buffer * sbuf, uint32_t* value)
{
  if (*value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t * ptr = (uint8_t *)(value);
    if (*value < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80;
      wptr[1] = ptr[ADATA_LEPOS4_0];
    }
    else if (*value < 0x10000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
    }
    else if (*value < 0x1000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,4);
      wptr[0] = 0x82;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
      wptr[3] = ptr[ADATA_LEPOS4_2];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,5);
      wptr[0] = 0x83;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
      wptr[3] = ptr[ADATA_LEPOS4_2];
      wptr[4] = ptr[ADATA_LEPOS4_3];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_write_int32(adata_stream_buffer * sbuf, int32_t* value)
{
  if (0 <= *value && *value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t negative_bit = 0;
    int32_t temp = *value;
    if (*value < 0)
    {
      negative_bit = adata_const_negative_bit_value;
      temp = -temp;
    }
    uint8_t * ptr = (uint8_t *)(&temp);
    if (temp < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS4_0];
    }
    else if (temp < 0x10000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
    }
    else if (temp < 0x1000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,4);
      wptr[0] = 0x82 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
      wptr[3] = ptr[ADATA_LEPOS4_2];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,5);
      wptr[0] = 0x83 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS4_0];
      wptr[2] = ptr[ADATA_LEPOS4_1];
      wptr[3] = ptr[ADATA_LEPOS4_2];
      wptr[4] = ptr[ADATA_LEPOS4_3];
    }
  }
}

static ADATA_INLINE void adata_stream_buffer_write_uint64(adata_stream_buffer * sbuf, uint64_t* value)
{
  if (*value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t * ptr = (uint8_t *)(value);
    if (*value < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80;
      wptr[1] = ptr[ADATA_LEPOS8_0];
    }
    else if (*value < 0x10000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
    }
    else if (*value < 0x1000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,4);
      wptr[0] = 0x82;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
    }
    else if (*value < 0x100000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,5);
      wptr[0] = 0x83;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
    }
    else if (*value < 0x10000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,6);
      wptr[0] = 0x84;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
    }
    else if (*value < 0x1000000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,7);
      wptr[0] = 0x85;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
    }
    else if (*value < 0x100000000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,8);
      wptr[0] = 0x86;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
      wptr[7] = ptr[ADATA_LEPOS8_6];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,9);
      wptr[0] = 0x87;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
      wptr[7] = ptr[ADATA_LEPOS8_6];
      wptr[8] = ptr[ADATA_LEPOS8_7];
    }
  }
  if (adata_stream_buffer_error(sbuf))
  {
    return;
  }
}

static ADATA_INLINE void adata_stream_buffer_write_int64(adata_stream_buffer * sbuf, int64_t* value)
{
  if (0 <= *value && *value < adata_const_tag_as_type)
  {
    uint8_t * ptr = adata_stream_buffer_append_write(sbuf,1);
    *ptr = (uint8_t)*value;
  }
  else
  {
    uint8_t negative_bit = 0;
    int64_t temp = *value;
    if (*value < 0)
    {
      negative_bit = adata_const_negative_bit_value;
      temp = -temp;
    }
    uint8_t * ptr = (uint8_t *)(&temp);
    if (temp < 0x100)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,2);
      wptr[0] = 0x80 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
    }
    else if (temp < 0x10000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,3);
      wptr[0] = 0x81 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
    }
    else if (temp < 0x1000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,4);
      wptr[0] = 0x82 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
    }
    else if (temp < 0x100000000)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,5);
      wptr[0] = 0x83 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
    }
    else if (temp < 0x10000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,6);
      wptr[0] = 0x84 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
    }
    else if (temp < 0x1000000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,7);
      wptr[0] = 0x85 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
    }
    else if (temp < 0x100000000000000LL)
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,8);
      wptr[0] = 0x86 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
      wptr[7] = ptr[ADATA_LEPOS8_6];
    }
    else
    {
      uint8_t * wptr = adata_stream_buffer_append_write(sbuf,9);
      wptr[0] = 0x87 + negative_bit;
      wptr[1] = ptr[ADATA_LEPOS8_0];
      wptr[2] = ptr[ADATA_LEPOS8_1];
      wptr[3] = ptr[ADATA_LEPOS8_2];
      wptr[4] = ptr[ADATA_LEPOS8_3];
      wptr[5] = ptr[ADATA_LEPOS8_4];
      wptr[6] = ptr[ADATA_LEPOS8_5];
      wptr[7] = ptr[ADATA_LEPOS8_6];
      wptr[8] = ptr[ADATA_LEPOS8_7];
    }
  }
}

#undef ADATA_LEPOS2_0
#undef ADATA_LEPOS2_1
#undef ADATA_LEPOS4_0
#undef ADATA_LEPOS4_1
#undef ADATA_LEPOS4_2
#undef ADATA_LEPOS4_3
#undef ADATA_LEPOS8_0
#undef ADATA_LEPOS8_1
#undef ADATA_LEPOS8_2
#undef ADATA_LEPOS8_3
#undef ADATA_LEPOS8_4
#undef ADATA_LEPOS8_5
#undef ADATA_LEPOS8_6
#undef ADATA_LEPOS8_7

#ifdef __cplusplus
}
#endif

#endif

