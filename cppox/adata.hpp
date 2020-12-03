// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <endians.h>
#include <exception>
#include <map>
#include <string>
#include <vector>

namespace adata
{
  namespace detail
  {
    constexpr inline bool endian_native_is_little = (std::endian::native == std::endian::little);

    template<int size>
    struct endian_pos {
      int idx[size] = {};
      constexpr endian_pos()
      {
        if constexpr (endian_native_is_little)
        {
          for (int i = 0; i < size; i++)
            idx[i] = i;
        }
        else
        {
          for (int i = 0; i < size; i++)
            idx[size - i - 1] = i;
        }
      }
    };

    template <typename T>
    class has_appendwrite
    {
      typedef char one;
      struct two { char x[2]; };

      template <typename C> static one test(decltype(&C::append_write));
      template <typename C> static two test(...);

    public:
      static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template <typename T>
    class has_getchar
    {
      typedef char one;
      struct two { char x[2]; };

      template <typename C> static one test(decltype(&C::get_char));
      template <typename C> static two test(...);

    public:
      static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template <typename T>
    class has_skipread
    {
      typedef char one;
      struct two { char x[2]; };

      template <typename C> static one test(decltype(&C::skip_read));
      template <typename C> static two test(...);

    public:
      static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
    };
  }

  constexpr inline auto ap2 = detail::endian_pos<2>();
  constexpr inline auto ap4 = detail::endian_pos<4>();
  constexpr inline auto ap8 = detail::endian_pos<8>();

  namespace
  {
    constexpr inline uint8_t const_tag_as_value = 0x7f;
    constexpr inline uint8_t const_tag_as_type = 0x80;
    constexpr inline uint8_t const_interger_byte_msak = 0x1f;
    constexpr inline uint8_t const_negative_bit_value = 0x20;
    constexpr inline uint8_t const_store_postive_integer_byte_mask = 0x80 - 2;
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
    explicit exception(error_code_t ec)
      :ec_(ec)
    {}

    inline error_code_t error_code() const { return ec_; }

    inline static const char* to_message(error_code_t ec)
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

    virtual const char* what() const noexcept
    {
      return to_message(ec_);
    }
  private:
    error_code_t ec_;
  };

  template<typename stream_ty, typename ty>
  inline void fix_skip_read(stream_ty& stream, ty*)
  {
    stream.skip_read(sizeof(ty));
  }

  template<typename ty>
  inline int32_t fix_size_of(const ty&)
  {
    return sizeof(ty);
  }

  inline int32_t size_of(const uint8_t& value)
  {
    if (value & const_tag_as_type)return 2;
    return 1;
  }

  inline int32_t size_of(const int8_t& value)
  {
    if (value & const_tag_as_type)return 2;
    return 1;
  }

  inline int32_t size_of(const uint16_t& value)
  {
    if (value < const_tag_as_type) return 1;
    else if (value < 0x100) return 2;
    return 3;
  }

  inline int32_t size_of(int16_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint16_t temp = value;
    if (value < 0) temp = -value;
    if (temp < 0x100) return 2;
    return 3;
  }

  inline int32_t size_of(const uint32_t& value)
  {
    if (value < const_tag_as_type) return 1;
    else if (value < 0x100) return 2;
    else if (value < 0x10000) return 3;
    else if (value < 0x1000000) return 4;
    return 5;
  }

  inline int32_t size_of(int32_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint32_t temp = value;
    if (value < 0) temp = -value;
    if (temp < 0x100) return 2;
    else if (temp < 0x10000) return 3;
    else if (temp < 0x1000000) return 4;
    return 5;
  }

  inline int32_t size_of(const uint64_t& value)
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

  inline int32_t size_of(int64_t value)
  {
    if (0 <= value && value < const_tag_as_type) return 1;
    uint64_t temp = value;
    if (value < 0) temp = -value;
    if (temp < 0x100) return 2;
    else if (temp < 0x10000) return 3;
    else if (temp < 0x1000000) return 4;
    else if (temp < 0x100000000) return 5;
    else if (temp < 0x10000000000LL) return 6;
    else if (temp < 0x1000000000000LL) return 7;
    else if (temp < 0x100000000000000LL) return 8;
    return 9;
  }

  inline int32_t size_of(const float&)
  {
    return sizeof(float);
  };

  inline int32_t size_of(const double&)
  {
    return sizeof(double);
  };

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

    inline bool bad() const { return bad_; }

    inline void set_read(unsigned char const* buffer, ::std::size_t length)
    {
      this->m_read_header_ptr = buffer;
      this->m_read_ptr = this->m_read_header_ptr;
      this->m_read_tail_ptr = this->m_read_header_ptr + length;
      bad_ = false;
    }

    inline void set_read(char const* buffer, ::std::size_t length)
    {
      set_read((unsigned char const*)buffer, length);
    }

    inline void set_write(unsigned char* buffer, ::std::size_t length)
    {
      this->m_write_header_ptr = buffer;
      this->m_write_ptr = this->m_write_header_ptr;
      this->m_write_tail_ptr = this->m_write_header_ptr + length;
      bad_ = false;
    }

    inline void set_write(char* buffer, ::std::size_t length)
    {
      set_write((unsigned char*)buffer, length);
    }

    inline::std::size_t read(char* buffer, std::size_t len)
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

    inline unsigned char get_char()
    {
      if (this->m_read_ptr + 1 > this->m_read_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      return *m_read_ptr++;
    }

    inline::std::size_t write(const char* buffer, std::size_t len)
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

    inline unsigned char* append_write(std::size_t len)
    {
      if (this->m_write_ptr + len > this->m_write_tail_ptr)
      {
        bad_ = true;
        throw exception(stream_buffer_overflow);
      }
      unsigned char* append_ptr = this->m_write_ptr;
      this->m_write_ptr += len;
      return append_ptr;
    }

    inline unsigned char const* skip_read(::std::size_t len)
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

    inline void clear_read()
    {
      this->m_read_ptr = this->m_read_header_ptr;
      bad_ = false;
    }

    inline void clear_write()
    {
      this->m_write_ptr = this->m_write_header_ptr;
      bad_ = false;
    }

    inline void clear()
    {
      this->m_read_ptr = this->m_read_header_ptr;
      this->m_write_ptr = this->m_write_header_ptr;
    }

    inline unsigned char const* read_ptr() const
    {
      return this->m_read_ptr;
    }

    inline unsigned char* write_ptr() const
    {
      return this->m_write_ptr;
    }

    inline const char* write_data() const
    {
      return (const char*)this->m_write_header_ptr;
    }

    inline std::size_t read_length() const
    {
      return this->m_read_ptr - this->m_read_header_ptr;
    }

    inline std::size_t write_length() const
    {
      return this->m_write_ptr - this->m_write_header_ptr;
    }

    inline std::size_t read_size() { return this->m_read_tail_ptr - this->m_read_header_ptr; }
    inline std::size_t write_size() { return this->m_write_tail_ptr - this->m_write_header_ptr; }

  };

  template<typename stream_ty>
  struct stream_adapter
  {
    stream_adapter(stream_ty& stream) :m_stream_(stream) {}

    inline bool bad()const { return !m_stream_.good(); }

    inline std::size_t read(char* buffer, std::size_t len)
    {
      std::size_t sz = m_stream_.read(buffer, len).gcount();
      check_bad();
      return sz;
    }

    inline std::size_t write(const char* buffer, std::size_t len)
    {
      m_stream_.write(buffer, len);
      check_bad();
      return len;
    }

    inline void skip_read(std::size_t len)
    {
      m_stream_.seekg(len);
      check_bad();
    }

    inline void clear() { m_stream_.clear(); }

    inline std::size_t read_size() { return 0; }
    inline std::size_t write_size() { return 0; }

    inline std::size_t read_length() const
    {
      return (std::size_t)this->m_stream_.tellg();
    }

    inline std::size_t write_length() const
    {
      return (std::size_t)this->m_stream_.tellp();
    }

    inline void check_bad()
    {
      if (bad())
      {
        throw exception(stream_buffer_overflow);
      }
    }
  private:
    stream_ty m_stream_;
  };

  template<typename stream_ty>
  inline stream_adapter<stream_ty> make_store(stream_ty& stream)
  {
    return stream_adapter<stream_ty>(stream);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, int8_t& value)
  {
    typedef int8_t value_type;
    if (detail::has_getchar<stream_ty>::value)
    {
      value = (value_type)stream.get_char();
    }
    else
    {
      stream.read((char*)&value, sizeof(value_type));
    }
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    if (detail::has_getchar<stream_ty>::value)
    {
      value = (value_type)stream.get_char();
    }
    else
    {
      stream.read((char*)&value, sizeof(value_type));
    }
  }

  template<size_t byte_size, size_t size>
  inline void read_to_byte(uint8_t* ptr, uint8_t* bytes)
  {
    static_assert((byte_size == 2) || (byte_size == 4) || (byte_size == 8)
      , "byte_size not support");
    if constexpr (byte_size == 2)
    {
      static_assert(size <= 2, "out of range");
      if constexpr (size >= 2)ptr[ap2.idx[1]] = bytes[1];
      if constexpr (size >= 1)ptr[ap2.idx[0]] = bytes[0];
    }
    else if constexpr (byte_size == 4)
    {
      static_assert(size <= 4, "out of range");
      if constexpr (size >= 4)ptr[ap4.idx[3]] = bytes[3];
      if constexpr (size >= 3)ptr[ap4.idx[2]] = bytes[2];
      if constexpr (size >= 2)ptr[ap4.idx[1]] = bytes[1];
      if constexpr (size >= 1)ptr[ap4.idx[0]] = bytes[0];
    }
    else if constexpr (byte_size == 8)
    {
      static_assert(size <= 8, "out of range");
      if constexpr (size >= 8)ptr[ap8.idx[7]] = bytes[7];
      if constexpr (size >= 7)ptr[ap8.idx[6]] = bytes[6];
      if constexpr (size >= 6)ptr[ap8.idx[5]] = bytes[5];
      if constexpr (size >= 5)ptr[ap8.idx[4]] = bytes[4];
      if constexpr (size >= 4)ptr[ap8.idx[3]] = bytes[3];
      if constexpr (size >= 3)ptr[ap8.idx[2]] = bytes[2];
      if constexpr (size >= 2)ptr[ap8.idx[1]] = bytes[1];
      if constexpr (size >= 1)ptr[ap8.idx[0]] = bytes[0];
    }
  }

  template<size_t byte_size, size_t  size>
  inline void write_to_byte(uint8_t* ptr, uint8_t* bytes)
  {
    static_assert((byte_size == 2) || (byte_size == 4) || (byte_size == 8)
      , "byte_size not support");
    if constexpr (byte_size == 2)
    {
      static_assert(size <= 2, "out of range");
      if constexpr (size >= 2)bytes[1] = ptr[ap2.idx[1]];
      if constexpr (size >= 1)bytes[0] = ptr[ap2.idx[0]];
    }
    else if constexpr (byte_size == 4)
    {
      static_assert(size <= 4, "out of range");
      if constexpr (size >= 4)bytes[3] = ptr[ap4.idx[3]];
      if constexpr (size >= 3)bytes[2] = ptr[ap4.idx[2]];
      if constexpr (size >= 2)bytes[1] = ptr[ap4.idx[1]];
      if constexpr (size >= 1)bytes[0] = ptr[ap4.idx[0]];
    }
    else if constexpr (byte_size == 8)
    {
      static_assert(size <= 8, "out of range");
      if constexpr (size >= 8)bytes[7] = ptr[ap8.idx[7]];
      if constexpr (size >= 7)bytes[6] = ptr[ap8.idx[6]];
      if constexpr (size >= 6)bytes[5] = ptr[ap8.idx[5]];
      if constexpr (size >= 5)bytes[4] = ptr[ap8.idx[4]];
      if constexpr (size >= 4)bytes[3] = ptr[ap8.idx[3]];
      if constexpr (size >= 3)bytes[2] = ptr[ap8.idx[2]];
      if constexpr (size >= 2)bytes[1] = ptr[ap8.idx[1]];
      if constexpr (size >= 1)bytes[0] = ptr[ap8.idx[0]];
    }
  }

  template<size_t byte_size>
  inline void read_to_byte(uint8_t* ptr, uint8_t* bytes, size_t  size)
  {
    static_assert((byte_size == 2) || (byte_size == 4) || (byte_size == 8)
      , "byte_size not support");
    if constexpr (byte_size == 2)
    {
      switch (size)
      {
      case 2:ptr[ap2.idx[1]] = bytes[1];
      case 1:ptr[ap2.idx[0]] = bytes[0];
        break;
      default:
        assert(false);
      }
    }
    else if constexpr (byte_size == 4)
    {
      switch (size)
      {
      case 4:ptr[ap4.idx[3]] = bytes[3];
      case 3:ptr[ap4.idx[2]] = bytes[2];
      case 2:ptr[ap4.idx[1]] = bytes[1];
      case 1:ptr[ap4.idx[0]] = bytes[0];
        break;
      default:
        assert(false);
      }
    }
    else if constexpr (byte_size == 8)
    {
      switch (size)
      {
      case 8:ptr[ap8.idx[7]] = bytes[7];
      case 7:ptr[ap8.idx[6]] = bytes[6];
      case 6:ptr[ap8.idx[5]] = bytes[5];
      case 5:ptr[ap8.idx[4]] = bytes[4];
      case 4:ptr[ap8.idx[3]] = bytes[3];
      case 3:ptr[ap8.idx[2]] = bytes[2];
      case 2:ptr[ap8.idx[1]] = bytes[1];
      case 1:ptr[ap8.idx[0]] = bytes[0];
        break;
      default:
        assert(false);
      }
    }
  }

  template<size_t byte_size>
  inline void write_to_byte(uint8_t* ptr, uint8_t* bytes, size_t size)
  {
    static_assert((byte_size == 2) || (byte_size == 4) || (byte_size == 8)
      , "byte_size not support");
    if constexpr (byte_size == 2)
    {
      switch (size)
      {
      case 2:bytes[1] = ptr[ap2.idx[1]];
      case 1:bytes[0] = ptr[ap2.idx[0]];
        break;
      default:
        assert(false);
      }
    }
    else if constexpr (byte_size == 4)
    {
      switch (size)
      {
      case 4:bytes[3] = ptr[ap4.idx[3]];
      case 3:bytes[2] = ptr[ap4.idx[2]];
      case 2:bytes[1] = ptr[ap4.idx[1]];
      case 1:bytes[0] = ptr[ap4.idx[0]];
        break;
      default:
        assert(false);
      }
    }
    else if constexpr (byte_size == 8)
    {
      switch (size)
      {
      case 8:bytes[7] = ptr[ap8.idx[7]];
      case 7:bytes[6] = ptr[ap8.idx[6]];
      case 6:bytes[5] = ptr[ap8.idx[5]];
      case 5:bytes[4] = ptr[ap8.idx[4]];
      case 4:bytes[3] = ptr[ap8.idx[3]];
      case 3:bytes[2] = ptr[ap8.idx[2]];
      case 2:bytes[1] = ptr[ap8.idx[1]];
      case 1:bytes[0] = ptr[ap8.idx[0]];
        break;
      default:
        assert(false);
      }
    }
  }

  template<typename stream_ty, typename ty>
  inline void fix_read_impl(stream_ty& stream, ty& value)
  {
    constexpr size_t byte_size = sizeof(value);
    if constexpr (detail::endian_native_is_little)
    {
      stream.read((char*)&value, byte_size);
    }
    else
    {
      uint8_t bytes[byte_size];
      stream.read(bytes, byte_size);
      uint8_t* ptr = (uint8_t*)&value;
      read_to_byte<byte_size, byte_size>(ptr, bytes);
    }
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, uint16_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, int16_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, uint32_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, int32_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, uint64_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_read(stream_ty& stream, int64_t& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const int8_t& value)
  {
    typedef int8_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      auto ptr = (int8_t*)stream.append_write(1);
      *ptr = value;
    }
    else
    {
      stream.write((char*)&value, sizeof(value_type));
    }
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const uint8_t& value)
  {
    typedef uint8_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      auto ptr = (int8_t*)stream.append_write(1);
      *ptr = value;
    }
    else
    {
      stream.write((char*)&value, sizeof(value_type));
    }
  }

  template<typename stream_ty, typename ty>
  inline void fix_write_impl(stream_ty& stream, const ty& value)
  {
    constexpr size_t byte_size = sizeof(value);
    if constexpr (detail::endian_native_is_little)
    {
      stream.write((char*)&value, byte_size);
    }
    else
    {
      if constexpr (detail::has_appendwrite<stream_ty>::value)
      {
        uint8_t* bytes = stream.append_write(byte_size);
        uint8_t* ptr = (uint8_t*)&value;
        write_to_byte< byte_size, byte_size>(ptr, bytes);
      }
      else
      {
        uint8_t bytes[byte_size];
        uint8_t* ptr = (uint8_t*)&value;
        write_to_byte< byte_size, byte_size>(ptr, bytes);
        stream.write((const char*)bytes, byte_size);
      }
    }
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const uint16_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const int16_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const uint32_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const int32_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const uint64_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void fix_write(stream_ty& stream, const int64_t& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, uint8_t*)
  {
    typedef uint8_t value_type;
    uint8_t value = 0;
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      value = stream.get_char();
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
    else
    {
      const int bytes = sizeof(value_type);
      stream.read((char*)&value, 1);
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, int8_t*)
  {
    typedef int8_t value_type;
    uint8_t value = 0;
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      value = (int8_t)stream.get_char();
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
    else
    {
      const int bytes = sizeof(value_type);
      stream.read((char*)&value, 1);
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
  }

  template<typename stream_ty, typename ty>
  inline void skip_read_impl(stream_ty& stream, ty*)
  {
    typedef ty value_type;
    uint8_t value = 0;
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      value = stream.get_char();
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
    else
    {
      const int bytes = sizeof(value_type);
      stream.read((char*)&value, 1);
      if (value > const_tag_as_value)
      {
        int read_bytes = (value & const_interger_byte_msak) + 1;
        stream.skip_read(read_bytes);
      }
    }
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, uint16_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, int16_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, uint32_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, int32_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, uint64_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, int64_t* arg)
  {
    skip_read_impl(stream, arg);
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, float*)
  {
    typedef float value_type;
    stream.skip_read(sizeof(value_type));
  }

  template<typename stream_ty>
  inline void skip_read(stream_ty& stream, double*)
  {
    typedef double value_type;
    stream.skip_read(sizeof(value_type));
  }

  inline void check_negative_assaigned_to_unsined_interger(uint8_t value)
  {
    if (value & const_negative_bit_value)
    {
      throw exception(negative_assign_to_unsigned_integer_number);
    }
  }

  inline void check_value_too_large_to_integer_number(int bytes, int read_bytes)
  {
    if (bytes < read_bytes)
    {
      throw exception(value_too_large_to_integer_number);
    }
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, uint8_t& value)
  {
    typedef uint8_t value_type;
    constexpr size_t bytes = sizeof(value_type);
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      value = (value_type)stream.get_char();
      if (value > const_tag_as_value)
      {
        check_negative_assaigned_to_unsined_interger(value);
        int read_bytes = (value & const_interger_byte_msak) + 1;
        check_value_too_large_to_integer_number(bytes, read_bytes);
        value = stream.get_char();
      }
    }
    else
    {
      value_type read_value[bytes] = { 0 };
      stream.read((char*)&value, 1);
      if (value > const_tag_as_value)
      {
        check_negative_assaigned_to_unsined_interger(value);
        int read_bytes = (value & const_interger_byte_msak) + 1;
        check_value_too_large_to_integer_number(bytes, read_bytes);
        stream.read((char*)&value, 1);
      }
    }
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, int8_t& value)
  {
    typedef int8_t value_type;
    const int bytes = sizeof(value_type);
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      uint8_t tag = stream.get_char();
      if (tag > const_tag_as_value)
      {
        int sign = 1;
        if (tag & const_negative_bit_value)
        {
          sign = -1;
        }
        int read_bytes = (tag & const_interger_byte_msak) + 1;
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
    else
    {
      value_type read_value[2] = { 0 };
      stream.read((char*)&value, 1);
      if ((uint8_t)value > const_tag_as_value)
      {
        int sign = 1;
        if (value & const_negative_bit_value)
        {
          sign = -1;
        }
        int read_bytes = (value & const_interger_byte_msak) + 1;
        check_value_too_large_to_integer_number(bytes, read_bytes);
        if constexpr (detail::has_getchar<stream_ty>::value)
        {
          read_value[1] = (value_type)stream.get_char();
        }
        else
        {
          stream.read((char*)&read_value[1], 1);
        }

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
  }

  template<typename stream_ty, typename ty>
  inline void read_uint(stream_ty& stream, ty& value)
  {
    typedef ty value_type;
    constexpr size_t bytes = sizeof(value_type);
    uint8_t byte[bytes];
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      byte[0] = (uint8_t)stream.get_char();
    }
    else
    {
      stream.read((char*)byte, 1);
    }
    value = byte[0];
    if (byte[0] > const_tag_as_value)
    {
      check_negative_assaigned_to_unsined_interger(byte[0]);
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
      if constexpr (detail::endian_native_is_little)
      {
        stream.read((char*)&value, read_bytes);
      }
      else
      {
        if constexpr (detail::has_skipread<stream_ty>::value)
        {
          auto sbyte = stream.skip_read(read_bytes);
          uint8_t* ptr = (uint8_t*)&value;
          read_to_byte<bytes>(ptr, sbyte, read_bytes);
        }
        else
        {
          stream.read((char*)byte, read_bytes);
          uint8_t* ptr = (uint8_t*)&value;
          read_to_byte<bytes>(ptr, byte, read_bytes);
        }
      }
    }
  }

  template<typename stream_ty, typename ty>
  inline void read_sint(stream_ty& stream, ty& value)
  {
    typedef ty value_type;
    constexpr size_t bytes = sizeof(value_type);
    uint8_t byte[bytes];
    if constexpr (detail::has_getchar<stream_ty>::value)
    {
      byte[0] = (value_type)stream.get_char();
    }
    else
    {
      stream.read((char*)byte, 1);
    }
    value = byte[0];
    if (byte[0] > const_tag_as_value)
    {
      int sign = 1;
      if (byte[0] & const_negative_bit_value)
      {
        sign = -1;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      check_value_too_large_to_integer_number(bytes, read_bytes);
      value = 0;
      if constexpr (detail::endian_native_is_little)
      {
        stream.read((char*)&value, read_bytes);
      }
      else
      {
        if constexpr (detail::has_skipread<stream_ty>::value)
        {
          auto sbyte = stream.skip_read(read_bytes);
          uint8_t* ptr = (uint8_t*)&value;
          read_to_byte<bytes>(ptr, sbyte, read_bytes);
        }
        else
        {
          stream.read((char*)byte, read_bytes);
          uint8_t* ptr = (uint8_t*)&value;
          read_to_byte<bytes>(ptr, byte, read_bytes);
        }
      }
      if (sign < 0)
      {
        value = -value;
      }
    }
  }


  template<typename stream_ty>
  inline void read(stream_ty& stream, uint16_t& value)
  {
    read_uint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, int16_t& value)
  {
    read_sint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, uint32_t& value)
  {
    read_uint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, int32_t& value)
  {
    read_sint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, uint64_t& value)
  {
    read_uint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, int64_t& value)
  {
    read_sint(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, float& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void read(stream_ty& stream, double& value)
  {
    fix_read_impl(stream, value);
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, uint8_t& value)
  {
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      typedef uint8_t value_type;
      if (value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
        *ptr = value;
      }
      else
      {
        uint8_t* ptr = stream.append_write(2);
        ptr[0] = 0x80;
        ptr[1] = value;
      }
    }
    else
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
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, int8_t& value)
  {
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      typedef int8_t value_type;
      if (0 <= value)
      {
        uint8_t* ptr = stream.append_write(1);
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
        uint8_t* ptr = stream.append_write(2);
        ptr[0] = 0x80 | negative_bit;
        ptr[1] = temp;
      }
    }
    else
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
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, const uint16_t& value)
  {
    typedef uint16_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
        *ptr = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)&value;
        if (value < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80;
          write_to_byte<2, 1>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81;
          write_to_byte<2, 2>(ptr, wptr + 1);
        }
      }
    }
    else
    {
      constexpr size_t byte_size = sizeof(value_type);
      int write_bytes = 0;
      uint8_t bytes[12];
      write_bytes = 1;
      if (value < const_tag_as_type)
      {
        bytes[0] = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)(&value);
        if (value < 0x100)
        {
          write_to_byte<2, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else
        {
          write_to_byte< byte_size, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, int16_t value)
  {
    typedef int16_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (0 <= value && value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80 + negative_bit;
          write_to_byte<2, 1>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81 + negative_bit;
          write_to_byte<2, 2>(ptr, wptr + 1);
        }
      }
    }
    else
    {
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          write_to_byte<2, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else
        {
          write_to_byte<2, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, const uint32_t& value)
  {
    typedef uint32_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
        *ptr = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)&value;
        if (value < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80;
          write_to_byte<4, 1>(ptr, wptr + 1);
        }
        else if (value < 0x10000)
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81;
          write_to_byte<4, 2>(ptr, wptr + 1);
        }
        else if (value < 0x1000000)
        {
          uint8_t* wptr = stream.append_write(4);
          wptr[0] = 0x82;
          write_to_byte<4, 3>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(5);
          wptr[0] = 0x83;
          write_to_byte<4, 4>(ptr, wptr + 1);
        }
      }
    }
    else
    {
      int write_bytes = 0;
      uint8_t bytes[12];
      write_bytes = 1;
      if (value < const_tag_as_type)
      {
        bytes[0] = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)(&value);
        if (value < 0x100)
        {
          write_to_byte<4, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else if (value < 0x10000)
        {
          write_to_byte<4, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        else if (value < 0x1000000)
        {
          write_to_byte<4, 3>(ptr, bytes + 1);
          write_bytes = 4;
        }
        else
        {
          write_to_byte<4, 4>(ptr, bytes + 1);
          write_bytes = 5;
        }
        bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, int32_t value)
  {
    typedef int32_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (0 <= value && value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80 + negative_bit;
          write_to_byte<4, 1>(ptr, wptr + 1);
        }
        else if (temp < 0x10000)
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81 + negative_bit;
          write_to_byte<4, 2>(ptr, wptr + 1);
        }
        else if (temp < 0x1000000)
        {
          uint8_t* wptr = stream.append_write(4);
          wptr[0] = 0x82 + negative_bit;
          write_to_byte<4, 3>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(5);
          wptr[0] = 0x83 + negative_bit;
          write_to_byte<4, 4>(ptr, wptr + 1);
        }
      }
    }
    else
    {
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          write_to_byte<4, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else if (temp < 0x10000)
        {
          write_to_byte<4, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        else if (temp < 0x1000000)
        {
          write_to_byte<4, 3>(ptr, bytes + 1);
          write_bytes = 4;
        }
        else
        {
          write_to_byte<4, 4>(ptr, bytes + 1);
          write_bytes = 5;
        }
        bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, const uint64_t& value)
  {
    typedef uint64_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
        *ptr = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)&value;
        if (value < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80;
          write_to_byte<8, 1>(ptr, wptr + 1);
        }
        else if (value < 0x10000)
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81;
          write_to_byte<8, 2>(ptr, wptr + 1);
        }
        else if (value < 0x1000000)
        {
          uint8_t* wptr = stream.append_write(4);
          wptr[0] = 0x82;
          write_to_byte<8, 3>(ptr, wptr + 1);
        }
        else if (value < 0x100000000)
        {
          uint8_t* wptr = stream.append_write(5);
          wptr[0] = 0x83;
          write_to_byte<8, 4>(ptr, wptr + 1);
        }
        else if (value < 0x10000000000LL)
        {
          uint8_t* wptr = stream.append_write(6);
          wptr[0] = 0x84;
          write_to_byte<8, 5>(ptr, wptr + 1);
        }
        else if (value < 0x1000000000000LL)
        {
          uint8_t* wptr = stream.append_write(7);
          wptr[0] = 0x85;
          write_to_byte<8, 6>(ptr, wptr + 1);
        }
        else if (value < 0x100000000000000LL)
        {
          uint8_t* wptr = stream.append_write(8);
          wptr[0] = 0x86;
          write_to_byte<8, 7>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(9);
          wptr[0] = 0x87;
          write_to_byte<8, 8>(ptr, wptr + 1);
        }
      }
    }
    else
    {
      int write_bytes = 0;
      uint8_t bytes[12];
      write_bytes = 1;
      if (value < const_tag_as_type)
      {
        bytes[0] = (uint8_t)value;
      }
      else
      {
        uint8_t* ptr = (uint8_t*)(&value);
        if (value < 0x100)
        {
          write_to_byte<8, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else if (value < 0x10000)
        {
          write_to_byte<8, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        else if (value < 0x1000000)
        {
          write_to_byte<8, 3>(ptr, bytes + 1);
          write_bytes = 4;
        }
        else if (value < 0x100000000)
        {
          write_to_byte<8, 4>(ptr, bytes + 1);
          write_bytes = 5;
        }
        else if (value < 0x10000000000LL)
        {
          write_to_byte<8, 5>(ptr, bytes + 1);
          write_bytes = 6;
        }
        else if (value < 0x1000000000000LL)
        {
          write_to_byte<8, 6>(ptr, bytes + 1);
          write_bytes = 7;
        }
        else if (value < 0x100000000000000LL)
        {
          write_to_byte<8, 7>(ptr, bytes + 1);
          write_bytes = 8;
        }
        else
        {
          write_to_byte<8, 8>(ptr, bytes + 1);
          write_bytes = 9;
        }
        bytes[0] = (uint8_t)(const_store_postive_integer_byte_mask + write_bytes);
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, int64_t value)
  {
    typedef int64_t value_type;
    if constexpr (detail::has_appendwrite<stream_ty>::value)
    {
      if (0 <= value && value < const_tag_as_type)
      {
        uint8_t* ptr = stream.append_write(1);
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          uint8_t* wptr = stream.append_write(2);
          wptr[0] = 0x80 + negative_bit;
          write_to_byte<8, 1>(ptr, wptr + 1);
        }
        else if (temp < 0x10000)
        {
          uint8_t* wptr = stream.append_write(3);
          wptr[0] = 0x81 + negative_bit;
          write_to_byte<8, 2>(ptr, wptr + 1);
        }
        else if (temp < 0x1000000)
        {
          uint8_t* wptr = stream.append_write(4);
          wptr[0] = 0x82 + negative_bit;
          write_to_byte<8, 3>(ptr, wptr + 1);
        }
        else if (temp < 0x100000000)
        {
          uint8_t* wptr = stream.append_write(5);
          wptr[0] = 0x83 + negative_bit;
          write_to_byte<8, 4>(ptr, wptr + 1);
        }
        else if (temp < 0x10000000000LL)
        {
          uint8_t* wptr = stream.append_write(6);
          wptr[0] = 0x84 + negative_bit;
          write_to_byte<8, 5>(ptr, wptr + 1);
        }
        else if (temp < 0x1000000000000LL)
        {
          uint8_t* wptr = stream.append_write(7);
          wptr[0] = 0x85 + negative_bit;
          write_to_byte<8, 6>(ptr, wptr + 1);
        }
        else if (temp < 0x100000000000000LL)
        {
          uint8_t* wptr = stream.append_write(8);
          wptr[0] = 0x86 + negative_bit;
          write_to_byte<8, 7>(ptr, wptr + 1);
        }
        else
        {
          uint8_t* wptr = stream.append_write(9);
          wptr[0] = 0x87 + negative_bit;
          write_to_byte<8, 8>(ptr, wptr + 1);
        }
      }
    }
    else
    {
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
        uint8_t* ptr = (uint8_t*)&temp;
        if (temp < 0x100)
        {
          write_to_byte<8, 1>(ptr, bytes + 1);
          write_bytes = 2;
        }
        else if (temp < 0x10000)
        {
          write_to_byte<8, 2>(ptr, bytes + 1);
          write_bytes = 3;
        }
        else if (temp < 0x1000000)
        {
          write_to_byte<8, 3>(ptr, bytes + 1);
          write_bytes = 4;
        }
        else if (temp < 0x100000000)
        {
          write_to_byte<8, 4>(ptr, bytes + 1);
          write_bytes = 5;
        }
        else if (temp < 0x10000000000LL)
        {
          write_to_byte<8, 5>(ptr, bytes + 1);
          write_bytes = 6;
        }
        else if (temp < 0x1000000000000LL)
        {
          write_to_byte<8, 6>(ptr, bytes + 1);
          write_bytes = 7;
        }
        else if (temp < 0x100000000000000LL)
        {
          write_to_byte<8, 7>(ptr, bytes + 1);
          write_bytes = 8;
        }
        else
        {
          write_to_byte<8, 8>(ptr, bytes + 1);
          write_bytes = 9;
        }
        bytes[0] = const_store_postive_integer_byte_mask + negative_bit + write_bytes;
      }
      stream.write((char*)bytes, write_bytes);
    }
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, const float& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename stream_ty>
  inline void write(stream_ty& stream, const double& value)
  {
    fix_write_impl(stream, value);
  }

  template<typename alloc_type>
  inline int32_t size_of(std::basic_string<char, std::char_traits<char>, alloc_type> const& str)
  {
    int32_t len = (int32_t)str.length();
    len += size_of(len);
    return len;
  }

  template<typename stream_ty, typename alloc_type>
  inline void read(stream_ty& stream, std::basic_string<char, std::char_traits<char>, alloc_type>& str)
  {
    int32_t len;
    read(stream, len);
    str.resize(len);
    stream.read((char*)str.data(), len);
  }

  template<typename stream_ty, typename alloc_type>
  inline void write(stream_ty& stream, std::basic_string<char, std::char_traits<char>, alloc_type> const& str)
  {
    int32_t len = (int32_t)str.length();
    write(stream, len);
    stream.write(str.data(), len);
  }

#ifndef MAX_ADATA_LEN 
# define MAX_ADATA_LEN 65535
#endif

  template<typename stream_ty>
  inline int32_t check_read_size(stream_ty& stream, int size = 0)
  {
    int32_t len;
    read(stream, len);
    if (size > 0 && len > size)
    {
      throw exception(number_of_element_not_match);
    }
    else if (len > MAX_ADATA_LEN)
    {
      throw exception(number_of_element_not_match);
    }
    return len;
  }

  template <typename stream_ty>
  inline void skip_read_compatible(stream_ty& stream)
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

  template<typename stream_ty, typename ty>
  inline void read_ec(stream_ty& stream, ty& value, error_code_t& ec)
  {
    try
    {
      read(stream, value);
    }
    catch (exception& e)
    {
      ec = e.error_code();
    }
  }

  template<typename stream_ty, typename ty>
  inline void fix_read_ec(stream_ty& stream, ty& value, error_code_t& ec)
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
  inline void write_ec(stream_ty& stream, ty& value, error_code_t& ec)
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
  inline void fix_write_ec(stream_ty& stream, ty& value, error_code_t& ec)
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

