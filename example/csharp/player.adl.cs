using System;
using System.IO;
using System.Collections.Generic;
using adata;

namespace my {namespace game {
  public partial class item: adata.base_obj
  {
    public Int32 type = 0;
    public Int32 level = 0;
    public Int64 id = 0;
  }

  public partial class player_v1: adata.base_obj
  {
    public Int32 id = 0;
    public Int32 age = 0;
    public float factor = 1.0F;
    public string name = "";
    public util.vec3 pos = new util.vec3();
    public List<item> inventory = new List<item>();
    public List<my.game.quest> quests = new List<my.game.quest>();
  }

  public partial class player_v2: adata.base_obj
  {
    public Int32 id = 0;
    //age deleted , skip define.
    //factor deleted , skip define.
    public string name = "";
    public util.vec3 pos = new util.vec3();
    public List<item> inventory = new List<item>();
    public List<my.game.quest> quests = new List<my.game.quest>();
    public List<Int32> friends = new List<Int32>();
  }

  public partial class item
  {
    public override void read(adata.zero_copy_buffer stream)
    {
      int offset = stream.read_length();
      Int64 tag = 0;
      adata.stream.read(stream,ref tag);
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);

      if((tag&1L)>0)      adata.stream.read(stream,ref this.id);
      if((tag&2L)>0)      adata.stream.read(stream,ref this.type);
      if((tag&4L)>0)      adata.stream.read(stream,ref this.level);
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
      size += adata.stream.size_of(this.id);
      size += adata.stream.size_of(this.type);
      size += adata.stream.size_of(this.level);
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public override void write(adata.zero_copy_buffer stream)
    {
      Int64 tag = 7L;
      adata.stream.write(stream,tag);
      adata.stream.write(stream,this.size_of());
      adata.stream.write(stream,this.id);
      adata.stream.write(stream,this.type);
      adata.stream.write(stream,this.level);
    }

    public override void raw_read(adata.zero_copy_buffer stream)
    {
      adata.stream.read(stream,ref this.id);
      adata.stream.read(stream,ref this.type);
      adata.stream.read(stream,ref this.level);
    }

    public override Int32 raw_size_of()
    {
      Int32 size = 0;
      size += adata.stream.size_of(this.id);
      size += adata.stream.size_of(this.type);
      size += adata.stream.size_of(this.level);
      return size;
    }

    public override void raw_write(adata.zero_copy_buffer stream)
    {
      adata.stream.write(stream,this.id);
      adata.stream.write(stream,this.type);
      adata.stream.write(stream,this.level);
    }

  }

  public partial class player_v1
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
        Int32 len3 = adata.stream.check_read_size(stream,30);
        adata.stream.read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      adata.stream.read(stream,ref this.age);
      if((tag&8L)>0)      this.pos.read(stream);
      if((tag&16L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          element.read(stream);
          this.inventory.Add(element);
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          element.read(stream);
          this.quests.Add(element);
        }
      }
      if((tag&64L)>0)      adata.stream.read(stream,ref this.factor);
      if(len_tag >= 0)
      {
        Int32 read_len = stream.read_length() - offset;
        if(len_tag > read_len) stream.skip_read((UInt32)(len_tag - read_len));
      }
    }

    public override Int32 size_of()
    {
      Int32 size = 0;
      Int64 tag = 77L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      size += adata.stream.size_of(this.id);
      if((tag&2L)>0)
      {
        size += adata.stream.size_of(this.name);
      }
      size += adata.stream.size_of(this.age);
      size += this.pos.size_of();
      if((tag&16L)>0)
      {
        Int32 len4 = this.inventory.Count;
        size += adata.stream.size_of(len4);
        foreach (item i in this.inventory)
        {
          size += i.size_of();
        }
      }
      if((tag&32L)>0)
      {
        Int32 len4 = this.quests.Count;
        size += adata.stream.size_of(len4);
        foreach (my.game.quest i in this.quests)
        {
          size += i.size_of();
        }
      }
      size += adata.stream.size_of(this.factor);
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public override void write(adata.zero_copy_buffer stream)
    {
      Int64 tag = 77L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      adata.stream.write(stream,tag);
      adata.stream.write(stream,this.size_of());
      adata.stream.write(stream,this.id);
      if((tag&2L)>0)      {
        adata.stream.write(stream,this.name);
      }
      adata.stream.write(stream,this.age);
      this.pos.write(stream);
      if((tag&16L)>0)      {
        Int32 len3 = this.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in this.inventory)
        {
          i.write(stream);
          ++count;
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = this.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in this.quests)
        {
          i.write(stream);
          ++count;
        }
      }
      adata.stream.write(stream,this.factor);
    }

    public override void raw_read(adata.zero_copy_buffer stream)
    {
      adata.stream.read(stream,ref this.id);
      {
        Int32 len3 = adata.stream.check_read_size(stream,30);
        adata.stream.read(stream,ref this.name,len3);
      }
      adata.stream.read(stream,ref this.age);
      this.pos.raw_read(stream);
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          element.raw_read(stream);
          this.inventory.Add(element);
        }
      }
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          element.raw_read(stream);
          this.quests.Add(element);
        }
      }
      adata.stream.read(stream,ref this.factor);
    }

    public override Int32 raw_size_of()
    {
      Int32 size = 0;
      size += adata.stream.size_of(this.id);
      size += adata.stream.size_of(this.name);
      size += adata.stream.size_of(this.age);
      size += this.pos.raw_size_of();
      size += adata.stream.size_of(this.inventory.Count); 
      foreach (item i in this.inventory)
      {
        size += i.raw_size_of();
      }
      size += adata.stream.size_of(this.quests.Count); 
      foreach (my.game.quest i in this.quests)
      {
        size += i.raw_size_of();
      }
      size += adata.stream.size_of(this.factor);
      return size;
    }

    public override void raw_write(adata.zero_copy_buffer stream)
    {
      adata.stream.write(stream,this.id);
      {
        adata.stream.write(stream,this.name);
      }
      adata.stream.write(stream,this.age);
      this.pos.raw_write(stream);
      {
        Int32 len3 = this.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in this.inventory)
        {
          i.raw_write(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in this.quests)
        {
          i.raw_write(stream);
          ++count;
        }
      }
      adata.stream.write(stream,this.factor);
    }

  }

  public partial class player_v2
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
        Int32 len3 = adata.stream.check_read_size(stream,30);
        adata.stream.read(stream,ref this.name,len3);
      }
      if((tag&4L)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);}
      if((tag&8L)>0)      this.pos.read(stream);
      if((tag&16L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          element.read(stream);
          this.inventory.Add(element);
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          element.read(stream);
          this.quests.Add(element);
        }
      }
      if((tag&64L)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);}
      if((tag&128L)>0)      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.friends.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Int32 element= 0; 
          adata.stream.read(stream,ref element);
          this.friends.Add(element);
        }
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
      Int64 tag = 9L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      if(this.friends.Count > 0){tag|=128L;}
      size += adata.stream.size_of(this.id);
      if((tag&2L)>0)
      {
        size += adata.stream.size_of(this.name);
      }
      //this.age deleted , skip write.
      size += this.pos.size_of();
      if((tag&16L)>0)
      {
        Int32 len4 = this.inventory.Count;
        size += adata.stream.size_of(len4);
        foreach (item i in this.inventory)
        {
          size += i.size_of();
        }
      }
      if((tag&32L)>0)
      {
        Int32 len4 = this.quests.Count;
        size += adata.stream.size_of(len4);
        foreach (my.game.quest i in this.quests)
        {
          size += i.size_of();
        }
      }
      //this.factor deleted , skip write.
      if((tag&128L)>0)
      {
        Int32 len4 = this.friends.Count;
        size += adata.stream.size_of(len4);
        foreach (Int32 i in this.friends)
        {
          size += adata.stream.size_of(i);
        }
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public override void write(adata.zero_copy_buffer stream)
    {
      Int64 tag = 9L;
      if(this.name.Length > 0){tag|=2L;}
      if(this.inventory.Count > 0){tag|=16L;}
      if(this.quests.Count > 0){tag|=32L;}
      if(this.friends.Count > 0){tag|=128L;}
      adata.stream.write(stream,tag);
      adata.stream.write(stream,this.size_of());
      adata.stream.write(stream,this.id);
      if((tag&2L)>0)      {
        adata.stream.write(stream,this.name);
      }
    //this.age deleted , skip write.
      this.pos.write(stream);
      if((tag&16L)>0)      {
        Int32 len3 = this.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in this.inventory)
        {
          i.write(stream);
          ++count;
        }
      }
      if((tag&32L)>0)      {
        Int32 len3 = this.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in this.quests)
        {
          i.write(stream);
          ++count;
        }
      }
    //this.factor deleted , skip write.
      if((tag&128L)>0)      {
        Int32 len3 = this.friends.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (Int32 i in this.friends)
        {
          adata.stream.write(stream,i);
          ++count;
        }
      }
    }

    public override void raw_read(adata.zero_copy_buffer stream)
    {
      adata.stream.read(stream,ref this.id);
      {
        Int32 len3 = adata.stream.check_read_size(stream,30);
        adata.stream.read(stream,ref this.name,len3);
      }
      this.pos.raw_read(stream);
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.inventory.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          element.raw_read(stream);
          this.inventory.Add(element);
        }
      }
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.quests.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          element.raw_read(stream);
          this.quests.Add(element);
        }
      }
      {
        Int32 len3 = adata.stream.check_read_size(stream);
        this.friends.Clear();
        for (int i = 0 ; i < len3 ; ++i)
        {
          Int32 element= 0; 
          adata.stream.read(stream,ref element);
          this.friends.Add(element);
        }
      }
    }

    public override Int32 raw_size_of()
    {
      Int32 size = 0;
      size += adata.stream.size_of(this.id);
      size += adata.stream.size_of(this.name);
      size += this.pos.raw_size_of();
      size += adata.stream.size_of(this.inventory.Count); 
      foreach (item i in this.inventory)
      {
        size += i.raw_size_of();
      }
      size += adata.stream.size_of(this.quests.Count); 
      foreach (my.game.quest i in this.quests)
      {
        size += i.raw_size_of();
      }
      size += adata.stream.size_of(this.friends.Count); 
      foreach (Int32 i in this.friends)
      {
        size += adata.stream.size_of(i);
      }
      return size;
    }

    public override void raw_write(adata.zero_copy_buffer stream)
    {
      adata.stream.write(stream,this.id);
      {
        adata.stream.write(stream,this.name);
      }
      this.pos.raw_write(stream);
      {
        Int32 len3 = this.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in this.inventory)
        {
          i.raw_write(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in this.quests)
        {
          i.raw_write(stream);
          ++count;
        }
      }
      {
        Int32 len3 = this.friends.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (Int32 i in this.friends)
        {
          adata.stream.write(stream,i);
          ++count;
        }
      }
    }

  }

}}

