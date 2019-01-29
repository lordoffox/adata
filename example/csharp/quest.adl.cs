using System;
using System.IO;
using System.Collections.Generic;
using adata;

namespace my {namespace game {
  public partial class quest: adata.base_obj
  {
    public Int32 id = 0;
    public string name = "";
    public string description = "";
  }

  public partial class quest
  {
    public override void read(adata.zero_copy_buffer stream)
    {
      int offset = stream.read_length();
      Int64 tag = 0;
      adata.stream.read(stream,ref tag);
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);

      if((tag&1L)>0)      adata.stream.read(stream,ref this.id);
      if((tag&2L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        adata.stream.read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        adata.stream.read(stream,ref this.description,len3);
      }
      if(len_tag >= 0)
      {
        Int32 read_len = stream.read_length() - offset;
        if(len_tag > read_len) stream.skip_read((UInt32)(len_tag - read_len));
      }
    }

    public override Int32 size_of()
    {
      Int32 size = 0;
      Int64 tag = 1L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.description.Length > 0){tag|=4L;}
      size += adata.stream.size_of(this.id);
      if((tag&2L)>0)
      {
        size += adata.stream.size_of(this.name);
      }
      if((tag&4L)>0)
      {
        size += adata.stream.size_of(this.description);
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public override void write(adata.zero_copy_buffer stream)
    {
      Int64 tag = 1L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.description.Length > 0){tag|=4L;}
      adata.stream.write(stream,tag);
      adata.stream.write(stream,this.size_of());
      adata.stream.write(stream,this.id);
      if((tag&2L)>0)      {
        adata.stream.write(stream,this.name);
      }
      if((tag&4L)>0)      {
        adata.stream.write(stream,this.description);
      }
    }

    public override void raw_read(adata.zero_copy_buffer stream)
    {
      adata.stream.read(stream,ref this.id);
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        adata.stream.read(stream,ref this.name,len3);
      }
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        adata.stream.read(stream,ref this.description,len3);
      }
    }

    public override Int32 raw_size_of()
    {
      Int32 size = 0;
      size += adata.stream.size_of(this.id);
      size += adata.stream.size_of(this.name);
      size += adata.stream.size_of(this.description);
      return size;
    }

    public override void raw_write(adata.zero_copy_buffer stream)
    {
      adata.stream.write(stream,this.id);
      {
        adata.stream.write(stream,this.name);
      }
      {
        adata.stream.write(stream,this.description);
      }
    }

  }

}}

