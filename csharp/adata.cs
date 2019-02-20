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

  public enum EErrorCode
  {
    success,
    negative_assign_to_unsigned_integer_number,
    value_too_large_to_integer_number,
    sequence_length_overflow,
    stream_buffer_overflow,
    number_of_element_not_macth,
    undefined_member_protocol_not_compatible,
  }

  public class ADataException : SystemException
  {
    public readonly EErrorCode m_error;
    public ADataException(EErrorCode ec) : base(ToMessage(ec))
    {
      m_error = ec;
    }
    static public string ToMessage(EErrorCode ec)
    {
      switch (ec)
      {
        case EErrorCode.negative_assign_to_unsigned_integer_number: return "assign negative value to unsigned.";
        case EErrorCode.value_too_large_to_integer_number: return "integer value out of range.";
        case EErrorCode.sequence_length_overflow: return "size out of range.";
        case EErrorCode.stream_buffer_overflow: return "buffer overflow.";
        case EErrorCode.number_of_element_not_macth: return "length too large.";
        case EErrorCode.undefined_member_protocol_not_compatible: return "undefined member, protocol not compatible.";
        default:
          return "";
      }
    }
  };

  public class ZeroCopyBuffer
  {
    public byte[] buffer;
    public int data_len;
    public int read_len;
    public int write_len;
    internal UType value;

    EErrorCode error_code;
    public ZeroCopyBuffer()
    {
      buffer = new byte[65536];
      data_len = 65536;
      read_len = 0;
      write_len = 0;
      value = new UType();
      error_code = EErrorCode.success;
    }

    public ZeroCopyBuffer(byte[] buf, int size = -1)
    {
      buffer = buf;
      if (buf != null)
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
      error_code = EErrorCode.success;
    }
    public void Reset(byte[] the_buffer, int size = -1)
    {
      if (size == -1)
      {
        size = the_buffer.Length;
      }
      this.buffer = the_buffer;
      this.data_len = size;
      this.read_len = 0;
      this.write_len = 0;
    }

    public void Clear()
    {
      this.read_len = 0;
      this.write_len = 0;
      error_code = EErrorCode.success;
    }

    public EErrorCode ErrorCode { get => error_code; }

    public void RaiseErrorCode(EErrorCode ec)
    {
      error_code = ec;
      throw new ADataException(error_code);
    }

    public bool Error { get => error_code != EErrorCode.success; }

    public void CheckReadBuffer(Int32 len)
    {
      if (read_len + len > data_len)
      {
        RaiseErrorCode(EErrorCode.stream_buffer_overflow);
      }
    }

    public void CheckWriteBuffer(Int32 len)
    {
      if (write_len + len > data_len)
      {
        RaiseErrorCode(EErrorCode.stream_buffer_overflow);
      }
    }

    public void SkipRead(Int32 len)
    {
      CheckReadBuffer(len);
      read_len += (int)len;
    }

    public int ReadLength()
    {
      return read_len;
    }

    public int WriteLength()
    {
      return write_len;
    }
  }

  public class Stream
  {
    private const byte const_tag_as_value = 0x7f;
    private const byte const_tag_as_type = 0x80;
    private const byte const_interger_byte_msak = 0x1f;
    private const byte const_negative_bit_value = 0x20;
    private const byte const_store_postive_integer_byte_mask = 0x80 - 2;

    private static readonly bool IsLittleEndian = BitConverter.IsLittleEndian;

    public static void SkipBytes(ZeroCopyBuffer stream, int count)
    {
      stream.SkipRead(count);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref sbyte value)
    {
      SkipBytes(stream, 1);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref byte value)
    {
      SkipBytes(stream, 1);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref Int16 value)
    {
      SkipBytes(stream, 2);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref UInt16 value)
    {
      SkipBytes(stream, 2);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref Int32 value)
    {
      SkipBytes(stream, 4);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref UInt32 value)
    {
      SkipBytes(stream, 4);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref Int64 value)
    {
      SkipBytes(stream, 8);
    }

    public static void FixSkipRead(ZeroCopyBuffer stream, ref UInt64 value)
    {
      SkipBytes(stream, 8);
    }

    public static Int32 FixSizeOf(sbyte value)
    {
      return 1;
    }

    public static Int32 FixSizeOf(byte value)
    {
      return 1;
    }

    public static Int32 FixSizeOf(Int16 value)
    {
      return 2;
    }

    public static Int32 FixSizeOf(UInt16 value)
    {
      return 2;
    }

    public static Int32 FixSizeOf(Int32 value)
    {
      return 4;
    }

    public static Int32 FixSizeOf(UInt32 value)
    {
      return 4;
    }

    public static Int32 FixSizeOf(Int64 value)
    {
      return 8;
    }

    public static Int32 FixSizeOf(UInt64 value)
    {
      return 8;
    }

    public static void SkipReadValueImpl(ZeroCopyBuffer stream, int max_bytes)
    {
      stream.CheckReadBuffer(1);
      byte tag = stream.buffer[stream.read_len++];
      if (tag <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(tag & const_interger_byte_msak) + 1;
      if (read_bytes > max_bytes)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
      stream.read_len += read_bytes;
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref sbyte value)
    {
      SkipReadValueImpl(stream, 1);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref byte value)
    {
      SkipReadValueImpl(stream, 1);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref Int16 value)
    {
      SkipReadValueImpl(stream, 2);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref UInt16 value)
    {
      SkipReadValueImpl(stream, 2);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref Int32 value)
    {
      SkipReadValueImpl(stream, 4);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref UInt32 value)
    {
      SkipReadValueImpl(stream, 4);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref Int64 value)
    {
      SkipReadValueImpl(stream, 8);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref UInt64 value)
    {
      SkipReadValueImpl(stream, 8);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref float value)
    {
      SkipBytes(stream, 4);
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref double value)
    {
      SkipBytes(stream, 8);
    }

    public static UInt32 SizeOf(sbyte value)
    {
      if ((value & const_tag_as_type) > 0)
        return 2;
      else return 1;
    }

    public static Int32 SizeOf(byte value)
    {
      if (value < const_tag_as_type)
        return 1;
      else return 2;
    }

    public static Int32 SizeOf(UInt16 value)
    {
      if (value < const_tag_as_type)
        return 1;
      else if (value < 0x100)
        return 2;
      return 3;
    }

    public static Int32 SizeOf(Int16 value)
    {
      if (0 <= value && value < const_tag_as_type)
        return 1;
      UInt16 temp = (UInt16)value;
      if (value < 0) temp = (UInt16)(-value);
      if (temp < 0x100)
        return 2;
      return 3;
    }

    public static Int32 SizeOf(UInt32 value)
    {
      if (value < const_tag_as_type)
        return 1;
      else if (value < 0x100)
        return 2;
      else if (value < 0x10000)
        return 3;
      else if (value < 0x1000000)
        return 4;
      return 5;
    }

    public static Int32 SizeOf(Int32 value)
    {
      if (0 <= value && value < const_tag_as_type)
        return 1;
      UInt32 temp = (UInt32)value;
      if (value < 0) temp = (UInt32)(-value);
      if (temp < 0x100)
        return 2;
      if (temp < 0x10000)
        return 3;
      if (temp < 0x1000000)
        return 4;
      return 5;
    }

    public static Int32 SizeOf(UInt64 value)
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

    public static Int32 SizeOf(Int64 value)
    {
      if (0 <= value && value < const_tag_as_type) return 1;
      UInt64 temp = (UInt64)value;
      if (value < 0) temp = (UInt64)(-value);
      if (temp < 0x100) return 2;
      else if (temp < 0x10000) return 3;
      else if (temp < 0x1000000) return 4;
      else if (temp < 0x100000000L) return 5;
      else if (temp < 0x10000000000L) return 6;
      else if (temp < 0x1000000000000L) return 7;
      else if (temp < 0x100000000000000L) return 8;
      return 9;
    }

    public static Int32 SizeOf(float value)
    {
      return 4;
    }

    public static Int32 SizeOf(double value)
    {
      return 8;
    }

    public static Int32 SizeOf(string value)
    {
      Int32 len = System.Text.Encoding.UTF8.GetByteCount(value);
      len += adata.Stream.SizeOf(len);
      return len;
    }

    public static Int32 SizeOf(byte[] value)
    {
      Int32 len = value.Length;
      len += adata.Stream.SizeOf(len);
      return len;
    }

    public static void FixRead(ZeroCopyBuffer stream, ref sbyte value)
    {
      stream.CheckReadBuffer(1);
      value = (sbyte)stream.buffer[stream.read_len++];
    }

    public static void FixRead(ZeroCopyBuffer stream, ref byte value)
    {
      stream.CheckReadBuffer(1);
      value = stream.buffer[stream.read_len++];
    }

    public static void FixRead(ZeroCopyBuffer stream, ref Int16 value)
    {
      stream.CheckReadBuffer(2);
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

    public static void FixRead(ZeroCopyBuffer stream, ref UInt16 value)
    {
      stream.CheckReadBuffer(2);
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

    public static void FixRead(ZeroCopyBuffer stream, ref Int32 value)
    {
      stream.CheckReadBuffer(4);
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

    public static void FixRead(ZeroCopyBuffer stream, ref UInt32 value)
    {
      stream.CheckReadBuffer(4);
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

    public static void FixRead(ZeroCopyBuffer stream, ref Int64 value)
    {
      stream.CheckReadBuffer(8);
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

    public static void FixRead(ZeroCopyBuffer stream, ref UInt64 value)
    {
      stream.CheckReadBuffer(8);
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

    public static void Read(ZeroCopyBuffer stream, ref float value)
    {
      stream.CheckReadBuffer(4);
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

    public static void Read(ZeroCopyBuffer stream, ref double value)
    {
      stream.CheckReadBuffer(8);
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

    public static void FixWrite(ZeroCopyBuffer stream, sbyte value)
    {
      stream.CheckWriteBuffer(1);
      stream.buffer[stream.write_len++] = (byte)value;
    }

    public static void FixWrite(ZeroCopyBuffer stream, byte value)
    {
      stream.CheckWriteBuffer(1);
      stream.buffer[stream.write_len++] = (byte)value;
    }

    public static void FixWrite(ZeroCopyBuffer stream, Int16 value)
    {
      stream.CheckWriteBuffer(2);
      stream.value.VI16 = value;
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

    public static void FixWrite(ZeroCopyBuffer stream, UInt16 value)
    {
      stream.CheckWriteBuffer(2);
      stream.value.VU16 = value;
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

    public static void FixWrite(ZeroCopyBuffer stream, Int32 value)
    {
      stream.CheckWriteBuffer(4);
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
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
      }
    }

    public static void FixWrite(ZeroCopyBuffer stream, UInt32 value)
    {
      stream.CheckWriteBuffer(4);
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
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
      }
    }

    public static void FixWrite(ZeroCopyBuffer stream, Int64 value)
    {
      stream.CheckWriteBuffer(8);
      stream.value.VI64 = value;
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

    public static void FixWrite(ZeroCopyBuffer stream, UInt64 value)
    {
      stream.CheckWriteBuffer(8);
      stream.value.VU64 = value;
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

    public static void Write(ZeroCopyBuffer stream, float value)
    {
      stream.CheckWriteBuffer(4);
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
        stream.buffer[stream.write_len++] = stream.value.Byte3;
        stream.buffer[stream.write_len++] = stream.value.Byte2;
        stream.buffer[stream.write_len++] = stream.value.Byte1;
        stream.buffer[stream.write_len++] = stream.value.Byte0;
      }
    }

    public static void Write(ZeroCopyBuffer stream, double value)
    {
      stream.CheckWriteBuffer(8);
      stream.value.VF64 = value;
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

    public static void Read(ZeroCopyBuffer stream, ref sbyte value)
    {
      stream.CheckReadBuffer(1);
      value = (sbyte)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      bool signed = (value & const_negative_bit_value) > 0;
      stream.CheckReadBuffer(read_bytes);
      value = (sbyte)stream.buffer[stream.read_len++];
      if (signed)
      {
        value = (sbyte)-value;
      }
    }

    public static void Read(ZeroCopyBuffer stream, ref byte value)
    {
      stream.CheckReadBuffer(1);
      value = stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
      value = stream.buffer[stream.read_len++];
    }

    public static void Read(ZeroCopyBuffer stream, ref Int16 value)
    {
      stream.CheckReadBuffer(1);
      value = (Int16)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      bool signed = (value & const_negative_bit_value) > 0;
      if (signed)
      {
        value -= const_negative_bit_value;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Read(ZeroCopyBuffer stream, ref UInt16 value)
    {
      stream.CheckReadBuffer(1);
      value = (UInt16)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Read(ZeroCopyBuffer stream, ref Int32 value)
    {
      stream.CheckReadBuffer(1);
      value = (Int32)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      bool signed = (value & const_negative_bit_value) > 0;
      if (signed)
      {
        value -= const_negative_bit_value;
      }
      int read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Read(ZeroCopyBuffer stream, ref UInt32 value)
    {
      stream.CheckReadBuffer(1);
      value = (UInt32)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      Int32 read_bytes = (Int32)(value & const_interger_byte_msak) + 1;
      if (read_bytes > 4)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Read(ZeroCopyBuffer stream, ref Int64 value)
    {
      stream.CheckReadBuffer(1);
      value = (Int64)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      bool signed = (value & const_negative_bit_value) > 0;
      if (signed)
      {
        value -= const_negative_bit_value;
      }
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Read(ZeroCopyBuffer stream, ref UInt64 value)
    {
      stream.CheckReadBuffer(1);
      value = (UInt64)stream.buffer[stream.read_len++];
      if (value >= 0 && value <= const_tag_as_value)
      {
        return;
      }
      int read_bytes = (int)(value & const_interger_byte_msak) + 1;
      if (read_bytes > 8)
      {
        stream.RaiseErrorCode(EErrorCode.value_too_large_to_integer_number);
      }
      stream.CheckReadBuffer(read_bytes);
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
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              break;
            }
          case 2:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              break;
            }
          case 3:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              break;
            }
          case 4:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              break;
            }
          case 5:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              break;
            }
          case 6:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              break;
            }
          case 7:
            {
              stream.value.Byte7 = stream.buffer[stream.read_len++];
              stream.value.Byte6 = stream.buffer[stream.read_len++];
              stream.value.Byte5 = stream.buffer[stream.read_len++];
              stream.value.Byte4 = stream.buffer[stream.read_len++];
              stream.value.Byte3 = stream.buffer[stream.read_len++];
              stream.value.Byte2 = stream.buffer[stream.read_len++];
              stream.value.Byte1 = stream.buffer[stream.read_len++];
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

    public static void Write(ZeroCopyBuffer stream, sbyte value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        byte temp = (byte)value;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          temp = (byte)(-value);
        }
        stream.CheckWriteBuffer(2);
        stream.buffer[stream.write_len++] = tag;
        stream.buffer[stream.write_len++] = temp;
      }
    }

    public static void Write(ZeroCopyBuffer stream, byte value)
    {
      if (value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = value;
      }
      else
      {
        byte tag = const_tag_as_type;
        stream.CheckWriteBuffer(2);
        stream.buffer[stream.write_len++] = tag;
        stream.buffer[stream.write_len++] = value;
      }
    }

    public static void Write(ZeroCopyBuffer stream, Int16 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        UInt16 temp = (UInt16)value;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          temp = (UInt16)(-value);
        }
        if (temp < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)temp;
        }
        else
        {
          stream.CheckWriteBuffer(3);
          stream.value.VU16 = temp;
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

    public static void Write(ZeroCopyBuffer stream, UInt16 value)
    {
      if (value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else
        {
          stream.CheckWriteBuffer(3);
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

    public static void Write(ZeroCopyBuffer stream, Int32 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        UInt32 temp = (UInt32)value;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          temp = (UInt32)(-value);
        }
        if (temp < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)temp;
        }
        else if (temp < 0x10000)
        {
          stream.CheckWriteBuffer(3);
          stream.value.VU32 = temp;
          ++tag;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
        }
        else if (temp < 0x1000000)
        {
          stream.CheckWriteBuffer(4);
          stream.value.VU32 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
        }
        else
        {
          stream.CheckWriteBuffer(5);
          stream.value.VU32 = temp;
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

    public static void Write(ZeroCopyBuffer stream, UInt32 value)
    {
      if (value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          stream.CheckWriteBuffer(3);
          stream.value.VU32 = value;
          stream.buffer[stream.write_len++] = 0x81;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
        }
        else if (value < 0x1000000)
        {
          stream.CheckWriteBuffer(4);
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
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
        }
        else
        {
          stream.CheckWriteBuffer(5);
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

    public static void Write(ZeroCopyBuffer stream, Int64 value)
    {
      if (0 <= value && value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        byte tag = const_tag_as_type;
        UInt64 temp = (UInt64)value;
        if (value < 0)
        {
          tag += const_negative_bit_value;
          temp = (UInt64)(-value);
        }
        if (temp < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = tag;
          stream.buffer[stream.write_len++] = (byte)temp;
        }
        else if (temp < 0x10000)
        {
          stream.CheckWriteBuffer(3);
          stream.value.VU64 = temp;
          ++tag;
          stream.buffer[stream.write_len++] = tag;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
          }
        }
        else if (temp < 0x1000000)
        {
          stream.CheckWriteBuffer(4);
          stream.value.VU64 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
          }
        }
        else if (temp < 0x100000000L)
        {
          stream.CheckWriteBuffer(5);
          stream.value.VU64 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
          }
        }
        else if (temp < 0x10000000000L)
        {
          stream.CheckWriteBuffer(6);
          stream.value.VU64 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
        }
        else if (temp < 0x1000000000000L)
        {
          stream.CheckWriteBuffer(7);
          stream.value.VU64 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
        }
        else if (temp < 0x100000000000000L)
        {
          stream.CheckWriteBuffer(8);
          stream.value.VU64 = temp;
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
        }
        else
        {
          stream.CheckWriteBuffer(9);
          stream.value.VU64 = temp;
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

    public static void Write(ZeroCopyBuffer stream, UInt64 value)
    {
      if (value <= const_tag_as_value)
      {
        stream.CheckWriteBuffer(1);
        stream.buffer[stream.write_len++] = (byte)value;
      }
      else
      {
        if (value < 0x100)
        {
          stream.CheckWriteBuffer(2);
          stream.buffer[stream.write_len++] = 0x80;
          stream.buffer[stream.write_len++] = (byte)value;
        }
        else if (value < 0x10000)
        {
          stream.CheckWriteBuffer(3);
          stream.value.VU64 = value;
          stream.buffer[stream.write_len++] = 0x81;
          if (IsLittleEndian)
          {
            stream.buffer[stream.write_len++] = stream.value.Byte0;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
          else
          {
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
          }
        }
        else if (value < 0x1000000)
        {
          stream.CheckWriteBuffer(4);
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
          }
        }
        else if (value < 0x100000000UL)
        {
          stream.CheckWriteBuffer(5);
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
          }
        }
        else if (value < 0x10000000000UL)
        {
          stream.CheckWriteBuffer(6);
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
          }
        }
        else if (value < 0x1000000000000UL)
        {
          stream.CheckWriteBuffer(7);
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
          }
        }
        else if (value < 0x100000000000000UL)
        {
          stream.CheckWriteBuffer(8);
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
            stream.buffer[stream.write_len++] = stream.value.Byte7;
            stream.buffer[stream.write_len++] = stream.value.Byte6;
            stream.buffer[stream.write_len++] = stream.value.Byte5;
            stream.buffer[stream.write_len++] = stream.value.Byte4;
            stream.buffer[stream.write_len++] = stream.value.Byte3;
            stream.buffer[stream.write_len++] = stream.value.Byte2;
            stream.buffer[stream.write_len++] = stream.value.Byte1;
          }
        }
        else
        {
          stream.CheckWriteBuffer(9);
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

    public static Int32 CheckReadSize(ZeroCopyBuffer stream)
    {
      return CheckReadSize(stream, 0);
    }

    public static Int32 CheckReadSize(ZeroCopyBuffer stream, int size)
    {
      Int32 len = 0;
      Read(stream, ref len);
      if (size > 0 && len > size)
      {
        stream.RaiseErrorCode(EErrorCode.number_of_element_not_macth);
      }
      return len;
    }

    public static void Read(ZeroCopyBuffer stream, ref string value, Int32 len)
    {
      stream.CheckReadBuffer(len);
      value = Encoding.UTF8.GetString(stream.buffer, stream.read_len, len);
      stream.read_len += (int)len;
    }

    public static void Read(ZeroCopyBuffer stream, ref byte[] value, Int32 len)
    {
      if (len == 0)
        return;
      stream.CheckReadBuffer(len);
      value = new byte[len];
      System.Buffer.BlockCopy(stream.buffer, stream.read_len, value, 0, len);
      stream.read_len += (int)len;
    }

    public static void Write(ZeroCopyBuffer stream, string value, Int32 len)
    {
      byte[] buff = Encoding.UTF8.GetBytes(value);
      if (buff.Length > len)
      {
        stream.RaiseErrorCode(EErrorCode.sequence_length_overflow);
      }
      stream.CheckWriteBuffer(buff.Length);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void Write(ZeroCopyBuffer stream, byte[] buff, Int32 len)
    {
      stream.CheckWriteBuffer(len);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void Read(ZeroCopyBuffer stream, ref string value)
    {
      Int32 len = 0;
      Read(stream, ref len);
      Read(stream, ref value, len);
    }

    public static void Read(ZeroCopyBuffer stream, ref byte[] value)
    {
      Int32 len = 0;
      Read(stream, ref len);
      Read(stream, ref value, len);
    }

    public static void Write(ZeroCopyBuffer stream, string value)
    {
      byte[] buff = Encoding.UTF8.GetBytes(value);

      Int32 len = buff.Length;
      Write(stream, len);
      stream.CheckWriteBuffer(len);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void Write(ZeroCopyBuffer stream, byte[] buff)
    {
      Int32 len = buff.Length;
      Write(stream, len);
      stream.CheckWriteBuffer(len);
      for (int i = 0; i < len; ++i)
      {
        stream.buffer[stream.write_len++] = buff[i];
      }
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref string value)
    {
      Int32 len = 0;
      Read(stream, ref len);
      stream.CheckReadBuffer(len);
      stream.read_len += (int)len;
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref string value, Int32 len)
    {
      stream.CheckReadBuffer(len);
      stream.read_len += (int)len;
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref byte[] value)
    {
      Int32 len = 0;
      Read(stream, ref len);
      stream.CheckReadBuffer(len);
      stream.read_len += (int)len;
    }

    public static void SkipRead(ZeroCopyBuffer stream, ref byte[] value, Int32 len)
    {
      stream.CheckReadBuffer(len);
      stream.read_len += (int)len;
    }

    public static void SkipReadCompatible(ZeroCopyBuffer stream)
    {
      int offset = stream.read_len;
      Int64 tag = 0;
      Read(stream, ref tag);
      Int32 len_tag = 0;
      Read(stream, ref len_tag);
      Int32 read_len = stream.read_len - offset;
      if (len_tag > read_len) stream.SkipRead(len_tag - read_len);
    }
  }

  public abstract class BaseObj
  {
    public abstract void Read(ZeroCopyBuffer stream);
    public abstract Int32 SizeOf();
    public abstract void Write(ZeroCopyBuffer stream);
    public abstract void RawRead(ZeroCopyBuffer stream);
    public abstract Int32 RawSizeOf();
    public abstract void RawWrite(ZeroCopyBuffer stream);
    public abstract void Reset();

    public void SkipRead(ZeroCopyBuffer stream)
    {
      Stream.SkipReadCompatible(stream);
    }
  }
}
