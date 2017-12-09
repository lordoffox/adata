package adata

import kotlin.reflect.full.primaryConstructor

internal const val TAG_AS_VALUE = 0x7f
internal const val TAG_AS_TYPE = 0x80
internal const val INTEGER_BYTE_MASK = 0x1f
internal const val NEGATIVE_BIT_MASK = 0x20
//internal const val StorePostiveIntegerByteMask = 0x80 - 2
@Suppress("INTEGER_OVERFLOW")
internal const val OVERFLOW_LONG = Long.MIN_VALUE - 1

@Suppress("UNUSED_PARAMETER")
fun fixSizeOf(v: Byte): Int = 1

@Suppress("UNUSED_PARAMETER")
fun fixSizeOf(v: Short): Int = 2

@Suppress("UNUSED_PARAMETER")
fun fixSizeOf(v: Int): Int = 4

@Suppress("UNUSED_PARAMETER")
fun fixSizeOf(v: Long): Int = 8

fun sizeOf(value: Byte): Int {
  return if (value in 0..TAG_AS_VALUE) 1 else 2
}

fun sizeOf(value: Short): Int {
  if (value in 0..TAG_AS_VALUE) return 1
  var temp = value.toInt()
  if (value < 0) temp = -value
  return if (temp < 0x100) 2 else 3
}

fun sizeOf(value: Int): Int {
  if (value in 0..TAG_AS_VALUE) return 1
  var temp = value.toLong()
  if (value < 0) temp = (-value).toLong()
  return when {
    temp < 0x100 -> 2
    temp < 0x10000 -> 3
    temp < 0x1000000 -> 4
    else -> 5
  }
}

fun sizeOf(value: Long): Int {
  var v = value
  if (v in 0..TAG_AS_VALUE) return 1
  if (v == OVERFLOW_LONG) return 9
  if (v < 0) v = -v
  return when {
    v < 0x100 -> 2
    v < 0x10000 -> 3
    v < 0x1000000 -> 4
    v < 0x100000000L -> 5
    v < 0x10000000000L -> 6
    v < 0x1000000000000L -> 7
    v < 0x100000000000000L -> 8
    else -> 9
  }
}

@Suppress("UNUSED_PARAMETER")
fun sizeOf(value: Float): Int = 4

@Suppress("UNUSED_PARAMETER")
fun sizeOf(value: Double): Int = 8

fun sizeOf(str: String): Int {
  var len = str.toByteArray(java.nio.charset.StandardCharsets.UTF_8).size
  len += sizeOf(len)
  return len
}

fun fixSizeOf(str: String): Int {
  return sizeOf(str)
}

class Stream {
  private var readBuffer: ByteArray = ByteArray(0)
  private var writeBuffer: ByteArray = ByteArray(1024)
  private var readLen = 0
  private var writeLen = 0

  fun getReadBuffer(): ByteArray {
    return readBuffer
  }

  fun setReadBuffer(buf: ByteArray) {
    readBuffer = buf
  }

  fun getWriteBuffer(): ByteArray {
    return writeBuffer
  }

  fun setWriteBuffer(buf: ByteArray) {
    writeBuffer = buf
  }

  fun writeData(data: ByteArray, len: Int) {
    System.arraycopy(data, 0, writeBuffer, writeLen, len)
    writeLen += len
  }

  fun clearRead() {
    readLen = 0
  }

  fun clearWrite() {
    writeLen = 0
  }

  fun clear() {
    clearRead()
    clearWrite()
  }

  fun readLength(): Int = readLen

  fun writeLength(): Int = writeLen

  fun readBufferSize(): Int = readBuffer.size

  fun writeBufferSizeLen(): Int = writeBuffer.size

  fun fixReadInt8(): Byte {
    return readBuffer[readLen++]
  }

  fun fixReadInt16(): Short {
    var v = loadUint8()
    v += loadUint8() shl 8
    return v.toShort()
  }

  fun fixReadInt32(): Int {
    var v = loadUint8()
    v += loadUint8() shl 8
    v += loadUint8() shl 16
    v += loadUint8() shl 24
    return v
  }

  fun fixReadInt64(): Long {
    var v = loadUint8Long()
    v += loadUint8Long() shl 8
    v += loadUint8Long() shl 16
    v += loadUint8Long() shl 24
    v += loadUint8Long() shl 32
    v += loadUint8Long() shl 40
    v += loadUint8Long() shl 48
    v += loadUint8Long() shl 56
    return v
  }

  fun fixWrite(v: Byte) {
    writeBuffer[writeLen++] = v
  }

  fun fixWrite(v1: Short) {
    var v: Int = v1.toInt()
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
  }

  fun fixWrite(v1: Int) {
    var v = v1
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
  }

  fun fixWrite(v1: Long) {
    var v = v1
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
    v = v shr 8
    writeBuffer[writeLen++] = (v and 0xff).toByte()
  }

  private fun loadUint8(): Int {
    return readBuffer[readLen++].toInt() and 0xff
  }

  private fun loadUint8Long(): Long {
    return (readBuffer[readLen++].toInt() and 0xff).toLong()
  }

  private fun saveUint8(v: Int) {
    writeBuffer[writeLen++] = (v and 0xff).toByte()
  }

  private fun readScacleInt(max_bytes: Int): Long {
    val tag = loadUint8()
    if (tag < TAG_AS_TYPE) {
      return tag.toLong()
    }
    val readBytes = (tag and INTEGER_BYTE_MASK) + 1
    if (readBytes > max_bytes)
      throw RuntimeException("integer value out of range.")
    var value: Long = 0
    var bit = 0
    var i = 0
    while (i < readBytes) {
      value += loadUint8Long() shl bit
      ++i
      bit += 8
    }
    if (tag and NEGATIVE_BIT_MASK > 0) {
      value = -value
    }
    return value
  }

  fun readInt8(): Byte {
    val value = readScacleInt(1)
    return value.toByte()
  }

  fun readInt16(): Short {
    val value = readScacleInt(2)
    return value.toShort()
  }

  fun readInt32(): Int {
    val value = readScacleInt(4)
    return value.toInt()
  }

  fun readInt64(): Long {
    return readScacleInt(8)
  }

  private fun writeScacleInt(value: Long) {
    var v = value
    if (v in 0..TAG_AS_VALUE) {
      saveUint8(v.toInt())
      return
    }
    if (v == OVERFLOW_LONG) {
      val tag = TAG_AS_TYPE + NEGATIVE_BIT_MASK
      saveUint8(tag + 7)
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      v = v shr 8
      saveUint8(v.toInt())
      return
    }
    var tag = TAG_AS_TYPE
    if (v < 0) {
      v = (-v)
      tag += NEGATIVE_BIT_MASK
    }
    when {
      v < 0x100 -> {
        saveUint8(tag)
        saveUint8(v.toInt())
      }
      v < 0x10000 -> {
        saveUint8(tag + 1)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      v < 0x1000000 -> {
        saveUint8(tag + 2)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      v < 0x100000000L -> {
        saveUint8(tag + 3)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      v < 0x10000000000L -> {
        saveUint8(tag + 4)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      v < 0x1000000000000L -> {
        saveUint8(tag + 5)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      v < 0x100000000000000L -> {
        saveUint8(tag + 6)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
      else -> {
        saveUint8(tag + 7)
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
        v = v shr 8
        saveUint8(v.toInt())
      }
    }
  }

  fun write(value: Byte) {
    writeScacleInt(value.toLong())
  }

  fun write(value: Short) {
    writeScacleInt(value.toLong())
  }

  fun write(value: Int) {
    writeScacleInt(value.toLong())
  }

  fun write(value: Long) {
    writeScacleInt(value)
  }

  fun readFloat(): Float {
    var l = loadUint8()
    l += loadUint8() shl 8
    l += loadUint8() shl 16
    l += loadUint8() shl 24
    return java.lang.Float.intBitsToFloat(l)
  }

  fun readDouble(): Double {
    var l = loadUint8Long()
    l += loadUint8Long() shl 8
    l += loadUint8Long() shl 16
    l += loadUint8Long() shl 24
    l += loadUint8Long() shl 32
    l += loadUint8Long() shl 40
    l += loadUint8Long() shl 48
    l += loadUint8Long() shl 56
    return java.lang.Double.longBitsToDouble(l)
  }

  fun write(value: Float) {
    var fbit = java.lang.Float.floatToIntBits(value)
    saveUint8(fbit)
    fbit = fbit shr 8
    saveUint8(fbit)
    fbit = fbit shr 8
    saveUint8(fbit)
    fbit = fbit shr 8
    saveUint8(fbit)
  }

  fun write(value: Double) {
    var fbit = java.lang.Double.doubleToLongBits(value)
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
    fbit = fbit shr 8
    saveUint8(fbit.toInt())
  }

  fun checkReadSize(len: Int): Int {
    val sLen = readInt32()
    if (len > 0 && sLen > sLen) {
      throw RuntimeException("length too large.")
    }
    return sLen
  }

  fun readStringBySize(len: Int): String {
    val str = String(readBuffer, readLen, len, java.nio.charset.StandardCharsets.UTF_8)
    readLen += len
    return str
  }

  fun readString(len: Int = 0): String {
    val sLen = checkReadSize(len)
    return readStringBySize(sLen)
  }

  //@JvmOverloads
  fun writeString(str: String, len: Int = 0) {
    val sbuf = str.toByteArray(java.nio.charset.StandardCharsets.UTF_8)
    val slen = sbuf.size
    if (len in 1..(slen - 1)) {
      throw RuntimeException("length too large.")
    }
    write(slen)
    writeData(sbuf, slen)
  }

  fun skipWrite(len: Int) {
    writeLen += len
  }

  fun skipRead(len: Int) {
    readLen += len
  }

  fun skipReadScacleInt() {
    val tag = loadUint8()
    if (tag < TAG_AS_TYPE) return
    val readBytes = (tag and INTEGER_BYTE_MASK) + 1
    skipRead(readBytes)
  }

  fun fixSkipReadInt8() {
    skipRead(1)
  }

  fun fixSkipReadInt16() {
    skipRead(2)
  }

  fun fixSkipReadInt32() {
    skipRead(4)
  }

  fun fixSkipReadInt64() {
    skipRead(8)
  }

  fun skipReadInt8() {
    skipReadScacleInt()
  }

  fun skipReadInt16() {
    skipReadScacleInt()
  }

  fun skipReadInt32() {
    skipReadScacleInt()
  }

  fun skipReadInt64() {
    skipReadScacleInt()
  }

  fun skipReadFloat() {
    skipRead(4)
  }

  fun skipReadDouble() {
    skipRead(8)
  }

  fun skipReadString() {
    val slen = readInt32().toLong()
    skipRead(slen.toInt())
  }

  fun skipReadCompatible() {
    val offset = readLen
    @Suppress("UNUSED_VARIABLE")
    val tag = readInt64()
    val lenTag = readInt32()
    val readLen = (readLen - offset)
    if (lenTag > readLen) skipRead(lenTag - readLen)
  }
}

interface Base {
  fun read(stream: Stream)
  fun sizeOf(): Int
  fun write(stream: Stream)
  fun rawRead(stream: Stream)
  fun rawSizeOf(): Int
  fun rawWrite(stream: Stream)
}

fun checkWriteSize(vLen: Int, len: Int) {
  if (len > 0) {
    if (vLen > len) {
      throw RuntimeException("length too large.")
    }
  }
}

inline fun <reified T> readElement(stream: Stream): T where T : Base {
  val vv = T::class.primaryConstructor!!.call()
  vv.read(stream)
  return vv
}

inline fun <reified T> rawReadElement(stream: Stream): T where T : Base {
  val vv = T::class.primaryConstructor!!.call()
  vv.rawRead(stream)
  return vv
}

