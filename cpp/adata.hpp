// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ADATA_HPP_HEADER_
#define ADATA_HPP_HEADER_

#include <cstddef>
#include <exception>
#ifdef _MSC_VER
# if _MSC_VER <= 1500
#   include "stdint.hpp"
# else
#   include <cstdint>
# endif
#elif __cplusplus < 201103L
# include <stdint.h>
#else
# include <cstdint>
#endif

//msvc2013 pls
#ifdef _MSC_VER
# if _MSC_VER <= 1800
#define constexpr
#define noexcept
# endif
#endif // _MSC_VER && (_MSC_VER <= 1800)

#include <cstring>

#include <vector>
#include <map>
#include <string>

//define inline macro
#ifndef ADATA_INLINE
# ifdef _MSC_VER
#  pragma inline_recursion(on)
#  define ADATA_INLINE __forceinline
# elif defined(__GNUC__)
#  define ADATA_INLINE inline __attribute__((always_inline))
# else
#  define ADATA_INLINE inline
# endif
#endif

//define endian check macro

#ifdef _WIN32
#else
#include <arpa/inet.h>  /* __BYTE_ORDER */
#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#    if __BYTE_ORDER == __LITTLE_ENDIAN
#        define __LITTLE_ENDIAN__
#    elif __BYTE_ORDER == __BIG_ENDIAN
#        define __BIG_ENDIAN__
#    elif _WIN32
#        define __LITTLE_ENDIAN__
#    endif
#endif

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

namespace adata
{
  namespace
  {
    enum
    {
      const_tag_as_value = 0x7f,
      const_tag_as_type,
      const_interger_byte_msak = 0x1f,
      const_negative_bit_value = 0x20,
      const_store_postive_integer_byte_mask = 0x80 - 2,
    };
  }

  enum error_code_t
  {
    success = 0,
    negative_assign_to_unsigned_integer_number,
    value_too_large_to_integer_number,
    sequence_length_overflow,
    stream_buffer_overflow,
    number_of_element_not_match,
    undefined_member_protocol_not_compatible,
  };

  class exception : public ::std::exception
  {
  public:
    explicit exception( error_code_t ec)
      :ec_(ec)
    {}

    ADATA_INLINE error_code_t error_code() const { return ec_; }

    ADATA_INLINE static const char * to_message(error_code_t ec)
    {
      switch (ec)
      {
      case negative_assign_to_unsigned_integer_number:
        return "can't assign negative number to unsigned integer number.";
      case value_too_large_to_integer_number:
        return "value too large to integer number";
      case sequence_length_overflow:
        return "sequence length overflow";
      case stream_buffer_overflow:
        return "stream buffer overflow";
      case number_of_element_not_match:
        return "number of element not match";
      default:
        break;
      }
      return "";
    }

    virtual const char * what() const noexcept
    {
      return to_message(ec_);
    }
  private:
    error_code_t ec_;
  };

  template<typename stream_ty, typename ty>
  ADATA_INLINE void fix_skip_read(stream_ty& stream, ty *)
  {
    stream.skip_read(sizeof(ty));    
  }

  template<typename ty>
  ADATA_INLINE int32_t fix_size_of(const ty&)
  {
    return sizeof(ty);
  }

  ADATA_INLINE int32_t size_of(const uint8_t& value)
  {
    if (value & const_tag_as_type)return 2;
    return 1;
  }

  ADATA_INLINE int32_t size_of(const int8_t& value)
  {
    if (value & const_tag_as_type)return 2;
    return 1;
  }

  ADATA_INLINE int32_t size_of(const uint16_t& value)
  {
    if (value < const_tag_as_type) return 1;
    else if (value < 0x100) return 2;
    return 3;
  }

  ADATA_INLINE int32_t size_of(int16_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint16_t temp = value;
    if (value < 0) temp = -value;
    if (temp < 0x100) return 2;
    return 3;
  }

  ADATA_INLINE int32_t size_of(const uint32_t& value)
  {
    if (value < const_tag_as_type) return 1;
    else if (value < 0x100) return 2;
    else if (value < 0x10000) return 3;
    else if (value < 0x1000000) return 4;
    return 5;
  }

  ADATA_INLINE int32_t size_of(int32_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint32_t temp = value;
    if (value < 0) temp = -value;
    if (temp < 0x100) return 2;
    else if (temp < 0x10000) return 3;
    else if (temp < 0x1000000) return 4;
    return 5;
  }

  ADATA_INLINE int32_t size_of(const uint64_t& value)
  {
    if (value < const_tag_as_type) return 1;
    else if (value < 0x100) return 2;
    else if (value < 0x10000) return 3;
    else if (value < 0x1000000) return 4;
    else if (value < 0x100000000) return 5;
    else if (value < 0x10000000000LL) return 6;
    else if (value < 0x1000000000000LL) return 7;
    else if (value < 0x100000000000000LL) return 8;
    return 9;
  }

  ADATA_INLINE int32_t size_of(int64_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint64_t temp = value;
    if (value < 0) value = -value;
    if (temp < 0x100) return 2;
    else if (temp < 0x10000) return 3;
    else if (temp < 0x1000000) return 4;
    else if (temp < 0x100000000) return 5;
    else if (temp < 0x10000000000LL) return 6;
    else if (temp < 0x1000000000000LL) return 7;
    else if (temp < 0x100000000000000LL) return 8;
    return 9;
  }

  ADATA_INLINE int32_t size_of(const float&)
  {
    return sizeof(float);
  };

  ADATA_INLINE int32_t size_of(const double&)
  {
    return sizeof(double);
  };

  template<typename stream_ty>
  struct stream_adapter
  {
    stream_adapter(stream_ty& stream) :m_stream_(stream)  {}

    ADATA_INLINE bool bad()const { return !m_stream_.good(); }

    ADATA_INLINE std::size_t read(char * buffer, std::size_t len)
    { 
      std::size_t sz = m_stream_.read(buffer, len).gcount(); 
      check_bad();
      return sz;
    }

    ADATA_INLINE std::size_t write(const char * buffer, std::size_t len)
    {
      m_stream_.write(buffer, len);
      check_bad();
      return len;
    }

    ADATA_INLINE void skip_read(std::size_t len)
    {
      m_stream_.seekg(len);
      check_bad();
    }

    ADATA_INLINE void clear() { m_stream_.clear();}

    ADATA_INLINE std::size_t read_size() { return 0; }
    ADATA_INLINE std::size_t write_size() { return 0; }

    ADATA_INLINE std::size_t read_length() const
    {
      return (std::size_t)this->m_stream_.tellg();
    }

    ADATA_INLINE std::size_t write_length() const
    {
      return (std::size_t)this->m_stream_.tellp();
    }

    ADATA_INLINE void check_bad()
    {
      if(bad())
      {
        throw exception(stream_buffer_overflow);
      }
    }
  private:
    stream_ty m_stream_;
  };

  template<typename stream_ty>
  ADATA_INLINE stream_adapter<stream_ty> make_store(stream_ty& stream)
  {
    return stream_adapter<stream_ty>(stream);
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, int8_t& value)
  {
    typedef int8_t value_type;
    stream.read((char*)&value, sizeof(value_type));
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    stream.read((char*)&value, sizeof(value_type));
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, int16_t& value)
  {
    typedef int16_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[2];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS2_0] = bytes[0];
    ptr[ADATA_LEPOS2_1] = bytes[1];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, uint16_t& value)
  {
    typedef uint16_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[2];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS2_0] = bytes[0];
    ptr[ADATA_LEPOS2_1] = bytes[1];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, int32_t& value)
  {
    typedef int32_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS4_0] = bytes[0];
    ptr[ADATA_LEPOS4_1] = bytes[1];
    ptr[ADATA_LEPOS4_2] = bytes[2];
    ptr[ADATA_LEPOS4_3] = bytes[3];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, uint32_t& value)
  {
    typedef uint32_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS4_0] = bytes[0];
    ptr[ADATA_LEPOS4_1] = bytes[1];
    ptr[ADATA_LEPOS4_2] = bytes[2];
    ptr[ADATA_LEPOS4_3] = bytes[3];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, int64_t& value)
  {
    typedef int64_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS8_0] = bytes[0];
    ptr[ADATA_LEPOS8_1] = bytes[1];
    ptr[ADATA_LEPOS8_2] = bytes[2];
    ptr[ADATA_LEPOS8_3] = bytes[3];
    ptr[ADATA_LEPOS8_4] = bytes[4];
    ptr[ADATA_LEPOS8_5] = bytes[5];
    ptr[ADATA_LEPOS8_6] = bytes[6];
    ptr[ADATA_LEPOS8_7] = bytes[7];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_read(stream_ty& stream, uint64_t& value)
  {
    typedef uint64_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS8_0] = bytes[0];
    ptr[ADATA_LEPOS8_1] = bytes[1];
    ptr[ADATA_LEPOS8_2] = bytes[2];
    ptr[ADATA_LEPOS8_3] = bytes[3];
    ptr[ADATA_LEPOS8_4] = bytes[4];
    ptr[ADATA_LEPOS8_5] = bytes[5];
    ptr[ADATA_LEPOS8_6] = bytes[6];
    ptr[ADATA_LEPOS8_7] = bytes[7];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const int8_t& value)
  {
    typedef int8_t value_type;
    stream.write((char*)&value, sizeof(value_type));
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const uint8_t& value)
  {
    typedef uint8_t value_type;
    stream.write((char*)&value, sizeof(value_type));
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const int16_t& value)
  {
    typedef int16_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[2];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS2_0];
    bytes[1] = ptr[ADATA_LEPOS2_1];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const uint16_t& value)
  {
    typedef uint16_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[2];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS2_0];
    bytes[1] = ptr[ADATA_LEPOS2_1];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const int32_t& value)
  {
    typedef int32_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const uint32_t& value)
  {
    typedef uint32_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const int64_t& value)
  {
    typedef int64_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    bytes[4] = ptr[ADATA_LEPOS8_4];
    bytes[5] = ptr[ADATA_LEPOS8_5];
    bytes[6] = ptr[ADATA_LEPOS8_6];
    bytes[7] = ptr[ADATA_LEPOS8_7];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void fix_write(stream_ty& stream, const uint64_t& value)
  {
    typedef uint64_t value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    bytes[4] = ptr[ADATA_LEPOS8_4];
    bytes[5] = ptr[ADATA_LEPOS8_5];
    bytes[6] = ptr[ADATA_LEPOS8_6];
    bytes[7] = ptr[ADATA_LEPOS8_7];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, uint8_t *)
  {
    typedef uint8_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, int8_t *)
  {
    typedef int8_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, uint16_t *)
  {
    typedef uint16_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, int16_t *)
  {
    typedef int16_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, uint32_t *)
  {
    typedef uint32_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, int32_t *)
  {
    typedef int32_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, uint64_t *)
  {
    typedef uint64_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, int64_t *)
  {
    typedef int64_t value_type;
    uint8_t value = 0;
    const int bytes = sizeof(value_type);
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, float *)
  {
    typedef float value_type;
    stream.skip_read(sizeof(value_type));
  }

  template<typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, double *)
  {
    typedef double value_type;
    stream.skip_read(sizeof(value_type));
  }

  ADATA_INLINE void check_negative_assaigned_to_unsined_interger(long value)
  {
    if (value & const_negative_bit_value)
    {
      throw exception(negative_assign_to_unsigned_integer_number);
    }
  }

  ADATA_INLINE void check_value_too_large_to_integer_number(int bytes , int read_bytes)
  {
    if (bytes < read_bytes)
    {
      throw exception(value_too_large_to_integer_number);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    const int bytes = sizeof(value_type);
    value_type read_value[2] = { 0 };
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      stream.read((char*)&value, 1);
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, int8_t& value)
  {
    typedef int8_t value_type;
    const int bytes = sizeof(value_type);
    value_type read_value[2] = { 0 };
    stream.read((char*)&value, 1);
    if (value > const_tag_as_value)
    {
      int sign = 1;
      if ((long)value & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      stream.read((char*)&read_value[1], 1);
      if (sign < 0)
      {
        value = -read_value[1];
      }
      else
      {
        value = read_value[1];
      }
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, uint16_t& value)
  {
    typedef uint16_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[2];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)bytes, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      ptr[0] = byte[ADATA_LEPOS2_0];
      ptr[1] = byte[ADATA_LEPOS2_1];
#endif
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, int16_t& value)
  {
    typedef int16_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[2];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      int sign = 1;
      if ((long)value & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)byte, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      ptr[0] = byts[ADATA_LEPOS2_0];
      ptr[1] = byts[ADATA_LEPOS2_1];
#endif
      if (sign < 0)
      {
        value = -value;
      }
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, uint32_t& value)
  {
    typedef uint32_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[4];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)byte, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      switch (read_bytes)
      {
      case 4:ptr[3] = byte[ADATA_LEPOS4_3];
      case 3:ptr[2] = byte[ADATA_LEPOS4_2];
      case 2:ptr[1] = byte[ADATA_LEPOS4_1];
      case 1:ptr[0] = byte[ADATA_LEPOS4_0];
      }
#endif
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, int32_t& value)
  {
    typedef int32_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[4];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      int sign = 1;
      if ((long)value & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)byte, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      switch (read_bytes)
      {
      case 4:ptr[3] = byte[ADATA_LEPOS4_3];
      case 3:ptr[2] = byte[ADATA_LEPOS4_2];
      case 2:ptr[1] = byte[ADATA_LEPOS4_1];
      case 1:ptr[0] = byte[ADATA_LEPOS4_0];
      }
#endif
      if (sign < 0)
      {
        value = -value;
      }
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, uint64_t& value)
  {
    typedef uint64_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[8];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)byte, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      switch (read_bytes)
      {
      case 8:ptr[7] = byte[ADATA_LEPOS8_7];
      case 7:ptr[6] = byte[ADATA_LEPOS8_6];
      case 6:ptr[5] = byte[ADATA_LEPOS8_5];
      case 5:ptr[4] = byte[ADATA_LEPOS8_4];
      case 4:ptr[3] = byte[ADATA_LEPOS8_3];
      case 3:ptr[2] = byte[ADATA_LEPOS8_2];
      case 2:ptr[1] = byte[ADATA_LEPOS8_1];
      case 1:ptr[0] = byte[ADATA_LEPOS8_0];
      }
#endif
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, int64_t& value)
  {
    typedef int64_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t byte[8];
    stream.read((char*)byte, 1);
    value = byte[0];
    if (value > const_tag_as_value)
    {
      int sign = 1;
      if ((long)value & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
#ifdef __LITTLE_ENDIAN__
      stream.read((char*)&value, read_bytes);
#else
      stream.read((char*)byte, read_bytes);
      uint8_t * ptr = (uint8_t *)&value;
      switch (read_bytes)
      {
      case 8:ptr[7] = byte[ADATA_LEPOS8_7];
      case 7:ptr[6] = byte[ADATA_LEPOS8_6];
      case 6:ptr[5] = byte[ADATA_LEPOS8_5];
      case 5:ptr[4] = byte[ADATA_LEPOS8_4];
      case 4:ptr[3] = byte[ADATA_LEPOS8_3];
      case 3:ptr[2] = byte[ADATA_LEPOS8_2];
      case 2:ptr[1] = byte[ADATA_LEPOS8_1];
      case 1:ptr[0] = byte[ADATA_LEPOS8_0];
      }
#endif
      if (sign < 0)
      {
        value = -value;
      }
    }
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, float& value)
  {
    typedef float value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS4_0] = bytes[0];
    ptr[ADATA_LEPOS4_1] = bytes[1];
    ptr[ADATA_LEPOS4_2] = bytes[2];
    ptr[ADATA_LEPOS4_3] = bytes[3];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void read(stream_ty& stream, double& value)
  {
    typedef double value_type;
#ifdef __LITTLE_ENDIAN__
    stream.read((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    stream.read(bytes, sizeof(value_type));
    uint8_t * ptr = (uint8_t *)&value;
    ptr[ADATA_LEPOS8_0] = bytes[0];
    ptr[ADATA_LEPOS8_1] = bytes[1];
    ptr[ADATA_LEPOS8_2] = bytes[2];
    ptr[ADATA_LEPOS8_3] = bytes[3];
    ptr[ADATA_LEPOS8_4] = bytes[4];
    ptr[ADATA_LEPOS8_5] = bytes[5];
    ptr[ADATA_LEPOS8_6] = bytes[6];
    ptr[ADATA_LEPOS8_7] = bytes[7];
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (value < const_tag_as_type)
    {
      bytes[0] = value;
    }
    else
    {
      bytes[0] = const_tag_as_type;
      bytes[1] = value;
      write_bytes = 2;
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, int8_t& value)
  {
    typedef int8_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (0 <= value)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      if (value < 0)
      {
        bytes[0] = 0x80 | const_negative_bit_value;
        bytes[1] = (uint8_t)value;
      }
      else
      {
        bytes[0] = 0x80;
        bytes[1] = (uint8_t)value;
      }
      write_bytes = 2;
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, const uint16_t& value)
  {
    typedef uint16_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t*)(&value);
      if (value < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS2_0];
        write_bytes = 2;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS2_0];
        bytes[2] = ptr[ADATA_LEPOS2_1];
        write_bytes = 3;
      }
      bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, int16_t value)
  {
    typedef int16_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (0 <= value && value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint16_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&temp;
      if (temp < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS2_0];
        write_bytes = 2;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS2_0];
        bytes[2] = ptr[ADATA_LEPOS2_1];
        write_bytes = 3;
      }
      bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, const uint32_t& value)
  {
    typedef uint32_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t*)(&value);
      if (value < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        write_bytes = 2;
      }
      else if (value < 0x10000)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        write_bytes = 3;
      }
      else if (value < 0x1000000)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        bytes[3] = ptr[ADATA_LEPOS4_2];
        write_bytes = 4;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        bytes[3] = ptr[ADATA_LEPOS4_2];
        bytes[4] = ptr[ADATA_LEPOS4_3];
        write_bytes = 5;
      }
      bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, int32_t value)
  {
    typedef int32_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (0 <= value && value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint32_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&value;
      if (temp < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        write_bytes = 2;
      }
      else if (temp < 0x10000)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        write_bytes = 3;
      }
      else if (temp < 0x1000000)
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        bytes[3] = ptr[ADATA_LEPOS4_2];
        write_bytes = 4;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS4_0];
        bytes[2] = ptr[ADATA_LEPOS4_1];
        bytes[3] = ptr[ADATA_LEPOS4_2];
        bytes[4] = ptr[ADATA_LEPOS4_3];
        write_bytes = 5;
      }
      bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, const uint64_t& value)
  {
    typedef uint64_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t*)(&value);
      if (value < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        write_bytes = 2;
      }
      else if (value < 0x10000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        write_bytes = 3;
      }
      else if (value < 0x1000000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        write_bytes = 4;
      }
      else if (value < 0x100000000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        write_bytes = 5;
      }
      else if (value < 0x10000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        write_bytes = 6;
      }
      else if (value < 0x1000000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        write_bytes = 7;
      }
      else if (value < 0x100000000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        bytes[7] = ptr[ADATA_LEPOS8_6];
        write_bytes = 8;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        bytes[7] = ptr[ADATA_LEPOS8_6];
        bytes[8] = ptr[ADATA_LEPOS8_7];
        write_bytes = 9;
      }
      bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, int64_t value)
  {
    typedef int64_t value_type;
    int write_bytes = 0;
    uint8_t bytes[12];
    write_bytes = 1;
    if (0 <= value && value < const_tag_as_type)
    {
      bytes[0] = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint64_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&value;
      if (temp < 0x100)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        write_bytes = 2;
      }
      else if (temp < 0x10000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        write_bytes = 3;
      }
      else if (temp < 0x1000000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        write_bytes = 4;
      }
      else if (temp < 0x100000000)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        write_bytes = 5;
      }
      else if (temp < 0x10000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        write_bytes = 6;
      }
      else if (temp < 0x1000000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        write_bytes = 7;
      }
      else if (temp < 0x100000000000000LL)
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        bytes[7] = ptr[ADATA_LEPOS8_6];
        write_bytes = 8;
      }
      else
      {
        bytes[1] = ptr[ADATA_LEPOS8_0];
        bytes[2] = ptr[ADATA_LEPOS8_1];
        bytes[3] = ptr[ADATA_LEPOS8_2];
        bytes[4] = ptr[ADATA_LEPOS8_3];
        bytes[5] = ptr[ADATA_LEPOS8_4];
        bytes[6] = ptr[ADATA_LEPOS8_5];
        bytes[7] = ptr[ADATA_LEPOS8_6];
        bytes[8] = ptr[ADATA_LEPOS8_7];
        write_bytes = 9;
      }
      bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
    }
    stream.write((char*)bytes, write_bytes);
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, const float& value)
  {
    typedef float value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[4];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream, const double& value)
  {
    typedef double value_type;
#ifdef __LITTLE_ENDIAN__
    stream.write((char*)&value, sizeof(value_type));
#else
    uint8_t bytes[8];
    uint8_t * ptr = (uint8_t *)&value;
    bytes[0] = ptr[ADATA_LEPOS4_0];
    bytes[1] = ptr[ADATA_LEPOS4_1];
    bytes[2] = ptr[ADATA_LEPOS4_2];
    bytes[3] = ptr[ADATA_LEPOS4_3];
    bytes[4] = ptr[ADATA_LEPOS8_4];
    bytes[5] = ptr[ADATA_LEPOS8_5];
    bytes[6] = ptr[ADATA_LEPOS8_6];
    bytes[7] = ptr[ADATA_LEPOS8_7];
    stream.write((const char*)bytes, sizeof(value_type));
#endif
  }

  struct zero_copy_buffer
  {
  private:
    unsigned char const* m_read_header_ptr;
    unsigned char* m_write_header_ptr;
    unsigned char const* m_read_ptr;
    unsigned char* m_write_ptr;
    unsigned char const* m_read_tail_ptr;
    unsigned char const* m_write_tail_ptr;
    bool bad_;
  public:
    zero_copy_buffer()
      :m_read_header_ptr(0),
      m_write_header_ptr(0),
      m_read_ptr(0),
      m_write_ptr(0),
      m_read_tail_ptr(0),
      m_write_tail_ptr(0),
      bad_(false)
    {
    }

    ~zero_copy_buffer()
    {
    }

    ADATA_INLINE bool bad() const { return bad_; }

    ADATA_INLINE void set_read(unsigned char const* buffer, ::std::size_t length)
    {
      this->m_read_header_ptr = buffer;
      this->m_read_ptr = this->m_read_header_ptr;
      this->m_read_tail_ptr = this->m_read_header_ptr + length;
      bad_ = false;
    }

    ADATA_INLINE void set_read(char const* buffer, ::std::size_t length)
    {
      set_read((unsigned char const*)buffer, length);
    }

    ADATA_INLINE void set_write(unsigned char* buffer, ::std::size_t length)
    {
      this->m_write_header_ptr = buffer;
      this->m_write_ptr = this->m_write_header_ptr;
      this->m_write_tail_ptr = this->m_write_header_ptr + length;
      bad_ = false;
    }

    ADATA_INLINE void set_write(char* buffer, ::std::size_t length)
    {
      set_write((unsigned char*)buffer, length);
    }

    ADATA_INLINE::std::size_t read(char * buffer, std::size_t len)
    {
      if (this->m_read_ptr + len > this->m_read_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      std::memcpy(buffer, this->m_read_ptr, len);
      this->m_read_ptr += len;
      return len;
    }

    ADATA_INLINE unsigned char get_char()
    {
      if (this->m_read_ptr + 1 > this->m_read_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      return *m_read_ptr++;
    }

    ADATA_INLINE::std::size_t write(const char * buffer, std::size_t len)
    {
      if (this->m_write_ptr + len > this->m_write_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      std::memcpy((void*)this->m_write_ptr, buffer, len);
      this->m_write_ptr += len;
      return len;
    }

    ADATA_INLINE unsigned char * append_write(std::size_t len)
    {
      if (this->m_write_ptr + len > this->m_write_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      unsigned char * append_ptr = this->m_write_ptr;
      this->m_write_ptr += len;
      return append_ptr;
    }

    ADATA_INLINE unsigned char const* skip_read(::std::size_t len)
    {
      if (this->m_read_ptr + len > this->m_read_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      unsigned char const* ptr = this->m_read_ptr;
      this->m_read_ptr += len;
      return ptr;
    }

    ADATA_INLINE void clear_read()
    {
      this->m_read_ptr = this->m_read_header_ptr;
      bad_ = false;
    }

    ADATA_INLINE void clear_write()
    {
      this->m_write_ptr = this->m_write_header_ptr;
      bad_ = false;
    }

    ADATA_INLINE void clear()
    {
      this->m_read_ptr = this->m_read_header_ptr;
      this->m_write_ptr = this->m_write_header_ptr;
    }

    ADATA_INLINE unsigned char const* read_ptr() const
    {
      return this->m_read_ptr;
    }

    ADATA_INLINE unsigned char* write_ptr() const
    {
      return this->m_write_ptr;
    }

    ADATA_INLINE const char * write_data() const
    {
      return (const char *)this->m_write_header_ptr;
    }

    ADATA_INLINE std::size_t read_length() const
    {
      return this->m_read_ptr - this->m_read_header_ptr;
    }

    ADATA_INLINE std::size_t write_length() const
    {
      return this->m_write_ptr - this->m_write_header_ptr;
    }

    ADATA_INLINE std::size_t read_size() { return this->m_read_tail_ptr - this->m_read_header_ptr; }
    ADATA_INLINE std::size_t write_size() { return this->m_write_tail_ptr - this->m_write_header_ptr; }

  };

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, int8_t& value)
  {
    value = (int8_t)stream.get_char();
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, uint8_t& value)
  {
    value = stream.get_char();
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, int16_t& value)
  {
    typedef int16_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS2_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS2_1] = read_ptr[1];
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, uint16_t& value)
  {
    typedef uint16_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS2_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS2_1] = read_ptr[1];
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, int32_t& value)
  {
    typedef int32_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS4_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS4_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS4_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS4_3] = read_ptr[3];
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, uint32_t& value)
  {
    typedef uint32_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS4_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS4_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS4_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS4_3] = read_ptr[3];
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, int64_t& value)
  {
    typedef int64_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS8_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS8_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS8_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS8_3] = read_ptr[3];
    value_ptr[ADATA_LEPOS8_4] = read_ptr[4];
    value_ptr[ADATA_LEPOS8_5] = read_ptr[5];
    value_ptr[ADATA_LEPOS8_6] = read_ptr[6];
    value_ptr[ADATA_LEPOS8_7] = read_ptr[7];
  }

  ADATA_INLINE void fix_read(zero_copy_buffer& stream, uint64_t& value)
  {
    typedef uint64_t value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS8_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS8_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS8_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS8_3] = read_ptr[3];
    value_ptr[ADATA_LEPOS8_4] = read_ptr[4];
    value_ptr[ADATA_LEPOS8_5] = read_ptr[5];
    value_ptr[ADATA_LEPOS8_6] = read_ptr[6];
    value_ptr[ADATA_LEPOS8_7] = read_ptr[7];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const int8_t& value)
  {
    uint8_t * ptr = stream.append_write(1);
    *ptr = (uint8_t)value;
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const uint8_t& value)
  {
    uint8_t * ptr = stream.append_write(1);
    *ptr = value;
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const int16_t& value)
  {
    typedef int16_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS2_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS2_1] = value_ptr[1];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const uint16_t& value)
  {
    typedef uint16_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS2_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS2_1] = value_ptr[1];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const int32_t& value)
  {
    typedef int32_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS4_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS4_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS4_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS4_3] = value_ptr[3];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const uint32_t& value)
  {
    typedef uint32_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS4_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS4_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS4_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS4_3] = value_ptr[3];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const int64_t& value)
  {
    typedef int64_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS8_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS8_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS8_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS8_3] = value_ptr[3];
    write_ptr[ADATA_LEPOS8_4] = value_ptr[4];
    write_ptr[ADATA_LEPOS8_5] = value_ptr[5];
    write_ptr[ADATA_LEPOS8_6] = value_ptr[6];
    write_ptr[ADATA_LEPOS8_7] = value_ptr[7];
  }

  ADATA_INLINE void fix_write(zero_copy_buffer& stream, const uint64_t& value)
  {
    typedef uint64_t value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS8_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS8_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS8_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS8_3] = value_ptr[3];
    write_ptr[ADATA_LEPOS8_4] = value_ptr[4];
    write_ptr[ADATA_LEPOS8_5] = value_ptr[5];
    write_ptr[ADATA_LEPOS8_6] = value_ptr[6];
    write_ptr[ADATA_LEPOS8_7] = value_ptr[7];
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, uint8_t *)
  {
    typedef uint8_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, int8_t *)
  {
    typedef int8_t value_type;
    uint8_t value = 0;
    value = (int8_t)stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, uint16_t *)
  {
    typedef uint16_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, int16_t *)
  {
    typedef int16_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, uint32_t *)
  {
    typedef uint32_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, int32_t *)
  {
    typedef int32_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, uint64_t *)
  {
    typedef uint64_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void skip_read(zero_copy_buffer& stream, int64_t *)
  {
    typedef int64_t value_type;
    uint8_t value = 0;
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      int read_bytes = (value & const_interger_byte_msak) + 1;
      stream.skip_read(read_bytes);
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    const int bytes = sizeof(value_type);
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = stream.get_char();
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, int8_t& value)
  {
    typedef int8_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t tag = stream.get_char();
    if (tag > const_tag_as_value)
    {
      int sign = 1;
      if ((long)tag & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (int(tag) & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = stream.get_char();
      if (sign < 0)
      {
        value = -value;
      }
    }
    else
    {
      value = tag;
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, uint16_t& value)
  {
    typedef uint16_t value_type;
    const int bytes = sizeof(value_type);
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (int(value) & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
      switch (read_bytes)
      {
      case 2:value_ptr[1] = read_ptr[ADATA_LEPOS2_1];
      case 1:value_ptr[0] = read_ptr[ADATA_LEPOS2_0];
      }
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, int16_t& value)
  {
    typedef int16_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t tag = stream.get_char();
    if (tag > const_tag_as_value)
    {
      int sign = 1;
      if ((long)tag & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (int(tag) & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
      switch (read_bytes)
      {
      case 2:value_ptr[1] = read_ptr[ADATA_LEPOS2_1];
      case 1:value_ptr[0] = read_ptr[ADATA_LEPOS2_0];
      }
      if (sign < 0)
      {
        value = -value;
      }
    }
    else
    {
      value = tag;
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, uint32_t& value)
  {
    typedef uint32_t value_type;
    const ::std::size_t bytes = sizeof(value_type);
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(value);
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
      switch (read_bytes)
      {
      case 4:value_ptr[3] = read_ptr[ADATA_LEPOS4_3];
      case 3:value_ptr[2] = read_ptr[ADATA_LEPOS4_2];
      case 2:value_ptr[1] = read_ptr[ADATA_LEPOS4_1];
      case 1:value_ptr[0] = read_ptr[ADATA_LEPOS4_0];
      }
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, int32_t& value)
  {
    typedef int32_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t tag = stream.get_char();
    if (tag > const_tag_as_value)
    {
      int sign = 1;
      if ((long)tag & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (int(tag) & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
      switch (read_bytes)
      {
      case 4:value_ptr[3] = read_ptr[ADATA_LEPOS4_3];
      case 3:value_ptr[2] = read_ptr[ADATA_LEPOS4_2];
      case 2:value_ptr[1] = read_ptr[ADATA_LEPOS4_1];
      case 1:value_ptr[0] = read_ptr[ADATA_LEPOS4_0];
      }
      if (sign < 0)
      {
        value = -value;
      }
    }
    else
    {
      value = tag;
    }
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, uint64_t& value)
  {
    typedef uint64_t value_type;
    const int bytes = sizeof(value_type);
    value = stream.get_char();
    if (value > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger((long)value);
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
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

  ADATA_INLINE void read(zero_copy_buffer& stream, int64_t& value)
  {
    typedef int64_t value_type;
    const int bytes = sizeof(value_type);
    uint8_t tag = stream.get_char();
    if (tag > const_tag_as_value)
    {
      int sign = 1;
      if ((long)tag & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (int(tag) & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
      uint8_t const* read_ptr = stream.skip_read(read_bytes);
      uint8_t * value_ptr = (uint8_t *)&value;
      value = 0;
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
        value = -value;
      }
    }
    else
    {
      value = tag;
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, const uint8_t& value)
  {
    typedef uint8_t value_type;
    if (value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = value;
    }
    else
    {
      uint8_t * ptr = stream.append_write(2);
      ptr[0] = 0x80;
      ptr[1] = value;
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, int8_t value)
  {
    typedef int8_t value_type;
    if (0 <= value)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint8_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = stream.append_write(2);
      ptr[0] = 0x80 | negative_bit;
      ptr[1] = temp;
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, const uint16_t& value)
  {
    typedef uint16_t value_type;
    if (value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t *)&value;
      if (value < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80;
        wptr[1] = ptr[ADATA_LEPOS2_0];
      }
      else
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81;
        wptr[1] = ptr[ADATA_LEPOS2_0];
        wptr[2] = ptr[ADATA_LEPOS2_1];
      }
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, int16_t value)
  {
    typedef int16_t value_type;
    if (0 <= value && value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint16_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&value;
      if (temp < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS2_0];
      }
      else
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS2_0];
        wptr[2] = ptr[ADATA_LEPOS2_1];
      }
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, const uint32_t& value)
  {
    typedef uint32_t value_type;
    if (value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t *)&value;
      if (value < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80;
        wptr[1] = ptr[ADATA_LEPOS4_0];
      }
      else if (value < 0x10000)
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
      }
      else if (value < 0x1000000)
      {
        uint8_t * wptr = stream.append_write(4);
        wptr[0] = 0x82;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
        wptr[3] = ptr[ADATA_LEPOS4_2];
      }
      else
      {
        uint8_t * wptr = stream.append_write(5);
        wptr[0] = 0x83;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
        wptr[3] = ptr[ADATA_LEPOS4_2];
        wptr[4] = ptr[ADATA_LEPOS4_3];
      }
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, int32_t value)
  {
    typedef int32_t value_type;
    if (0 <= value && value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint32_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&value;
      if (temp < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS4_0];
      }
      else if (temp < 0x10000)
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
      }
      else if (temp < 0x1000000)
      {
        uint8_t * wptr = stream.append_write(4);
        wptr[0] = 0x82 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
        wptr[3] = ptr[ADATA_LEPOS4_2];
      }
      else
      {
        uint8_t * wptr = stream.append_write(5);
        wptr[0] = 0x83 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS4_0];
        wptr[2] = ptr[ADATA_LEPOS4_1];
        wptr[3] = ptr[ADATA_LEPOS4_2];
        wptr[4] = ptr[ADATA_LEPOS4_3];
      }
    }
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, const uint64_t& value)
  {
    typedef uint64_t value_type;
    if (value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t * ptr = (uint8_t *)&value;
      if (value < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80;
        wptr[1] = ptr[ADATA_LEPOS8_0];
      }
      else if (value < 0x10000)
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
      }
      else if (value < 0x1000000)
      {
        uint8_t * wptr = stream.append_write(4);
        wptr[0] = 0x82;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
      }
      else if (value < 0x100000000)
      {
        uint8_t * wptr = stream.append_write(5);
        wptr[0] = 0x83;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
        wptr[4] = ptr[ADATA_LEPOS8_3];
      }
      else if (value < 0x10000000000LL)
      {
        uint8_t * wptr = stream.append_write(6);
        wptr[0] = 0x84;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
        wptr[4] = ptr[ADATA_LEPOS8_3];
        wptr[5] = ptr[ADATA_LEPOS8_4];
      }
      else if (value < 0x1000000000000LL)
      {
        uint8_t * wptr = stream.append_write(7);
        wptr[0] = 0x85;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
        wptr[4] = ptr[ADATA_LEPOS8_3];
        wptr[5] = ptr[ADATA_LEPOS8_4];
        wptr[6] = ptr[ADATA_LEPOS8_5];
      }
      else if (value < 0x100000000000000LL)
      {
        uint8_t * wptr = stream.append_write(8);
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
        uint8_t * wptr = stream.append_write(9);
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
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, int64_t value)
  {
    typedef int64_t value_type;
    if (0 <= value && value < const_tag_as_type)
    {
      uint8_t * ptr = stream.append_write(1);
      *ptr = (uint8_t)value;
    }
    else
    {
      uint8_t negative_bit = 0;
      uint64_t temp = value;
      if (value < 0)
      {
        negative_bit = const_negative_bit_value;
        temp = -value;
      }
      uint8_t * ptr = (uint8_t *)&value;
      if (temp < 0x100)
      {
        uint8_t * wptr = stream.append_write(2);
        wptr[0] = 0x80 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS8_0];
      }
      else if (temp < 0x10000)
      {
        uint8_t * wptr = stream.append_write(3);
        wptr[0] = 0x81 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
      }
      else if (temp < 0x1000000)
      {
        uint8_t * wptr = stream.append_write(4);
        wptr[0] = 0x82 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
      }
      else if (temp < 0x100000000)
      {
        uint8_t * wptr = stream.append_write(5);
        wptr[0] = 0x83 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
        wptr[4] = ptr[ADATA_LEPOS8_3];
      }
      else if (temp < 0x10000000000LL)
      {
        uint8_t * wptr = stream.append_write(6);
        wptr[0] = 0x84 + negative_bit;
        wptr[1] = ptr[ADATA_LEPOS8_0];
        wptr[2] = ptr[ADATA_LEPOS8_1];
        wptr[3] = ptr[ADATA_LEPOS8_2];
        wptr[4] = ptr[ADATA_LEPOS8_3];
        wptr[5] = ptr[ADATA_LEPOS8_4];
      }
      else if (temp < 0x1000000000000LL)
      {
        uint8_t * wptr = stream.append_write(7);
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
        uint8_t * wptr = stream.append_write(8);
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
        uint8_t * wptr = stream.append_write(9);
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

  ADATA_INLINE void write(zero_copy_buffer& stream, const float& value)
  {
    typedef float value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS4_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS4_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS4_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS4_3] = value_ptr[3];
  }

  ADATA_INLINE void write(zero_copy_buffer& stream, const double& value)
  {
    typedef double value_type;
    uint8_t * write_ptr = stream.append_write(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    write_ptr[ADATA_LEPOS8_0] = value_ptr[0];
    write_ptr[ADATA_LEPOS8_1] = value_ptr[1];
    write_ptr[ADATA_LEPOS8_2] = value_ptr[2];
    write_ptr[ADATA_LEPOS8_3] = value_ptr[3];
    write_ptr[ADATA_LEPOS8_4] = value_ptr[4];
    write_ptr[ADATA_LEPOS8_5] = value_ptr[5];
    write_ptr[ADATA_LEPOS8_6] = value_ptr[6];
    write_ptr[ADATA_LEPOS8_7] = value_ptr[7];
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, float& value)
  {
    typedef float value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS4_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS4_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS4_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS4_3] = read_ptr[3];
  }

  ADATA_INLINE void read(zero_copy_buffer& stream, double& value)
  {
    typedef double value_type;
    uint8_t const* read_ptr = stream.skip_read(sizeof(value_type));
    uint8_t * value_ptr = (uint8_t *)&value;
    value_ptr[ADATA_LEPOS8_0] = read_ptr[0];
    value_ptr[ADATA_LEPOS8_1] = read_ptr[1];
    value_ptr[ADATA_LEPOS8_2] = read_ptr[2];
    value_ptr[ADATA_LEPOS8_3] = read_ptr[3];
    value_ptr[ADATA_LEPOS8_4] = read_ptr[4];
    value_ptr[ADATA_LEPOS8_5] = read_ptr[5];
    value_ptr[ADATA_LEPOS8_6] = read_ptr[6];
    value_ptr[ADATA_LEPOS8_7] = read_ptr[7];
  }

  template<typename alloc_type>
  ADATA_INLINE int32_t size_of(std::basic_string<char, std::char_traits<char>, alloc_type> const& str)
  {
    int32_t len = (int32_t)str.length();
    len += size_of(len);
    return len;
  }

  template<typename stream_ty ,typename alloc_type>
  ADATA_INLINE void read(stream_ty& stream, std::basic_string<char, std::char_traits<char>, alloc_type>& str)
  {
    int32_t len;
    read(stream, len);
    str.resize(len);
    stream.read((char *)str.data(), len);
  }

  template<typename stream_ty, typename alloc_type>
  ADATA_INLINE void write(stream_ty& stream, std::basic_string<char, std::char_traits<char>, alloc_type> const& str)
  {
    int32_t len = (int32_t)str.length();
    write(stream, len);
    stream.write(str.data(), len);
  }

  template<typename stream_ty>
  ADATA_INLINE int32_t check_read_size(stream_ty& stream, int size = 0)
  {
    int32_t len;
    read(stream, len);
    if (size > 0 && len > size)
    {
      throw exception(number_of_element_not_match);
    }
    return len;
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read_compatible(stream_ty& stream)
  {
    ::std::size_t offset = stream.read_length();
    int64_t tag = 0;
    read(stream, tag);
    int32_t len_tag = 0;
    read(stream, len_tag);

    if (len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if (len > read_len) stream.skip_read(len - read_len);
    }
  }

  template<typename stream_ty , typename ty>
  ADATA_INLINE void read_ec(stream_ty& stream , ty& value , error_code_t& ec)
  {
    try
    {
      read(stream, value);
    }
    catch(exception& e)
    {
      ec = e.error_code();
    }
  }


  template<typename stream_ty, typename ty>
  ADATA_INLINE void fix_read_ec(stream_ty& stream, ty& value, error_code_t& ec)
  {
    try
    {
      fix_read(stream, value);
    }
    catch (exception& e)
    {
      ec = e.error_code();
    }
  }

  template<typename stream_ty, typename ty>
  ADATA_INLINE void write_ec(stream_ty& stream, ty& value, error_code_t& ec)
  {
    try
    {
      write(stream, value);
    }
    catch (exception& e)
    {
      ec = e.error_code();
    }
  }

  template<typename stream_ty, typename ty>
  ADATA_INLINE void fix_write_ec(stream_ty& stream, ty& value, error_code_t& ec)
  {
    try
    {
      fix_write(stream, value);
    }
    catch (exception& e)
    {
      ec = e.error_code();
    }
  }

  template<typename T>
  struct is_adata
  {
    static const bool value = false;
  };
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

#endif
