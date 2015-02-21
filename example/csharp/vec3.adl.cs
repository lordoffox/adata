using System;
using System.IO;
using System.Collections.Generic;

namespace util {
  public class vec3
  {
    public float x = 0.0F;
    public float y = 0.0F;
    public float z = 0.0F;
  }

class vec3_stream
{
    public static void read(adata.zero_copy_buffer stream, ref vec3 value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {adata.stream.read(stream,ref value.x);{if(stream.error()){stream.trace_error("x",-1);return;}}}
      if((tag&2)>0)      {adata.stream.read(stream,ref value.y);{if(stream.error()){stream.trace_error("y",-1);return;}}}
      if((tag&4)>0)      {adata.stream.read(stream,ref value.z);{if(stream.error()){stream.trace_error("z",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static void skip_read(adata.zero_copy_buffer stream, ref vec3 value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("x",-1);return;}}}
      if((tag&2)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("y",-1);return;}}}
      if((tag&4)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("z",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static Int32 size_of(vec3 value)
    {
      Int32 size = 0;
      UInt64 tag = 7;
      {
        size += adata.stream.size_of(value.x);
      }
      {
        size += adata.stream.size_of(value.y);
      }
      {
        size += adata.stream.size_of(value.z);
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public static void write(adata.zero_copy_buffer stream , vec3 value)
    {
      UInt64 tag = 7;
      adata.stream.write(stream,tag);
      if(stream.error()){ return; }
      adata.stream.write(stream,size_of(value));
      if(stream.error()){return;}
      {adata.stream.write(stream,value.x);{if(stream.error()){stream.trace_error("x",-1);return;}}}
      {adata.stream.write(stream,value.y);{if(stream.error()){stream.trace_error("y",-1);return;}}}
      {adata.stream.write(stream,value.z);{if(stream.error()){stream.trace_error("z",-1);return;}}}
      return;
    }

  }


}

