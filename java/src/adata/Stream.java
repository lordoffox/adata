package adata;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;

public class Stream {
  ByteBuffer m_read_buf;
  ByteBuffer m_write_buf;
  byte[] m_datas = new byte[9];
  
  final static int const_tag_as_value = 0x7f;
  final static int const_tag_as_type = 0x80;
  final static int const_interger_byte_msak = 0x1f;
  final static int const_negative_bit_value = 0x20;
  final static int const_store_postive_integer_byte_mask = 0x80 - 2;

  public ByteBuffer get_read_buffer(){
	return m_read_buf;
  }
  
  public void set_read_buffer(ByteBuffer buf){
	m_read_buf = buf;
  }
  
  public ByteBuffer get_write_buffer(){
	return m_write_buf;
  }
  
  public void set_write_buffer(ByteBuffer buf){
	m_write_buf = buf;
  }
  
  public void write_data(byte[] data , int len){
	m_write_buf.put(data,0,len);
  }
  
  public void clear_read(){
	m_read_buf.position(0);
  }
  
  public void clear_write(){
	m_write_buf.position(0);
  }
  
  public void clear(){
	clear_read();
	clear_write();
  }
  
  public int read_length(){
	return m_read_buf.position();
  }
  
  public int write_length(){
	return m_write_buf.position();
  }
  
  public static int fix_sizeof_int8(){
	return 1;
  }

  public static int fix_sizeof_int16(){
	return 2;
  }

  public static int fix_sizeof_int32(){
	return 4;
  }

  public static int fix_sizeof_int64(){
	return 8;
  }

  public byte fix_read_int8(){
	return m_read_buf.get();
  }
  
  public short fix_read_int16(){
	return m_read_buf.getShort();
  }
  
  public int fix_read_int32(){
	return m_read_buf.getInt();
  }
  
  public long fix_read_int64(){
	return m_read_buf.getLong();
  }

  public void fix_write_int8(byte val){
	m_write_buf.put(val);
  }
  
  public void fix_write_int16(short val){
	m_write_buf.putShort(val);
  }
  
  public void fix_write_int32(int val){
	m_write_buf.putInt(val);
  }
  
  public void fix_write_int64(long val){
	  m_write_buf.putLong(val);
  }
   
  public static int sizeof_int8(byte value){
	if(value >=0 && value < const_tag_as_type)return 1;
	return 2;
  }
  
  public static int sizeof_int16(short value){
    if (0 <= value && value < const_tag_as_type) return 1;
    if (value < 0) value = (short)(-value);
    if (value < 0x100) return 2;
    return 3;
  }
  
  public static int sizeof_int32(int value){
    if (0 <= value && value < const_tag_as_type) return 1;
    if (value < 0) value = (int)(-value);
    if (value < 0x100) return 2;
    if (value < 0x10000) return 3;
    if (value < 0x1000000) return 4;
    return 5;
  }

  public static int sizeof_int64(long value){
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

  public static int sizeof_float(float value){
	return 4;
  }
  
  public static int sizeof_double(double value){
	return 8;
  }
  
  static int uint8(byte v){
    if(v <0 )return 256 + v;
    return v;	  
  }
  
  int load_uint8()
  {
	return uint8(m_read_buf.get());
  }

  long read_scacle_int(int max_bytes) throws Exception{
	int tag = load_uint8();
	if(tag < const_tag_as_type){
	  return tag;
	}
	int read_bytes = (tag & const_interger_byte_msak) + 1;
	if(read_bytes > max_bytes)
	  throw new Exception("integer value out of range.");
	long value = 0;
	switch(read_bytes){
	case 1:{	  
	  value = load_uint8();
	  break;
	}
	case 2:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 3:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 4:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[3]);
	  value <<= 8; value += uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 5:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[4]);
	  value <<= 8; value += uint8(m_datas[3]);
	  value <<= 8; value += uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 6:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[5]);
	  value <<= 8; value += uint8(m_datas[4]);
	  value <<= 8; value += uint8(m_datas[3]);
	  value <<= 8; value += uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 7:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[6]);
	  value <<= 8; value += uint8(m_datas[5]);
	  value <<= 8; value += uint8(m_datas[4]);
	  value <<= 8; value += uint8(m_datas[3]);
	  value <<= 8; value += uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}
	case 8:{
	  m_read_buf.get(m_datas, 0, read_bytes);
	  value = uint8(m_datas[7]);
	  value <<= 8; value += uint8(m_datas[6]);
	  value <<= 8; value += uint8(m_datas[5]);
	  value <<= 8; value += uint8(m_datas[4]);
	  value <<= 8; value += uint8(m_datas[3]);
	  value <<= 8; value += uint8(m_datas[2]);
	  value <<= 8; value += uint8(m_datas[1]);
	  value <<= 8; value += uint8(m_datas[0]);
	  break;
	}	  
	}
	if((tag & const_negative_bit_value)>0){
		value = - value;
	}
	return value;
  }

  public byte read_int8() throws Exception{
	long value = read_scacle_int(1);
	return (byte)value;
  }

  public short read_int16() throws Exception{
	long value = read_scacle_int(2);
	return (short)value;
  }

  public int read_int32() throws Exception{
	long value = read_scacle_int(4);
	return (int)value;
  }

  public long read_int64() throws Exception{
	long value = read_scacle_int(8);
	return value;
  }

  void write_scacle_int(long value){
    if (0 <= value && value < const_tag_as_type){
      m_write_buf.put((byte)value);
      return;
    }
    m_datas[0] = (byte)(const_tag_as_type);
	if (value < 0){
	  value = (long)(-value);
	  m_datas[0] += const_negative_bit_value;
	}
	if (value < 0x100){
	  m_datas[1] = (byte)(value);
	  m_write_buf.put(m_datas,0,2);
	}
	else if (value < 0x10000){
	  m_datas[0]+=1;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  m_write_buf.put(m_datas,0,3);
	}
	else if (value < 0x1000000){
	  m_datas[0]+=2;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  m_write_buf.put(m_datas,0,4);
	}
	else if (value < 0x100000000L){
	  m_datas[0]+=3;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  value >>= 8; m_datas[4] = (byte)(value);
	  m_write_buf.put(m_datas,0,5);
	}
	else if (value < 0x10000000000L){
	  m_datas[0]+=4;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  value >>= 8; m_datas[4] = (byte)(value);
	  value >>= 8; m_datas[5] = (byte)(value);
	  m_write_buf.put(m_datas,0,6);
	}
	else if (value < 0x1000000000000L){
	  m_datas[0]+=4;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  value >>= 8; m_datas[4] = (byte)(value);
	  value >>= 8; m_datas[5] = (byte)(value);
	  value >>= 8; m_datas[6] = (byte)(value);
	  m_write_buf.put(m_datas,0,7);
	}
	else if (value < 0x100000000000000L){
	  m_datas[0]+=4;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  value >>= 8; m_datas[4] = (byte)(value);
	  value >>= 8; m_datas[5] = (byte)(value);
	  value >>= 8; m_datas[6] = (byte)(value);
	  value >>= 8; m_datas[7] = (byte)(value);
	  m_write_buf.put(m_datas,0,8);
	}
	else {
	  m_datas[0]+=4;
	  m_datas[1] = (byte)(value);
	  value >>= 8; m_datas[2] = (byte)(value);
	  value >>= 8; m_datas[3] = (byte)(value);
	  value >>= 8; m_datas[4] = (byte)(value);
	  value >>= 8; m_datas[5] = (byte)(value);
	  value >>= 8; m_datas[6] = (byte)(value);
	  value >>= 8; m_datas[7] = (byte)(value);
	  value >>= 8; m_datas[8] = (byte)(value);
	  m_write_buf.put(m_datas,0,9);
	}	
  }
  
  public void write_int8(byte value){
	write_scacle_int(value);
  }
  
  public void write_int16(short value){
	write_scacle_int(value);
  }
  
  public void write_int32(int value){
	write_scacle_int(value);
  }
  
  public void write_int64(long value){
	write_scacle_int(value);
  }
  
  public float read_float(){
	return m_read_buf.getFloat();
  }
  
  public double read_double(){
	return m_read_buf.getDouble();
  }
  
  public void write_float(float value){
	m_write_buf.putFloat(value);
  }

  public void write_double(double value){
	m_write_buf.putDouble(value);
  }
  
  static Charset cs_utf8 = java.nio.charset.StandardCharsets.UTF_8;
  static CharsetDecoder dc_utf8 = cs_utf8.newDecoder();
  static CharsetEncoder ec_utf8 = cs_utf8.newEncoder();
  
  public int check_read_size(int len) throws Exception{
	long slen = read_int64();
	if(len >0 && slen > slen){
      throw new Exception("length too large.");
	}
	return (int)slen;
  }
  
  public static int sizeof_string(String str){
	int len = str.getBytes(cs_utf8).length;
	len += sizeof_int32(len);	
	return len;
  }

  public String read_string(int len) throws Exception{
	byte[] string_buf = new byte[len];
	m_read_buf.get(string_buf,0,len);
	return new String( string_buf, cs_utf8 );
  }
  
  public void write_string(String str , int len) throws Exception{
	byte[] sbuf = str.getBytes(cs_utf8);
	int slen = sbuf.length;
	if(len > 0 && slen > len){
	  throw new Exception("length too large.");
	}
	write_int32(slen);
	m_write_buf.put(sbuf);
  }
  
  public void skip_read(int len)
  {
	m_read_buf.position(m_read_buf.position() + len);
  }
  
  void skip_read_scacle_int()
  {
	byte tag = m_read_buf.get();
	if(tag < const_tag_as_type) return;
	int read_bytes = (tag & const_interger_byte_msak) + 1;
	skip_read(read_bytes);
  }
  
  public void fix_skip_read_int8(){
	skip_read(1);
  }
  
  public void fix_skip_read_int16(){
	skip_read(2);
  }
  
  public void fix_skip_read_int32(){
	skip_read(4);
  }
  
  public void fix_skip_read_int64(){
	skip_read(8);
  }
  
  public void skip_read_int8(){
	skip_read_scacle_int();
  }
  
  public void skip_read_int16(){
	skip_read_scacle_int();
  }
  
  public void skip_read_int32(){
	skip_read_scacle_int();
  }
  
  public void skip_read_int64(){
	skip_read_scacle_int();
  }
  
  public void skip_read_float(){
	skip_read(4);
  }
  
  public void skip_read_double(){
	skip_read(8);
  }
  
  public void skip_read_string() throws Exception{
	long slen = read_int64();
	skip_read((int)slen);
  }
  
  public void skip_read_compatible() throws Exception
  {
    int offset = m_read_buf.position();
    @SuppressWarnings("unused")
	long tag = read_int64();
    Integer len_tag = read_int32();
    Integer read_len = (Integer)(m_read_buf.position() - offset);    
    if (len_tag > read_len) skip_read(len_tag - read_len);
  }
}
