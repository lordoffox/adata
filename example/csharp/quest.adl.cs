using System;
using System.IO;
using System.Collections.Generic;

namespace my {namespace game {
  public class quest
  {
    public Int32 id = 0;
    public string name = "";
    public string description = "";
  }

class quest_stream
{
    public static void read(adata.zero_copy_buffer stream, ref quest value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {adata.stream.read(stream,ref value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      if((tag&2)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        adata.stream.read(stream,ref value.name,len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("description",-1);return;}}
        adata.stream.read(stream,ref value.description,len3);
        {if(stream.error()){stream.trace_error("description",-1);return;}}
      }
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static void skip_read(adata.zero_copy_buffer stream, ref quest value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      if((tag&2)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        stream.skip_read(len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("description",-1);return;}}
        stream.skip_read(len3);
        {if(stream.error()){stream.trace_error("description",-1);return;}}
      }
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static Int32 size_of(quest value)
    {
      Int32 size = 0;
      UInt64 tag = 1;
      if(value.name.Length > 0){tag|=2;}
      if(value.description.Length > 0){tag|=4;}
      {
        size += adata.stream.size_of(value.id);
      }
      if((tag&2)>0)
      {
        UInt32 len4 = (UInt32)value.name.Length;
        size += adata.stream.size_of(len4);
        size += (Int32)len4;
      }
      if((tag&4)>0)
      {
        UInt32 len4 = (UInt32)value.description.Length;
        size += adata.stream.size_of(len4);
        size += (Int32)len4;
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public static void write(adata.zero_copy_buffer stream , quest value)
    {
      UInt64 tag = 1;
      if(value.name.Length > 0){tag|=2;}
      if(value.description.Length > 0){tag|=4;}
      adata.stream.write(stream,tag);
      if(stream.error()){ return; }
      adata.stream.write(stream,size_of(value));
      if(stream.error()){return;}
      {adata.stream.write(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      if((tag&2)>0)
      {
        UInt32 len3 = (UInt32)value.name.Length;
        adata.stream.write(stream,len3);
        adata.stream.write(stream,value.name,len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)
      {
        UInt32 len3 = (UInt32)value.description.Length;
        adata.stream.write(stream,len3);
        adata.stream.write(stream,value.description,len3);
        {if(stream.error()){stream.trace_error("description",-1);return;}}
      }
      return;
    }

  }


}}

