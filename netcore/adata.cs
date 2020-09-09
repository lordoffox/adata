// (C) Copyright Ning Ding 2014.8
// lordoffox@gmail.com
// Distributed under the boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Buffers;

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

    public class ZeroCopyBufferReader
    {
        public Memory<byte> ReadMemory { get; private set; }

        public int ReadSize { get; private set; }
        public int ReadLen = 0;
        internal UType value;

        EErrorCode error_code;

        public ZeroCopyBufferReader(Memory<byte> buf)
        {
            Reset(buf);
        }

        public void Reset(Memory<byte> buf)
        {
            ReadMemory = buf;
            ReadSize = buf.Length;
            ReadLen = 0;
            value = new UType();
            error_code = EErrorCode.success;
        }

        public void Clear()
        {
            this.ReadLen = 0;
            error_code = EErrorCode.success;
        }

        public EErrorCode ErrorCode { get { return error_code; } }

        public void RaiseErrorCode(EErrorCode ec)
        {
            error_code = ec;
            throw new ADataException(error_code);
        }

        public bool Error { get { return error_code != EErrorCode.success; } }

        public bool End { get { return ReadLen >= ReadSize; } }

        public void CheckReadBuffer(Int32 len)
        {
            if (ReadLen + len > ReadSize)
            {
                RaiseErrorCode(EErrorCode.stream_buffer_overflow);
            }
        }

        public void SkipRead(Int32 len)
        {
            CheckReadBuffer(len);
            ReadLen += (int)len;
        }

        public int ReadLength()
        {
            return ReadLen;
        }
    }

    public class ZeroCopyBufferWriter
    {
        IMemoryOwner<byte> WriteMemoryOwner;
        public Memory<byte> WriteMemory { get; private set; }

        public int WriteSize { get; private set; }
        public int WriteLen = 0;
        internal UType value;

        EErrorCode error_code;

        public ZeroCopyBufferWriter(int sz = 65536)
        {
            WriteMemoryOwner = MemoryPool<byte>.Shared.Rent(sz);
            WriteMemory = WriteMemoryOwner.Memory;
            WriteSize = sz;
            WriteLen = 0;
            value = new UType();
            error_code = EErrorCode.success;
        }

        public void Clear()
        {
            this.WriteLen = 0;
            error_code = EErrorCode.success;
        }

        public EErrorCode ErrorCode { get { return error_code; } }

        public void RaiseErrorCode(EErrorCode ec)
        {
            error_code = ec;
            throw new ADataException(error_code);
        }

        public bool Error { get { return error_code != EErrorCode.success; } }

        public void CheckWriteBuffer(Int32 len)
        {
            var want = WriteLen + len;

            if (want > WriteSize)
            {
                var new_data_len = WriteSize + (WriteSize >> 1);
                if (want > new_data_len)
                {
                    new_data_len = new_data_len + (new_data_len >> 1);
                }
                var new_write_buffer_memoey_ = MemoryPool<byte>.Shared.Rent(new_data_len);
                WriteMemory.CopyTo(new_write_buffer_memoey_.Memory);
                WriteMemoryOwner.Dispose();
                WriteMemoryOwner = new_write_buffer_memoey_;
                WriteMemory = WriteMemoryOwner.Memory;
                WriteSize = new_data_len;
            }
        }

        public int WriteLength()
        {
            return WriteLen;
        }

        public Span<byte> GetSpan()
        {
            return WriteMemory.Span.Slice(0, WriteLen);
        }

        public Memory<byte> GetMemory()
        {
            return WriteMemory.Slice(0, WriteLen);
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

        public static void SkipBytes(ZeroCopyBufferReader stream, int count)
        {
            stream.SkipRead(count);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref sbyte value)
        {
            SkipBytes(stream, 1);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref byte value)
        {
            SkipBytes(stream, 1);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref Int16 value)
        {
            SkipBytes(stream, 2);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref UInt16 value)
        {
            SkipBytes(stream, 2);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref Int32 value)
        {
            SkipBytes(stream, 4);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref UInt32 value)
        {
            SkipBytes(stream, 4);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref Int64 value)
        {
            SkipBytes(stream, 8);
        }

        public static void FixSkipRead(ZeroCopyBufferReader stream, ref UInt64 value)
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

        public static void SkipReadValueImpl(ZeroCopyBufferReader stream, int max_bytes)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            byte tag = buffer[stream.ReadLen++];
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
            stream.ReadLen += read_bytes;
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref sbyte value)
        {
            SkipReadValueImpl(stream, 1);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref byte value)
        {
            SkipReadValueImpl(stream, 1);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref Int16 value)
        {
            SkipReadValueImpl(stream, 2);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref UInt16 value)
        {
            SkipReadValueImpl(stream, 2);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref Int32 value)
        {
            SkipReadValueImpl(stream, 4);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref UInt32 value)
        {
            SkipReadValueImpl(stream, 4);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref Int64 value)
        {
            SkipReadValueImpl(stream, 8);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref UInt64 value)
        {
            SkipReadValueImpl(stream, 8);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref float value)
        {
            SkipBytes(stream, 4);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref double value)
        {
            SkipBytes(stream, 8);
        }

        public static Int32 SizeOf(sbyte value)
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

        public static void FixRead(ZeroCopyBufferReader stream, ref sbyte value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (sbyte)buffer[stream.ReadLen++];
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref byte value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = buffer[stream.ReadLen++];
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref Int16 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(2);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VI16;
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref UInt16 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(2);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VU16;
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref Int32 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(4);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VI32;
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref UInt32 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(4);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VU32;
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref Int64 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(8);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte7 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte7 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VI64;
        }

        public static void FixRead(ZeroCopyBufferReader stream, ref UInt64 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(8);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte7 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte7 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VU64;
        }

        public static void Read(ZeroCopyBufferReader stream, ref float value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(4);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VF32;
        }

        public static void Read(ZeroCopyBufferReader stream, ref double value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(8);
            if (IsLittleEndian)
            {
                stream.value.Byte0 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte7 = buffer[stream.ReadLen++];
            }
            else
            {
                stream.value.Byte7 = buffer[stream.ReadLen++];
                stream.value.Byte6 = buffer[stream.ReadLen++];
                stream.value.Byte5 = buffer[stream.ReadLen++];
                stream.value.Byte4 = buffer[stream.ReadLen++];
                stream.value.Byte3 = buffer[stream.ReadLen++];
                stream.value.Byte2 = buffer[stream.ReadLen++];
                stream.value.Byte1 = buffer[stream.ReadLen++];
                stream.value.Byte0 = buffer[stream.ReadLen++];
            }
            value = stream.value.VF64;
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, sbyte value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(1);
            buffer[stream.WriteLen++] = (byte)value;
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, byte value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(1);
            buffer[stream.WriteLen++] = (byte)value;
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, Int16 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(2);
            stream.value.VI16 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, UInt16 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(2);
            stream.value.VU16 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, Int32 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(4);
            stream.value.VI32 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, UInt32 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(4);
            stream.value.VU32 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, Int64 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(8);
            stream.value.VI64 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte7;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte7;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void FixWrite(ZeroCopyBufferWriter stream, UInt64 value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(8);
            stream.value.VU64 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte7;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte7;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, float value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(4);
            stream.value.VF32 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, double value)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(8);
            stream.value.VF64 = value;
            if (IsLittleEndian)
            {
                buffer[stream.WriteLen++] = stream.value.Byte0;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte7;
            }
            else
            {
                buffer[stream.WriteLen++] = stream.value.Byte7;
                buffer[stream.WriteLen++] = stream.value.Byte6;
                buffer[stream.WriteLen++] = stream.value.Byte5;
                buffer[stream.WriteLen++] = stream.value.Byte4;
                buffer[stream.WriteLen++] = stream.value.Byte3;
                buffer[stream.WriteLen++] = stream.value.Byte2;
                buffer[stream.WriteLen++] = stream.value.Byte1;
                buffer[stream.WriteLen++] = stream.value.Byte0;
            }
        }

        public static void Read(ZeroCopyBufferReader stream, ref sbyte value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (sbyte)buffer[stream.ReadLen++];
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
            value = (sbyte)buffer[stream.ReadLen++];
            if (signed)
            {
                value = (sbyte)-value;
            }
        }

        public static void Read(ZeroCopyBufferReader stream, ref byte value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = buffer[stream.ReadLen++];
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
            value = buffer[stream.ReadLen++];
        }

        public static void Read(ZeroCopyBufferReader stream, ref Int16 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (Int16)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
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
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
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

        public static void Read(ZeroCopyBufferReader stream, ref UInt16 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (UInt16)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
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
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                }
            }
            value = stream.value.VU16;
        }

        public static void Read(ZeroCopyBufferReader stream, ref Int32 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (Int32)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
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
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
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

        public static void Read(ZeroCopyBufferReader stream, ref UInt32 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (UInt32)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
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
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                }
            }
            value = stream.value.VU32;
        }

        public static void Read(ZeroCopyBufferReader stream, ref Int64 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (Int64)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 5:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 6:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 7:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 8:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte7 = buffer[stream.ReadLen++];
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
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 5:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 6:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 7:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 8:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
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

        public static void Read(ZeroCopyBufferReader stream, ref UInt64 value)
        {
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(1);
            value = (UInt64)buffer[stream.ReadLen++];
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
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 5:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 6:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 7:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 8:
                        {
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte7 = buffer[stream.ReadLen++];
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
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 2:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 3:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 4:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 5:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 6:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 7:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            break;
                        }
                    case 8:
                        {
                            stream.value.Byte7 = buffer[stream.ReadLen++];
                            stream.value.Byte6 = buffer[stream.ReadLen++];
                            stream.value.Byte5 = buffer[stream.ReadLen++];
                            stream.value.Byte4 = buffer[stream.ReadLen++];
                            stream.value.Byte3 = buffer[stream.ReadLen++];
                            stream.value.Byte2 = buffer[stream.ReadLen++];
                            stream.value.Byte1 = buffer[stream.ReadLen++];
                            stream.value.Byte0 = buffer[stream.ReadLen++];
                            break;
                        }
                }
            }
            value = stream.value.VU64;
        }

        public static void Write(ZeroCopyBufferWriter stream, sbyte value)
        {
            var buffer = stream.WriteMemory.Span;
            if (0 <= value && value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
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
                buffer[stream.WriteLen++] = tag;
                buffer[stream.WriteLen++] = temp;
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, byte value)
        {
            var buffer = stream.WriteMemory.Span;
            if (value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = value;
            }
            else
            {
                byte tag = const_tag_as_type;
                stream.CheckWriteBuffer(2);
                buffer[stream.WriteLen++] = tag;
                buffer[stream.WriteLen++] = value;
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, Int16 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (0 <= value && value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
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
                    buffer[stream.WriteLen++] = tag;
                    buffer[stream.WriteLen++] = (byte)temp;
                }
                else
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU16 = temp;
                    ++tag;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, UInt16 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
            }
            else
            {
                if (value < 0x100)
                {
                    stream.CheckWriteBuffer(2);
                    buffer[stream.WriteLen++] = 0x80;
                    buffer[stream.WriteLen++] = (byte)value;
                }
                else
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU16 = value;
                    buffer[stream.WriteLen++] = 0x81;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, Int32 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (0 <= value && value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
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
                    buffer[stream.WriteLen++] = tag;
                    buffer[stream.WriteLen++] = (byte)temp;
                }
                else if (temp < 0x10000)
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU32 = temp;
                    ++tag;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                }
                else if (temp < 0x1000000)
                {
                    stream.CheckWriteBuffer(4);
                    stream.value.VU32 = temp;
                    tag += 2;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                }
                else
                {
                    stream.CheckWriteBuffer(5);
                    stream.value.VU32 = temp;
                    tag += 3;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, UInt32 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
            }
            else
            {
                if (value < 0x100)
                {
                    stream.CheckWriteBuffer(2);
                    buffer[stream.WriteLen++] = 0x80;
                    buffer[stream.WriteLen++] = (byte)value;
                }
                else if (value < 0x10000)
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU32 = value;
                    buffer[stream.WriteLen++] = 0x81;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                }
                else if (value < 0x1000000)
                {
                    stream.CheckWriteBuffer(4);
                    stream.value.VU32 = value;
                    buffer[stream.WriteLen++] = 0x82;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                }
                else
                {
                    stream.CheckWriteBuffer(5);
                    stream.value.VU32 = value;
                    buffer[stream.WriteLen++] = 0x83;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, Int64 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (0 <= value && value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
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
                    buffer[stream.WriteLen++] = tag;
                    buffer[stream.WriteLen++] = (byte)temp;
                }
                else if (temp < 0x10000)
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU64 = temp;
                    ++tag;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                    }
                }
                else if (temp < 0x1000000)
                {
                    stream.CheckWriteBuffer(4);
                    stream.value.VU64 = temp;
                    tag += 2;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                    }
                }
                else if (temp < 0x100000000L)
                {
                    stream.CheckWriteBuffer(5);
                    stream.value.VU64 = temp;
                    tag += 3;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                    }
                }
                else if (temp < 0x10000000000L)
                {
                    stream.CheckWriteBuffer(6);
                    stream.value.VU64 = temp;
                    tag += 4;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                }
                else if (temp < 0x1000000000000L)
                {
                    stream.CheckWriteBuffer(7);
                    stream.value.VU64 = temp;
                    tag += 5;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                }
                else if (temp < 0x100000000000000L)
                {
                    stream.CheckWriteBuffer(8);
                    stream.value.VU64 = temp;
                    tag += 6;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                }
                else
                {
                    stream.CheckWriteBuffer(9);
                    stream.value.VU64 = temp;
                    tag += 7;
                    buffer[stream.WriteLen++] = tag;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static void Write(ZeroCopyBufferWriter stream, UInt64 value)
        {
            var buffer = stream.WriteMemory.Span;
            if (value <= const_tag_as_value)
            {
                stream.CheckWriteBuffer(1);
                buffer[stream.WriteLen++] = (byte)value;
            }
            else
            {
                if (value < 0x100)
                {
                    stream.CheckWriteBuffer(2);
                    buffer[stream.WriteLen++] = 0x80;
                    buffer[stream.WriteLen++] = (byte)value;
                }
                else if (value < 0x10000)
                {
                    stream.CheckWriteBuffer(3);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x81;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                    }
                }
                else if (value < 0x1000000)
                {
                    stream.CheckWriteBuffer(4);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x82;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                    }
                }
                else if (value < 0x100000000UL)
                {
                    stream.CheckWriteBuffer(5);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x83;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                    }
                }
                else if (value < 0x10000000000UL)
                {
                    stream.CheckWriteBuffer(6);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x84;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                    }
                }
                else if (value < 0x1000000000000UL)
                {
                    stream.CheckWriteBuffer(7);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x85;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                    }
                }
                else if (value < 0x100000000000000UL)
                {
                    stream.CheckWriteBuffer(8);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x86;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                    }
                }
                else
                {
                    stream.CheckWriteBuffer(9);
                    stream.value.VU64 = value;
                    buffer[stream.WriteLen++] = 0x87;
                    if (IsLittleEndian)
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                    }
                    else
                    {
                        buffer[stream.WriteLen++] = stream.value.Byte7;
                        buffer[stream.WriteLen++] = stream.value.Byte6;
                        buffer[stream.WriteLen++] = stream.value.Byte5;
                        buffer[stream.WriteLen++] = stream.value.Byte4;
                        buffer[stream.WriteLen++] = stream.value.Byte3;
                        buffer[stream.WriteLen++] = stream.value.Byte2;
                        buffer[stream.WriteLen++] = stream.value.Byte1;
                        buffer[stream.WriteLen++] = stream.value.Byte0;
                    }
                }
            }
        }

        public static Int32 CheckReadSize(ZeroCopyBufferReader stream)
        {
            return CheckReadSize(stream, 0);
        }

        public static Int32 CheckReadSize(ZeroCopyBufferReader stream, int size)
        {
            Int32 len = 0;
            Read(stream, ref len);
            if (size > 0 && len > size)
            {
                stream.RaiseErrorCode(EErrorCode.number_of_element_not_macth);
            }
            return len;
        }

        public static void Read(ZeroCopyBufferReader stream, ref string value, Int32 len)
        {
            if (len == 0)
            {
                value = string.Empty;
                return;
            }
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(len);
            var bytes = buffer.Slice(stream.ReadLen, len);
            value = Encoding.UTF8.GetString(bytes);
            stream.ReadLen += len;
        }

        public static void Read(ZeroCopyBufferReader stream, ref byte[] value, Int32 len)
        {
            if (len == 0)
            {
                value = new byte[0];
                return;
            }
            var buffer = stream.ReadMemory.Span;
            stream.CheckReadBuffer(len);
            value = new byte[len];
            Span<byte> value_bytes = value;
            var bytes = buffer.Slice(stream.ReadLen, len);
            bytes.CopyTo(value_bytes);
            stream.ReadLen += len;
        }

        public static void Write(ZeroCopyBufferWriter stream, string value, Int32 len)
        {
            stream.CheckWriteBuffer(len);
            var buffer = stream.WriteMemory.Span;
            var bytes = buffer.Slice(stream.WriteLen, len);
            Encoding.UTF8.GetBytes(value, bytes);
            stream.WriteLen += len;
        }

        public static void Write(ZeroCopyBufferWriter stream, byte[] buff, Int32 len)
        {
            var buffer = stream.WriteMemory.Span;
            stream.CheckWriteBuffer(len);
            for (int i = 0; i < len; ++i)
            {
                buffer[stream.WriteLen++] = buff[i];
            }
        }

        public static void Read(ZeroCopyBufferReader stream, ref string value)
        {
            Int32 len = 0;
            Read(stream, ref len);
            Read(stream, ref value, len);
        }

        public static void Read(ZeroCopyBufferReader stream, ref byte[] value)
        {
            Int32 len = 0;
            Read(stream, ref len);
            Read(stream, ref value, len);
        }

        public static void Write(ZeroCopyBufferWriter stream, string value)
        {
            var len = Encoding.UTF8.GetByteCount(value);
            var buffer = stream.WriteMemory.Span;
            Write(stream, len);
            Write(stream, value, len);
        }

        public static void Write(ZeroCopyBufferWriter stream, byte[] buff)
        {
            Int32 len = buff.Length;
            Write(stream, len);
            stream.CheckWriteBuffer(len);
            Write(stream, buff, len);
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref string value)
        {
            Int32 len = 0;
            Read(stream, ref len);
            stream.CheckReadBuffer(len);
            stream.ReadLen += (int)len;
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref string value, Int32 len)
        {
            stream.CheckReadBuffer(len);
            stream.ReadLen += (int)len;
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref byte[] value)
        {
            Int32 len = 0;
            Read(stream, ref len);
            stream.CheckReadBuffer(len);
            stream.ReadLen += (int)len;
        }

        public static void SkipRead(ZeroCopyBufferReader stream, ref byte[] value, Int32 len)
        {
            stream.CheckReadBuffer(len);
            stream.ReadLen += (int)len;
        }

        public static void SkipReadCompatible(ZeroCopyBufferReader stream)
        {
            int offset = stream.ReadLen;
            Int64 tag = 0;
            Read(stream, ref tag);
            Int32 len_tag = 0;
            Read(stream, ref len_tag);
            Int32 read_len = stream.ReadLen - offset;
            if (len_tag > read_len) stream.SkipRead(len_tag - read_len);
        }

        public static void WriteStream(StringBuilder builder, byte[] buff)
        {
            for (int i = 0; i < buff.Length; i++)
            {
                builder.Append(buff[i].ToString("x2"));
            }
        }
    }

    public abstract class BaseObj
    {
        public abstract void Read(ZeroCopyBufferReader stream);
        public abstract Int32 SizeOf();
        public abstract void Write(ZeroCopyBufferWriter stream);
        public abstract void RawRead(ZeroCopyBufferReader stream);
        public abstract Int32 RawSizeOf();
        public abstract void RawWrite(ZeroCopyBufferWriter stream);
        public abstract void Reset();
        public abstract void WriteStream(StringBuilder builder);

        public void SkipRead(ZeroCopyBufferReader stream)
        {
            Stream.SkipReadCompatible(stream);
        }

        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();
            WriteStream(builder);
            return builder.ToString();
        }
    }
}
