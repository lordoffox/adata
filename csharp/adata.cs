// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace adata
{
  [StructLayout(LayoutKind.Explicit)]
  public struct UType
  {
    [FieldOffset(0)]
    public UInt16 VU16;
    [FieldOffset(0)]
    public Int16 VI16;
    [FieldOffset(0)]
    public UInt32 VU32;
    [FieldOffset(0)]
    public Int32 VI32;
    [FieldOffset(0)]
    public UInt64 VU64;
    [FieldOffset(0)]
    public Int64 VI64;
    [FieldOffset(0)]
    public float VF32;
    [FieldOffset(0)]
    public double VF64;
    [FieldOffset(0)]
    public byte Byte0;
    [FieldOffset(1)]
    public byte Byte1;
    [FieldOffset(2)]
    public byte Byte2;
    [FieldOffset(3)]
    public byte Byte3;
    [FieldOffset(4)]
    public byte Byte4;
    [FieldOffset(5)]
    public byte Byte5;
    [FieldOffset(6)]
    public byte Byte6;
    [FieldOffset(7)]
    public byte Byte7;
  }

  public enum error_code_t
  {
    success,
    negative_assign_to_unsigned_integer_number,
    value_too_large_to_integer_number,
    sequence_length_overflow,
    stream_buffer_overflow,
    number_of_element_not_macth,
    undefined_member_protocol_not_compatible,
  }

  public class exception : SystemException
  {
    error_code_t m_error;
    public error_code_t ErrorCode
    {
      get { return m_error; }
    }
    public exception(error_code_t ec):base(to_message(ec))
    {
      m_error = ec;
    }
    static public string to_message(error_code_t ec)
    {
      switch(ec)
      {
        case error_code_t.negative_assign_to_unsigned_integer_number: return "assign negative value to unsigned.";
        case error_code_t.value_too_large_to_integer_number:return "integer value out of range.";
        case error_code_t.sequence_length_overflow:return "size out of range.";
        case error_code_t.stream_buffer_overflow:return "buffer overflow.";
        case error_code_t.number_of_element_not_macth:return "length too large.";
        case error_code_t.undefined_member_protocol_not_compatible:return "undefined member, protocol not compatible.";
        default:
          return "";
      }
    }
  };

  public class zero_copy_buffer
  {
    public byte[] buffer;
    public int data_len;
    public int read_len;
    public int write_len;
    internal UType value;
    public error_code_t error_code;
    public zero_copy_buffer()
    {
      buffer = new byte[65536];
      data_len = 65536;
      read_len = 0;
      write_len = 0;
      value = new UType();
      error_code = error_code_t.success;
    }

    public zero_copy_buffer(byte[] buf , int size = -1)
    {
      buffer = buf;
      if(buf != null)
      {
        if (buf.Length < size)
        {
          size = buf.Length;
        }
        else if (size == -1)
        {
          size = buf.Length;
        }
      }
      else
      {
        size = 0;
      }
      data_len = size;
      read_len = 0;
      write_len = 0;
      value = new UType();
      error_code = error_code_t.success;
    }
    public void reset(byte[] the_buffer , int size = -1)
    {
      if(size == -1)
      {
        size = the_buffer.Length;
      }
      this.buffer = the_buffer;
      this.data_len = the_buffer.Length;
      this.read_len = 0;
      this.write_len = 0;
    }

    public void clear()
    {
      this.read_len = 0;
      this.write_len = 0;
      error_code = error_code_t.success;
    }

    public void trace_error(string tag)
    {
    }

    public void trace_error(string tag, int sub)
    {
    }

    public string get_trace_info()
    {
      return "";
    }

    public error_code_t get_error_code() { return error_code; }
    public void set_error_code(error_code_t ec) { error_code = ec; }

    public bool error() { return error_code != error_code_t.success; }

    public void skip_read(UInt32 len)
    {
      if (read_len + len > data_len)
      {
        error_code = error_code_t.stream_buffer_overflow;
        throw new exception(error_code);
      }
      read_len += (int)len;
    }

    public int read_length()
    {
      return read_len;
    }

    public int write_length()
    {
      return write_len;
    }
  }

  public class stream
  {
    private const byte const_tag_as_value = 0x7f;
    private const byte const_tag_as_type = 0x80;
    private const byte const_interger_byte_msak = 0x1f;
    private const byte const_negative_bit_value = 0x20;
    private const byte const_store_postive_integer_byte_mask = 0x80 - 2;

    private static bool IsLittleEndian = BitConverter.IsLittleEndian;

    public static void fix_skip_read(zero_copy_buffer stream, ref sbyte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 1;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref byte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 1;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref Int16 value)
    {
      if (stream.read_len + 2 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 2;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref UInt16 value)
    {
      if (stream.read_len + 2 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 2;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref Int32 value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 4;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref UInt32 value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 4;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref Int64 value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 8;
    }

    public static void fix_skip_read(zero_copy_buffer stream, ref UInt64 value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 8;
    }

    public static Int32 fix_size_of(sbyte value)
    {
      return 1;
    }

    public static Int32 fix_size_of(byte value)
    {
      return 1;
    }

    public static Int32 fix_size_of(Int16 value)
    {
      return 2;
    }

    public static Int32 fix_size_of(UInt16 value)
    {
      return 2;
    }

    public static Int32 fix_size_of(Int32 value)
    {
      return 4;
    }

    public static Int32 fix_size_of(UInt32 value)
    {
      return 4;
    }

    public static Int32 fix_size_of(Int64 value)
    {
      return 8;
    }

    public static Int32 fix_size_of(UInt64 value)
    {
      return 8;
    }

    public static void skip_read(zero_copy_buffer stream, ref sbyte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref byte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref Int16 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref UInt16 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref Int32 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref UInt32 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref Int64 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref UInt64 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += read_bytes;
    }

    public static void skip_read(zero_copy_buffer stream, ref float value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 4;
    }

    public static void skip_read(zero_copy_buffer stream, ref double value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += 8;
    }

    public static UInt32 size_of(sbyte value)
    {
      if ((value & const_tag_as_type) > 0) return 2;
      else return 1;
    }

    public static Int32 size_of(byte value)
    {
      if (value < const_tag_as_type) return 1;
      else return 2;
    }

    public static Int32 size_of(UInt16 value)
    {
      if (value < const_tag_as_type) return 1;
      else if (value < 0x100) return 2;
      return 3;
    }

    public static Int32 size_of(Int16 value)
    {
      if (0 <= value && value < const_tag_as_type) return 1;
      if (value < 0) value = (Int16)(-value);
      if (value < 0x100) return 2;
      return 3;
    }

    public static Int32 size_of(UInt32 value)
    {
      if (value < const_tag_as_type) return 1;
      else if (value < 0x100) return 2;
      else if (value < 0x10000) return 3;
      else if (value < 0x1000000) return 4;
      return 5;
    }

    public static Int32 size_of(Int32 value)
    {
      if (0 <= value && value < const_tag_as_type) return 1;
      if (value < 0) value = (Int32)(-value);
      if (value < 0x100) return 2;
      if (value < 0x10000) return 3;
      if (value < 0x1000000) return 4;
      return 5;
    }

    public static Int32 size_of(UInt64 value)
    {
      if (value < const_tag_as_type) return 1;
      else if (value < 0x100) return 2;
      else if (value < 0x10000) return 3;
      else if (value < 0x1000000) return 4;
      else if (value < 0x100000000UL) return 5;
      else if (value < 0x10000000000UL) return 6;
      else if (value < 0x1000000000000UL) return 7;
      else if (value < 0x100000000000000UL) return 8;
      return 9;
    }

    public static Int32 size_of(Int64 value)
    {
      if (0 <= value && value < const_tag_as_type) return 1;
      if (value < 0) value = (Int64)(-value);
      if (value < 0x100) return 2;
      else if (value < 0x10000) return 3;
      else if (value < 0x1000000) return 4;
      else if (value < 0x100000000L) return 5;
      else if (value < 0x10000000000L) return 6;
      else if (value < 0x1000000000000L) return 7;
      else if (value < 0x100000000000000L) return 8;
      return 9;
    }

    public static Int32 size_of(float value)
    {
      return 4;
    }

    public static Int32 size_of(double value)
    {
      return 8;
    }

    public static Int32 size_of(string value)
    {
      Int32 len = System.Text.Encoding.UTF8.GetByteCount(value);
      len += adata.stream.size_of(len);
      return len;
    }

    public static void fix_read(zero_copy_buffer stream, ref sbyte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (sbyte)stream.buffer[stream.read_len++];
    }

    public static void fix_read(zero_copy_buffer stream, ref byte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = stream.buffer[stream.read_len++];
    }

    public static void fix_read(zero_copy_buffer stream, ref Int16 value)
    {
      if (stream.read_len + 2 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VI16;
    }

    public static void fix_read(zero_copy_buffer stream, ref UInt16 value)
    {
      if (stream.read_len + 2 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VU16;
    }

    public static void fix_read(zero_copy_buffer stream, ref Int32 value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VI32;
    }

    public static void fix_read(zero_copy_buffer stream, ref UInt32 value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VU32;
    }

    public static void fix_read(zero_copy_buffer stream, ref Int64 value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte7 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte7 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VI64;
    }

    public static void fix_read(zero_copy_buffer stream, ref UInt64 value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte7 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte7 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VU64;
    }

    public static void read(zero_copy_buffer stream, ref float value)
    {
      if (stream.read_len + 4 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VF32;
    }

    public static void read(zero_copy_buffer stream, ref double value)
    {
      if (stream.read_len + 8 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      if (IsLittleEndian)
      {
        stream.value.Byte0 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte7 = stream.buffer[stream.read_len++];
      }
      else
      {
        stream.value.Byte7 = stream.buffer[stream.read_len++];
        stream.value.Byte6 = stream.buffer[stream.read_len++];
        stream.value.Byte5 = stream.buffer[stream.read_len++];
        stream.value.Byte4 = stream.buffer[stream.read_len++];
        stream.value.Byte3 = stream.buffer[stream.read_len++];
        stream.value.Byte2 = stream.buffer[stream.read_len++];
        stream.value.Byte1 = stream.buffer[stream.read_len++];
        stream.value.Byte0 = stream.buffer[stream.read_len++];
      }
      value = stream.value.VF64;
    }

    public static void fix_write(zero_copy_buffer stream, sbyte value)
    {
      if (stream.write_len + 1 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.buffer[stream.write_len++] = (byte)value;
    }

    public static void fix_write(zero_copy_buffer stream, byte value)
    {
      if (stream.write_len + 1 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.buffer[stream.write_len++] = (byte)value;
    }

    public static void fix_write(zero_copy_buffer stream, Int16 value)
    {
      if (stream.write_len + 2 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VI16 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
      }
    }

    public static void fix_write(zero_copy_buffer stream, UInt16 value)
    {
      if (stream.write_len + 2 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VU16 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
      }
    }

    public static void fix_write(zero_copy_buffer stream, Int32 value)
    {
      if (stream.write_len + 4 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VI32 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void fix_write(zero_copy_buffer stream, UInt32 value)
    {
      if (stream.write_len + 4 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VU32 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void fix_write(zero_copy_buffer stream, Int64 value)
    {
      if (stream.write_len + 8 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VI64 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void fix_write(zero_copy_buffer stream, UInt64 value)
    {
      if (stream.write_len + 8 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VU64 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void write(zero_copy_buffer stream, float value)
    {
      if (stream.write_len + 4 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VF32 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void write(zero_copy_buffer stream, double value)
    {
      if (stream.write_len + 8 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }

      stream.value.VF64 = value;
      if (IsLittleEndian)
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
      else
      {
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte3;
      }
    }

    public static void read(zero_copy_buffer stream, ref sbyte value)
    {
      if (stream.read_len + 1 >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (sbyte)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      bool signed = (value & const_negative_bit_value) > 0;
      if (stream.read_len + read_bytes >= stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (sbyte)stream.buffer[stream.read_len++];
      if (signed)
      {
        value = (sbyte)-value;
      }
    }

    public static void read(zero_copy_buffer stream, ref byte value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = stream.buffer[stream.read_len++];
    }

    public static void read(zero_copy_buffer stream, ref Int16 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (Int16)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      bool signed = (value & const_negative_bit_value) > 0;
      stream.value.VU64 = 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      if (signed)
      {
        value = (Int16)(-stream.value.VI16);
      }
      else
      {
        value = stream.value.VI16;
      }
    }

    public static void read(zero_copy_buffer stream, ref UInt16 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (UInt16)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.value.VU64 = 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      value = stream.value.VU16;
    }

    public static void read(zero_copy_buffer stream, ref Int32 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (Int32)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      bool signed = (value & const_negative_bit_value) > 0;
      stream.value.VU64 = 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      if (signed)
      {
        value = -stream.value.VI32;
      }
      else
      {
        value = stream.value.VI32;
      }
    }

    public static void read(zero_copy_buffer stream, ref UInt32 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (UInt32)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      UInt32 read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.value.VU64 = 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      value = stream.value.VU32;
    }

    public static void read(zero_copy_buffer stream, ref Int64 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (Int64)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.value.VU64 = 0;
      bool signed = (value & const_negative_bit_value) > 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              break;
            }
          case 8:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 8:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      if (signed)
      {
        value = -stream.value.VI64;
      }
      else
      {
        value = stream.value.VI64;
      }
    }

    public static void read(zero_copy_buffer stream, ref UInt64 value)
    {
      if (stream.read_len + 1 > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = (UInt64)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.error_code = error_code_t.value_too_large_to_integer_number;
        throw new exception(stream.error_code);
      }
      if (stream.read_len + read_bytes > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.value.VU64 = 0;
      if (IsLittleEndian)
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              break;
            }
          case 8:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      else
      {
        switch (read_bytes)
        {
          case 1:
            {
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
          case 8:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
              stream.value.Byte0 = stream.buffer[stream.read_len++];
              break;
            }
        }
      }
      value = stream.value.VU64;
    }

    public static void write(zero_copy_buffer stream, sbyte value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          value = (sbyte)(-value);
        }
        if (stream.write_len + 2 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = tag;
        stream.buffer[stream.write_len++] = (byte)value;
      }
    }

    public static void write(zero_copy_buffer stream, byte value)
    {
      if (value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = value;
      }
      else
      {
        byte tag = const_tag_as_type;
        if (stream.write_len + 2 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = tag;
        stream.buffer[stream.write_len++] = value;
      }
    }

    public static void write(zero_copy_buffer stream, Int16 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          value = (Int16)(-value);
        }
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI16 = value;
          ++tag;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static void write(zero_copy_buffer stream, UInt16 value)
    {
      if (value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU16 = value;
          stream.buffer[stream.write_len++] = 0x81;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static void write(zero_copy_buffer stream, Int32 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          value = -value;
        }
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI32 = value;
          ++tag;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000)
        {
          if (stream.write_len + 4 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI32 = value;
          tag += 2;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else
        {
          if (stream.write_len + 5 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI32 = value;
          tag += 3;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static void write(zero_copy_buffer stream, UInt32 value)
    {
      if (value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU32 = value;
          stream.buffer[stream.write_len++] = 0x81;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000)
        {
          if (stream.write_len + 4 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU32 = value;
          stream.buffer[stream.write_len++] = 0x82;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else
        {
          if (stream.write_len + 5 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU32 = value;
          stream.buffer[stream.write_len++] = 0x83;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static void write(zero_copy_buffer stream, Int64 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          value = -value;
        }
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          ++tag;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000)
        {
          if (stream.write_len + 4 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 2;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x100000000L)
        {
          if (stream.write_len + 5 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 3;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x10000000000L)
        {
          if (stream.write_len + 6 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 4;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000000000L)
        {
          if (stream.write_len + 7 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 5;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x100000000000000L)
        {
          if (stream.write_len + 8 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 6;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else
        {
          if (stream.write_len + 9 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VI64 = value;
          tag += 7;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte7;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static void write(zero_copy_buffer stream, UInt64 value)
    {
      if (value <= const_tag_as_value)
      {
        if (stream.write_len + 1 >= stream.data_len)
        {
          stream.error_code = error_code_t.stream_buffer_overflow;
          throw new exception(stream.error_code);
        }
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          if (stream.write_len + 2 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          if (stream.write_len + 3 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x81;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000)
        {
          if (stream.write_len + 4 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x82;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x100000000UL)
        {
          if (stream.write_len + 5 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x83;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x10000000000UL)
        {
          if (stream.write_len + 6 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x84;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x1000000000000UL)
        {
          if (stream.write_len + 7 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x85;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else if (value < 0x100000000000000UL)
        {
          if (stream.write_len + 8 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x86;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
        else
        {
          if (stream.write_len + 9 >= stream.data_len)
          {
            stream.error_code = error_code_t.stream_buffer_overflow;
            throw new exception(stream.error_code);
          }
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x87;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte7;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
            stream.buffer[stream.write_len++] = stream.value.Byte0;
          }
        }
      }
    }

    public static Int32 check_read_size(zero_copy_buffer stream)
    {
      return check_read_size(stream, 0);
    }

    public static Int32 check_read_size(zero_copy_buffer stream, int size)
    {
      Int32 len = 0;
      read(stream, ref len);
      if (size > 0 && len > size)
      {
        stream.error_code = error_code_t.number_of_element_not_macth;
        throw new exception(stream.error_code);
      }
      return len;
    }

    public static void read(zero_copy_buffer stream, ref string value, Int32 len)
    {
      if (stream.read_len + len > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      value = Encoding.UTF8.GetString(stream.buffer, stream.read_len, (int)len);
      stream.read_len += (int)len;
    }

    public static void write(zero_copy_buffer stream, string value, Int32 len)
    {
      if (stream.write_len + len > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      byte[] buff = Encoding.UTF8.GetBytes(value);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void read(zero_copy_buffer stream, ref string value)
    {
      Int32 len = 0;
      read(stream, ref len);
      read(stream, ref value, len);
    }

    public static void write(zero_copy_buffer stream, string value)
    {
      byte[] buff = Encoding.UTF8.GetBytes(value);

      Int32 len = buff.Length;
      if (stream.write_len + len > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      write(stream, len);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void skip_read(zero_copy_buffer stream, ref string value, UInt32 len)
    {
      if (stream.read_len + len > stream.data_len)
      {
        stream.error_code = error_code_t.stream_buffer_overflow;
        throw new exception(stream.error_code);
      }
      stream.read_len += (int)len;
    }

    public static void skip_read_compatible(zero_copy_buffer stream)
    {
      int offset = stream.read_len;
      Int64 tag = 0;
      read(stream, ref tag);
      Int32 len_tag = 0;
      read(stream, ref len_tag);
      Int32 read_len = stream.read_len - offset;
      if (len_tag > read_len) stream.skip_read((UInt32)(len_tag - read_len));
    }
  }

  public abstract class base_obj
  {
    public abstract void read(adata.zero_copy_buffer stream);
    public abstract Int32 size_of();
    public abstract void write(adata.zero_copy_buffer stream);
    public abstract void raw_read(adata.zero_copy_buffer stream);
    public abstract Int32 raw_size_of();
    public abstract void raw_write(adata.zero_copy_buffer stream);

    public void skip_read(adata.zero_copy_buffer stream)
    {
      adata.stream.skip_read_compatible(stream);
    }

  }
}
