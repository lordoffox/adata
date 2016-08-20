package adata;
import java.nio.charset.Charset;

public class Stream {
  byte[] readBuffer;
  byte[] writeBuffer;
  int readLen = 0;
  int writeLen = 0;
  
  final static int const_tag_as_value = 0x7f;
  final static int const_tag_as_type = 0x80;
  final static int const_interger_byte_msak = 0x1f;
  final static int const_negative_bit_value = 0x20;
  final static int const_store_postive_integer_byte_mask = 0x80 - 2;
  
  public byte[] getReadBuffer(){
	return readBuffer;
  }
  
  public void setReadBuffer(byte[] buf){
	readBuffer = buf;
  }
  
  public byte[] getWriteBuffer(){
	return writeBuffer;
  }
  
  public void setWriteBuffer(byte[] buf){
	writeBuffer = buf;
  }
  
  public void writeData(byte[] data , int len){
	System.arraycopy(writeBuffer, writeLen, data, 0, len);
	writeLen += len;
  }
  
  public void clearRead(){
	readLen= 0;
  }
  
  public void clearWrite(){
	writeLen = 0;
  }
  
  public void clear(){
	clearRead();
	clearWrite();
  }
  
  public int readLength(){
	return readBuffer.length;
  }
  
  public int writeLength(){
	return writeBuffer.length;
  }
  
  public static int fixSizeOfInt8(){
	return 1;
  }

  public static int fixSizeOfInt16(){
	return 2;
  }

  public static int fixSizeOfInt32(){
	return 4;
  }

  public static int fixSizeOfInt64(){
	return 8;
  }

  public byte fixReadInt8(){
	return readBuffer[readLen++];
  }
  
  public short fixReadInt16(){
	int v = readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	return (short)v;
  }
  
  public int fixReadInt32(){
	int v = readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	return v;
  }
  
  public long fixReadInt64(){
	long v = readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	v <<= 8;
	v += readBuffer[readLen++] & 0xff;
	return v;
  }

  public void fixWriteInt8(byte val){
	writeBuffer[writeLen++] = val;
  }
  
  public void fixWriteInt16(short val){
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
  }
  
  public void fixWriteInt32(int val){
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
  }
  
  public void fixWriteInt64(long val){
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
	val >>= 8;
	writeBuffer[writeLen++] = (byte)(val&0xff);
  }
   
  public static int sizeOfInt8(byte value){
	if(value >=0 && value < const_tag_as_type)return 1;
	return 2;
  }
  
  public static int sizeOfInt16(short value){
    if (0 <= value && value < const_tag_as_type) return 1;
    if (value < 0) value = (short)(-value);
    if (value < 0x100) return 2;
    return 3;
  }
  
  public static int sizeOfInt32(int value){
    if (0 <= value && value < const_tag_as_type) return 1;
    if (value < 0) value = (int)(-value);
    if (value < 0x100) return 2;
    if (value < 0x10000) return 3;
    if (value < 0x1000000) return 4;
    return 5;
  }

  public static int sizeOfInt64(long value){
	if (0 <= value && value < const_tag_as_type) return 1;
	if (value < 0) value = (long)(-value);
	if (value < 0x100) return 2;
	else if (value < 0x10000) return 3;
	else if (value < 0x1000000) return 4;
	else if (value < 0x100000000L) return 5;
	else if (value < 0x10000000000L) return 6;
	else if (value < 0x1000000000000L) return 7;
	else if (value < 0x100000000000000L) return 8;
	return 9;
  }

  public static int sizeOfFloat(float value){
	return 4;
  }
  
  public static int sizeOfDouble(double value){
	return 8;
  }
  
  static int uint8(byte v){
    return v & 0xff;	  
  }
  
  int loadUint8(){
	return readBuffer[readLen++] & 0xff;
  }

  long loadUint8Long(){
	return readBuffer[readLen++] & 0xff;
  }

  void saveUint8(int val){
	writeBuffer[writeLen++] = (byte)(val & 0xff);
  }
  
  long readScacleInt(int max_bytes){
	int tag = loadUint8();
	if(tag < const_tag_as_type){
	  return tag;
	}
	int read_bytes = (tag & const_interger_byte_msak) + 1;
	if(read_bytes > max_bytes)
	  throw new RuntimeException("integer value out of range.");
	long value = 0;
	int bit = 0;
	for(int i = 0 ; i < read_bytes; ++i , bit += 8){
	  value += loadUint8Long() << bit;
	}
	if((tag & const_negative_bit_value)>0){
		value = - value;
	}
	return value;
  }

  public byte readInt8(){
	long value = readScacleInt(1);
	return (byte)value;
  }

  public short readInt16(){
	long value = readScacleInt(2);
	return (short)value;
  }

  public int readInt32(){
	long value = readScacleInt(4);
	return (int)value;
  }

  public long readInt64(){
	long value = readScacleInt(8);
	return value;
  }

  void writeScacleInt(long value){
    if (0 <= value && value < const_tag_as_type){
      saveUint8((int)value);
      return;
    }
    int tag = (const_tag_as_type);
	if (value < 0){
	  value = (long)(-value);
	  tag += const_negative_bit_value;
	}
	if (value < 0x100){
	  saveUint8(tag);
	  saveUint8((int)value);
	}
	else if (value < 0x10000){
	  saveUint8(tag+1);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else if (value < 0x1000000){
	  saveUint8(tag+2);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else if (value < 0x100000000L){
	  saveUint8(tag+3);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else if (value < 0x10000000000L){
	  saveUint8(tag+4);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else if (value < 0x1000000000000L){
	  saveUint8(tag+5);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else if (value < 0x100000000000000L){
	  saveUint8(tag+6);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}
	else {
	  saveUint8(tag+7);
	  saveUint8((int)value);
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	  value >>= 8;
	  saveUint8((int)(value));
	}	
  }
  
  public void writeInt8(byte value){
	writeScacleInt(value);
  }
  
  public void writeInt16(short value){
	writeScacleInt(value);
  }
  
  public void writeInt32(int value){
	writeScacleInt(value);
  }
  
  public void writeInt64(long value){
	writeScacleInt(value);
  }
  
  public float readFloat(){
    int l = loadUint8();
    l += loadUint8() << 8;
    l += loadUint8() << 16;
    l += loadUint8() << 24;
    return Float.intBitsToFloat(l);
  }
  
  public double readDouble(){
    long l = loadUint8Long();
    l += loadUint8Long() << 8;
    l += loadUint8Long() << 16;
    l += loadUint8Long() << 24;
    l += loadUint8Long() << 32;
    l += loadUint8Long() << 40;
    l += loadUint8Long() << 48;
    l += loadUint8Long() << 56;
    return Double.longBitsToDouble(l);
  }
  
  public void writeFloat(float value){
	int fbit = Float.floatToIntBits(value);  
    saveUint8(fbit);
    fbit >>= 8;
    saveUint8(fbit);
    fbit >>= 8;
    saveUint8(fbit);
    fbit >>= 8;
    saveUint8(fbit);
  }

  public void writeDouble(double value){
	long fbit = Double.doubleToLongBits(value);  
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
    fbit >>= 8;
    saveUint8((int)fbit);
  }
  
  static Charset cs_utf8 = java.nio.charset.StandardCharsets.UTF_8;
  
  public int checkReadSize(int len){
	long slen = readInt64();
	if(len >0 && slen > slen){
      throw new RuntimeException("length too large.");
	}
	return (int)slen;
  }
  
  public static int sizeOfString(String str){
	int len = str.getBytes(cs_utf8).length;
	len += sizeOfInt32(len);	
	return len;
  }

  public String readString(int len){
	String str = new String( readBuffer , readLen , len , cs_utf8 );
	readLen += len;
	return str;
  }
  
  public String readString(){
	int len = readInt32();
	return readString(len);
  }
  
  public void writeString(String str , int len){
	byte[] sbuf = str.getBytes(cs_utf8);
	int slen = sbuf.length;
	if(len > 0 && slen > len){
	  throw new RuntimeException("length too large.");
	}
	writeInt32(slen);
	writeData(sbuf,slen);
  }
  
  public void writeString(String str){
	writeString(str,0);
  }
  
  public void skipRead(int len)
  {
	readLen += len;
  }
  
  void skipReadScacleInt()
  {
	int tag = loadUint8();
	if(tag < const_tag_as_type) return;
	int read_bytes = (tag & const_interger_byte_msak) + 1;
	skipRead(read_bytes);
  }
  
  public void fixSkipReadInt8(){
	skipRead(1);
  }
  
  public void fixSkipReadInt16(){
	skipRead(2);
  }
  
  public void fixSkipReadInt32(){
	skipRead(4);
  }
  
  public void fixSkipReadInt64(){
	skipRead(8);
  }
  
  public void skipReadInt8(){
	skipReadScacleInt();
  }
  
  public void skipReadInt16(){
	skipReadScacleInt();
  }
  
  public void skipReadInt32(){
	skipReadScacleInt();
  }
  
  public void skipReadInt64(){
	skipReadScacleInt();
  }
  
  public void skipReadFloat(){
	skipRead(4);
  }
  
  public void skipReadDouble(){
	skipRead(8);
  }
  
  public void skipReadString(){
	long slen = readInt32();
	skipRead((int)slen);
  }
  
  public void skipReadCompatible()
  {
    int offset = readLen;
    @SuppressWarnings("unused")
	long tag = readInt64();
    Integer len_tag = readInt32();
    Integer read_len = (Integer)(readLen - offset);    
    if (len_tag > read_len) skipRead(len_tag - read_len);
  }
}
