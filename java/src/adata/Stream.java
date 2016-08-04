package adata;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;

public class Stream {
  ByteBuffer readBuffer;
  ByteBuffer writeBuffer;
  byte[] dataBytes = new byte[9];
  
  final static int const_tag_as_value = 0x7f;
  final static int const_tag_as_type = 0x80;
  final static int const_interger_byte_msak = 0x1f;
  final static int const_negative_bit_value = 0x20;
  final static int const_store_postive_integer_byte_mask = 0x80 - 2;

  public ByteBuffer getReadBuffer(){
	return readBuffer;
  }
  
  public void setReadBuffer(ByteBuffer buf){
	readBuffer = buf;
  }
  
  public ByteBuffer getWriteBuffer(){
	return writeBuffer;
  }
  
  public void setWriteBuffer(ByteBuffer buf){
	writeBuffer = buf;
  }
  
  public void writeData(byte[] data , int len){
	writeBuffer.put(data,0,len);
  }
  
  public void clearRead(){
	readBuffer.position(0);
  }
  
  public void clearWrite(){
	writeBuffer.position(0);
  }
  
  public void clear(){
	clearRead();
	clearWrite();
  }
  
  public int readLength(){
	return readBuffer.position();
  }
  
  public int writeLength(){
	return writeBuffer.position();
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
	return readBuffer.get();
  }
  
  public short fixReadInt16(){
	return readBuffer.getShort();
  }
  
  public int fixReadInt32(){
	return readBuffer.getInt();
  }
  
  public long fixReadInt64(){
	return readBuffer.getLong();
  }

  public void fixWriteInt8(byte val){
	writeBuffer.put(val);
  }
  
  public void fixWriteInt16(short val){
	writeBuffer.putShort(val);
  }
  
  public void fixWriteInt32(int val){
	writeBuffer.putInt(val);
  }
  
  public void fixWriteInt64(long val){
	  writeBuffer.putLong(val);
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
    if(v <0 )return 256 + v;
    return v;	  
  }
  
  int loadUint8()
  {
	return uint8(readBuffer.get());
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
	switch(read_bytes){
	case 1:{	  
	  value = loadUint8();
	  break;
	}
	case 2:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 3:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 4:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[3]);
	  value <<= 8; value += uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 5:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[4]);
	  value <<= 8; value += uint8(dataBytes[3]);
	  value <<= 8; value += uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 6:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[5]);
	  value <<= 8; value += uint8(dataBytes[4]);
	  value <<= 8; value += uint8(dataBytes[3]);
	  value <<= 8; value += uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 7:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[6]);
	  value <<= 8; value += uint8(dataBytes[5]);
	  value <<= 8; value += uint8(dataBytes[4]);
	  value <<= 8; value += uint8(dataBytes[3]);
	  value <<= 8; value += uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}
	case 8:{
	  readBuffer.get(dataBytes, 0, read_bytes);
	  value = uint8(dataBytes[7]);
	  value <<= 8; value += uint8(dataBytes[6]);
	  value <<= 8; value += uint8(dataBytes[5]);
	  value <<= 8; value += uint8(dataBytes[4]);
	  value <<= 8; value += uint8(dataBytes[3]);
	  value <<= 8; value += uint8(dataBytes[2]);
	  value <<= 8; value += uint8(dataBytes[1]);
	  value <<= 8; value += uint8(dataBytes[0]);
	  break;
	}	  
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
      writeBuffer.put((byte)value);
      return;
    }
    dataBytes[0] = (byte)(const_tag_as_type);
	if (value < 0){
	  value = (long)(-value);
	  dataBytes[0] += const_negative_bit_value;
	}
	if (value < 0x100){
	  dataBytes[1] = (byte)(value);
	  writeBuffer.put(dataBytes,0,2);
	}
	else if (value < 0x10000){
	  dataBytes[0]+=1;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  writeBuffer.put(dataBytes,0,3);
	}
	else if (value < 0x1000000){
	  dataBytes[0]+=2;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  writeBuffer.put(dataBytes,0,4);
	}
	else if (value < 0x100000000L){
	  dataBytes[0]+=3;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  value >>= 8; dataBytes[4] = (byte)(value);
	  writeBuffer.put(dataBytes,0,5);
	}
	else if (value < 0x10000000000L){
	  dataBytes[0]+=4;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  value >>= 8; dataBytes[4] = (byte)(value);
	  value >>= 8; dataBytes[5] = (byte)(value);
	  writeBuffer.put(dataBytes,0,6);
	}
	else if (value < 0x1000000000000L){
	  dataBytes[0]+=4;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  value >>= 8; dataBytes[4] = (byte)(value);
	  value >>= 8; dataBytes[5] = (byte)(value);
	  value >>= 8; dataBytes[6] = (byte)(value);
	  writeBuffer.put(dataBytes,0,7);
	}
	else if (value < 0x100000000000000L){
	  dataBytes[0]+=4;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  value >>= 8; dataBytes[4] = (byte)(value);
	  value >>= 8; dataBytes[5] = (byte)(value);
	  value >>= 8; dataBytes[6] = (byte)(value);
	  value >>= 8; dataBytes[7] = (byte)(value);
	  writeBuffer.put(dataBytes,0,8);
	}
	else {
	  dataBytes[0]+=4;
	  dataBytes[1] = (byte)(value);
	  value >>= 8; dataBytes[2] = (byte)(value);
	  value >>= 8; dataBytes[3] = (byte)(value);
	  value >>= 8; dataBytes[4] = (byte)(value);
	  value >>= 8; dataBytes[5] = (byte)(value);
	  value >>= 8; dataBytes[6] = (byte)(value);
	  value >>= 8; dataBytes[7] = (byte)(value);
	  value >>= 8; dataBytes[8] = (byte)(value);
	  writeBuffer.put(dataBytes,0,9);
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
	return readBuffer.getFloat();
  }
  
  public double readDouble(){
	return readBuffer.getDouble();
  }
  
  public void writeFloat(float value){
	writeBuffer.putFloat(value);
  }

  public void writeDouble(double value){
	writeBuffer.putDouble(value);
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
	byte[] string_buf = new byte[len];
	readBuffer.get(string_buf,0,len);
	return new String( string_buf, cs_utf8 );
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
	writeBuffer.put(sbuf);
  }
  
  public void writeString(String str){
	writeString(str,0);
  }
  
  public void skipRead(int len)
  {
	readBuffer.position(readBuffer.position() + len);
  }
  
  void skipReadScacleInt()
  {
	byte tag = readBuffer.get();
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
    int offset = readBuffer.position();
    @SuppressWarnings("unused")
	long tag = readInt64();
    Integer len_tag = readInt32();
    Integer read_len = (Integer)(readBuffer.position() - offset);    
    if (len_tag > read_len) skipRead(len_tag - read_len);
  }
}
