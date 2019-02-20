using System;
using System.Collections.Generic;
using adata;

namespace util{
  public partial class Vec3: BaseObj
  {
    public float x = 0.0F;
    public float y = 0.0F;
    public float z = 0.0F;
  }

  public partial class Vec3
  {
    public override void Read(ZeroCopyBuffer stream)
    {
      int offset = stream.ReadLength();
      Int64 tag = 0;
      Stream.Read(stream,ref tag);
      Int32 len_tag = 0;
      Stream.Read(stream,ref len_tag);

      if((tag&1L)>0)      Stream.Read(stream,ref this.x);
      if((tag&2L)>0)      Stream.Read(stream,ref this.y);
      if((tag&4L)>0)      Stream.Read(stream,ref this.z);
      if(len_tag >= 0)
      {
        Int32 read_len = stream.ReadLength() - offset;
        if(len_tag > read_len) stream.SkipRead(len_tag - read_len);
      }
    }

    public override Int32 SizeOf()
    {
      Int32 size = 0;
      Int64 tag = 7L;
      size += Stream.SizeOf(this.x);
      size += Stream.SizeOf(this.y);
      size += Stream.SizeOf(this.z);
      size += Stream.SizeOf(tag);
      size += Stream.SizeOf(size + Stream.SizeOf(size));
      return size;
    }

    public override void Write(ZeroCopyBuffer stream)
    {
      Int64 tag = 7L;
      Stream.Write(stream,tag);
      Stream.Write(stream,this.SizeOf());
      Stream.Write(stream,this.x);
      Stream.Write(stream,this.y);
      Stream.Write(stream,this.z);
    }

    public override void RawRead(ZeroCopyBuffer stream)
    {
      Stream.Read(stream,ref this.x);
      Stream.Read(stream,ref this.y);
      Stream.Read(stream,ref this.z);
    }

    public override Int32 RawSizeOf()
    {
      Int32 size = 0;
      size += Stream.SizeOf(this.x);
      size += Stream.SizeOf(this.y);
      size += Stream.SizeOf(this.z);
      return size;
    }

    public override void RawWrite(ZeroCopyBuffer stream)
    {
      Stream.Write(stream,this.x);
      Stream.Write(stream,this.y);
      Stream.Write(stream,this.z);
    }

    public override void Reset()
    {
      this.x = 0.0F;
      this.y = 0.0F;
      this.z = 0.0F;
    }

  }

}

