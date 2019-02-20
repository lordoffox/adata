using System;
using System.Collections.Generic;
using adata;

namespace my{namespace game{
  public partial class Quest: BaseObj
  {
    public Int32 id = 0;
    public string name = "";
    public string description = "";
  }

  public partial class Quest
  {
    public override void Read(ZeroCopyBuffer stream)
    {
      int offset = stream.ReadLength();
      Int64 tag = 0;
      Stream.Read(stream,ref tag);
      Int32 len_tag = 0;
      Stream.Read(stream,ref len_tag);

      if((tag&1L)>0)      Stream.Read(stream,ref this.id);
      if((tag&2L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        Stream.Read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        Stream.Read(stream,ref this.description,len3);
      }
      if(len_tag >= 0)
      {
        Int32 read_len = stream.ReadLength() - offset;
        if(len_tag > read_len) stream.SkipRead(len_tag - read_len);
      }
    }

    public override Int32 SizeOf()
    {
      Int32 size = 0;
      Int64 tag = 1L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.description.Length > 0){tag|=4L;}
      size += Stream.SizeOf(this.id);
      if((tag&2L)>0)
      {
        size += Stream.SizeOf(this.name);
      }
      if((tag&4L)>0)
      {
        size += Stream.SizeOf(this.description);
      }
      size += Stream.SizeOf(tag);
      size += Stream.SizeOf(size + Stream.SizeOf(size));
      return size;
    }

    public override void Write(ZeroCopyBuffer stream)
    {
      Int64 tag = 1L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.description.Length > 0){tag|=4L;}
      Stream.Write(stream,tag);
      Stream.Write(stream,this.SizeOf());
      Stream.Write(stream,this.id);
      if((tag&2L)>0)      {
        Stream.Write(stream,this.name);
      }
      if((tag&4L)>0)      {
        Stream.Write(stream,this.description);
      }
    }

    public override void RawRead(ZeroCopyBuffer stream)
    {
      Stream.Read(stream,ref this.id);
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        Stream.Read(stream,ref this.name,len3);
      }
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        Stream.Read(stream,ref this.description,len3);
      }
    }

    public override Int32 RawSizeOf()
    {
      Int32 size = 0;
      size += Stream.SizeOf(this.id);
      size += Stream.SizeOf(this.name);
      size += Stream.SizeOf(this.description);
      return size;
    }

    public override void RawWrite(ZeroCopyBuffer stream)
    {
      Stream.Write(stream,this.id);
      {
        Stream.Write(stream,this.name);
      }
      {
        Stream.Write(stream,this.description);
      }
    }

    public override void Reset()
    {
      this.id = 0;
      this.name = "";
      this.description = "";
    }

  }

}}

