using System;
using System.IO;
using System.Collections.Generic;
using adata;

namespace util {
  public partial class vec3: adata.base_obj
  {
    public float x = 0.0F;
    public float y = 0.0F;
    public float z = 0.0F;
  }

  public partial class vec3
  {
    public override void read(adata.zero_copy_buffer stream)
    {
      int offset = stream.read_length();
      Int64 tag = 0;
      adata.stream.read(stream,ref tag);
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);

      if((tag&1L)>0)      adata.stream.read(stream,ref this.x);
      if((tag&2L)>0)      adata.stream.read(stream,ref this.y);
      if((tag&4L)>0)      adata.stream.read(stream,ref this.z);
      if(len_tag >= 0)
      {
        Int32 read_len = stream.read_length() - offset;
        if(len_tag > read_len) stream.skip_read((UInt32)(len_tag - read_len));
      }
    }

    public override Int32 size_of()
    {
      Int32 size = 0;
      Int64 tag = 7L;
      size += adata.stream.size_of(this.x);
      size += adata.stream.size_of(this.y);
      size += adata.stream.size_of(this.z);
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public override void write(adata.zero_copy_buffer stream)
    {
      Int64 tag = 7L;
      adata.stream.write(stream,tag);
      adata.stream.write(stream,this.size_of());
      adata.stream.write(stream,this.x);
      adata.stream.write(stream,this.y);
      adata.stream.write(stream,this.z);
    }

    public override void raw_read(adata.zero_copy_buffer stream)
    {
      adata.stream.read(stream,ref this.x);
      adata.stream.read(stream,ref this.y);
      adata.stream.read(stream,ref this.z);
    }

    public override Int32 raw_size_of()
    {
      Int32 size = 0;
      size += adata.stream.size_of(this.x);
      size += adata.stream.size_of(this.y);
      size += adata.stream.size_of(this.z);
      return size;
    }

    public override void raw_write(adata.zero_copy_buffer stream)
    {
      adata.stream.write(stream,this.x);
      adata.stream.write(stream,this.y);
      adata.stream.write(stream,this.z);
    }

  }

}

