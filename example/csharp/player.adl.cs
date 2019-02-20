using System;
using System.Collections.Generic;
using adata;

namespace my{namespace game{
  public partial class Item: BaseObj
  {
    public Int32 type = 0;
    public Int32 level = 0;
    public Int64 id = 0;
  }

  public partial class PlayerV1: BaseObj
  {
    public Int32 id = 0;
    public Int32 age = 0;
    public float factor = 1.0F;
    public string name = "";
    public util.Vec3 pos = new util.Vec3();
    public List<Item> inventory = new List<Item>();
    public List<my.game.Quest> quests = new List<my.game.Quest>();
  }

  public partial class PlayerV2: BaseObj
  {
    public Int32 id = 0;
    //age deleted , skip define.
    //factor deleted , skip define.
    public string name = "";
    public util.Vec3 pos = new util.Vec3();
    public List<Item> inventory = new List<Item>();
    public List<my.game.Quest> quests = new List<my.game.Quest>();
    public List<Int32> friends = new List<Int32>();
  }

  public partial class Item
  {
    public override void Read(ZeroCopyBuffer stream)
    {
      int offset = stream.ReadLength();
      Int64 tag = 0;
      Stream.Read(stream,ref tag);
      Int32 len_tag = 0;
      Stream.Read(stream,ref len_tag);

      if((tag&1L)>0)      Stream.Read(stream,ref this.id);
      if((tag&2L)>0)      Stream.Read(stream,ref this.type);
      if((tag&4L)>0)      Stream.Read(stream,ref this.level);
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
      size += Stream.SizeOf(this.id);
      size += Stream.SizeOf(this.type);
      size += Stream.SizeOf(this.level);
      size += Stream.SizeOf(tag);
      size += Stream.SizeOf(size + Stream.SizeOf(size));
      return size;
    }

    public override void Write(ZeroCopyBuffer stream)
    {
      Int64 tag = 7L;
      Stream.Write(stream,tag);
      Stream.Write(stream,this.SizeOf());
      Stream.Write(stream,this.id);
      Stream.Write(stream,this.type);
      Stream.Write(stream,this.level);
    }

    public override void RawRead(ZeroCopyBuffer stream)
    {
      Stream.Read(stream,ref this.id);
      Stream.Read(stream,ref this.type);
      Stream.Read(stream,ref this.level);
    }

    public override Int32 RawSizeOf()
    {
      Int32 size = 0;
      size += Stream.SizeOf(this.id);
      size += Stream.SizeOf(this.type);
      size += Stream.SizeOf(this.level);
      return size;
    }

    public override void RawWrite(ZeroCopyBuffer stream)
    {
      Stream.Write(stream,this.id);
      Stream.Write(stream,this.type);
      Stream.Write(stream,this.level);
    }

    public override void Reset()
    {
      this.id = 0;
      this.type = 0;
      this.level = 0;
    }

  }

  public partial class PlayerV1
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
        Int32 len3 = Stream.CheckReadSize(stream,30);
        Stream.Read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      Stream.Read(stream,ref this.age);
      if((tag&8L)>0)      this.pos.Read(stream);
      if((tag&16L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Item element= new Item(); 
          element.Read(stream);
          this.inventory.Add(element);
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.Quest element= new my.game.Quest(); 
          element.Read(stream);
          this.quests.Add(element);
        }
      }
      if((tag&64L)>0)      Stream.Read(stream,ref this.factor);
      if(len_tag >= 0)
      {
        Int32 read_len = stream.ReadLength() - offset;
        if(len_tag > read_len) stream.SkipRead(len_tag - read_len);
      }
    }

    public override Int32 SizeOf()
    {
      Int32 size = 0;
      Int64 tag = 77L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      size += Stream.SizeOf(this.id);
      if((tag&2L)>0)
      {
        size += Stream.SizeOf(this.name);
      }
      size += Stream.SizeOf(this.age);
      size += this.pos.SizeOf();
      if((tag&16L)>0)
      {
        Int32 len4 = this.inventory.Count;
        size += Stream.SizeOf(len4);
        foreach (Item i in this.inventory)
        {
          size += i.SizeOf();
        }
      }
      if((tag&32L)>0)
      {
        Int32 len4 = this.quests.Count;
        size += Stream.SizeOf(len4);
        foreach (my.game.Quest i in this.quests)
        {
          size += i.SizeOf();
        }
      }
      size += Stream.SizeOf(this.factor);
      size += Stream.SizeOf(tag);
      size += Stream.SizeOf(size + Stream.SizeOf(size));
      return size;
    }

    public override void Write(ZeroCopyBuffer stream)
    {
      Int64 tag = 77L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      Stream.Write(stream,tag);
      Stream.Write(stream,this.SizeOf());
      Stream.Write(stream,this.id);
      if((tag&2L)>0)      {
        Stream.Write(stream,this.name);
      }
      Stream.Write(stream,this.age);
      this.pos.Write(stream);
      if((tag&16L)>0)      {
        Int32 len3 = this.inventory.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Item i in this.inventory)
        {
          i.Write(stream);
          ++count;
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = this.quests.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (my.game.Quest i in this.quests)
        {
          i.Write(stream);
          ++count;
        }
      }
      Stream.Write(stream,this.factor);
    }

    public override void RawRead(ZeroCopyBuffer stream)
    {
      Stream.Read(stream,ref this.id);
      {
        Int32 len3 = Stream.CheckReadSize(stream,30);
        Stream.Read(stream,ref this.name,len3);
      }
      Stream.Read(stream,ref this.age);
      this.pos.RawRead(stream);
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Item element= new Item(); 
          element.RawRead(stream);
          this.inventory.Add(element);
        }
      }
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.Quest element= new my.game.Quest(); 
          element.RawRead(stream);
          this.quests.Add(element);
        }
      }
      Stream.Read(stream,ref this.factor);
    }

    public override Int32 RawSizeOf()
    {
      Int32 size = 0;
      size += Stream.SizeOf(this.id);
      size += Stream.SizeOf(this.name);
      size += Stream.SizeOf(this.age);
      size += this.pos.RawSizeOf();
      size += Stream.SizeOf(this.inventory.Count); 
      foreach (Item i in this.inventory)
      {
        size += i.RawSizeOf();
      }
      size += Stream.SizeOf(this.quests.Count); 
      foreach (my.game.Quest i in this.quests)
      {
        size += i.RawSizeOf();
      }
      size += Stream.SizeOf(this.factor);
      return size;
    }

    public override void RawWrite(ZeroCopyBuffer stream)
    {
      Stream.Write(stream,this.id);
      {
        Stream.Write(stream,this.name);
      }
      Stream.Write(stream,this.age);
      this.pos.RawWrite(stream);
      {
        Int32 len3 = this.inventory.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Item i in this.inventory)
        {
          i.RawWrite(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.quests.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (my.game.Quest i in this.quests)
        {
          i.RawWrite(stream);
          ++count;
        }
      }
      Stream.Write(stream,this.factor);
    }

    public override void Reset()
    {
      this.id = 0;
      this.name = "";
      this.age = 0;
      this.pos.Reset();
      this.inventory.Clear();
      this.quests.Clear();
      this.factor = 1.0F;
    }

  }

  public partial class PlayerV2
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
        Int32 len3 = Stream.CheckReadSize(stream,30);
        Stream.Read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      {Int32 dummy_value = 0;Stream.SkipRead(stream,ref dummy_value);}
      if((tag&8L)>0)      this.pos.Read(stream);
      if((tag&16L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Item element= new Item(); 
          element.Read(stream);
          this.inventory.Add(element);
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.Quest element= new my.game.Quest(); 
          element.Read(stream);
          this.quests.Add(element);
        }
      }
      if((tag&64L)>0)      {float dummy_value = 0;Stream.SkipRead(stream,ref dummy_value);}
      if((tag&128L)>0)      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.friends.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Int32 element= 0; 
          Stream.Read(stream,ref element);
          this.friends.Add(element);
        }
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
      Int64 tag = 9L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      if(this.friends.Count > 0){tag|=128L;}
      size += Stream.SizeOf(this.id);
      if((tag&2L)>0)
      {
        size += Stream.SizeOf(this.name);
      }
      //this.age deleted , skip write.
      size += this.pos.SizeOf();
      if((tag&16L)>0)
      {
        Int32 len4 = this.inventory.Count;
        size += Stream.SizeOf(len4);
        foreach (Item i in this.inventory)
        {
          size += i.SizeOf();
        }
      }
      if((tag&32L)>0)
      {
        Int32 len4 = this.quests.Count;
        size += Stream.SizeOf(len4);
        foreach (my.game.Quest i in this.quests)
        {
          size += i.SizeOf();
        }
      }
      //this.factor deleted , skip write.
      if((tag&128L)>0)
      {
        Int32 len4 = this.friends.Count;
        size += Stream.SizeOf(len4);
        foreach (Int32 i in this.friends)
        {
          size += Stream.SizeOf(i);
        }
      }
      size += Stream.SizeOf(tag);
      size += Stream.SizeOf(size + Stream.SizeOf(size));
      return size;
    }

    public override void Write(ZeroCopyBuffer stream)
    {
      Int64 tag = 9L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      if(this.friends.Count > 0){tag|=128L;}
      Stream.Write(stream,tag);
      Stream.Write(stream,this.SizeOf());
      Stream.Write(stream,this.id);
      if((tag&2L)>0)      {
        Stream.Write(stream,this.name);
      }
    //this.age deleted , skip write.
      this.pos.Write(stream);
      if((tag&16L)>0)      {
        Int32 len3 = this.inventory.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Item i in this.inventory)
        {
          i.Write(stream);
          ++count;
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = this.quests.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (my.game.Quest i in this.quests)
        {
          i.Write(stream);
          ++count;
        }
      }
    //this.factor deleted , skip write.
      if((tag&128L)>0)      {
        Int32 len3 = this.friends.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Int32 i in this.friends)
        {
          Stream.Write(stream,i);
          ++count;
        }
      }
    }

    public override void RawRead(ZeroCopyBuffer stream)
    {
      Stream.Read(stream,ref this.id);
      {
        Int32 len3 = Stream.CheckReadSize(stream,30);
        Stream.Read(stream,ref this.name,len3);
      }
      this.pos.RawRead(stream);
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Item element= new Item(); 
          element.RawRead(stream);
          this.inventory.Add(element);
        }
      }
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.Quest element= new my.game.Quest(); 
          element.RawRead(stream);
          this.quests.Add(element);
        }
      }
      {
        Int32 len3 = Stream.CheckReadSize(stream);
        this.friends.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Int32 element= 0; 
          Stream.Read(stream,ref element);
          this.friends.Add(element);
        }
      }
    }

    public override Int32 RawSizeOf()
    {
      Int32 size = 0;
      size += Stream.SizeOf(this.id);
      size += Stream.SizeOf(this.name);
      size += this.pos.RawSizeOf();
      size += Stream.SizeOf(this.inventory.Count); 
      foreach (Item i in this.inventory)
      {
        size += i.RawSizeOf();
      }
      size += Stream.SizeOf(this.quests.Count); 
      foreach (my.game.Quest i in this.quests)
      {
        size += i.RawSizeOf();
      }
      size += Stream.SizeOf(this.friends.Count); 
      foreach (Int32 i in this.friends)
      {
        size += Stream.SizeOf(i);
      }
      return size;
    }

    public override void RawWrite(ZeroCopyBuffer stream)
    {
      Stream.Write(stream,this.id);
      {
        Stream.Write(stream,this.name);
      }
      this.pos.RawWrite(stream);
      {
        Int32 len3 = this.inventory.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Item i in this.inventory)
        {
          i.RawWrite(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.quests.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (my.game.Quest i in this.quests)
        {
          i.RawWrite(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.friends.Count;
        Stream.Write(stream,len3);
        int count = 0;
        foreach (Int32 i in this.friends)
        {
          Stream.Write(stream,i);
          ++count;
        }
      }
    }

    public override void Reset()
    {
      this.id = 0;
      this.name = "";
      this.pos.Reset();
      this.inventory.Clear();
      this.quests.Clear();
      this.friends.Clear();
    }

  }

}}

