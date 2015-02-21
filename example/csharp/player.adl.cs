using System;
using System.IO;
using System.Collections.Generic;

namespace my {namespace game {
  public class item
  {
    public Int32 type = 0;
    public Int32 level = 0;
    public Int64 id = 0;
  }

  public class player_v1
  {
    public Int32 id = 0;
    public Int32 age = 0;
    public float factor = 1.0F;
    public string name = "";
    public util.vec3 pos = new util.vec3();
    public List<item> inventory = new List<item>();
    public List<my.game.quest> quests = new List<my.game.quest>();
  }

  public class player_v2
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

class player_stream
{
    public static void read(adata.zero_copy_buffer stream, ref item value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {adata.stream.read(stream,ref value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      if((tag&2)>0)      {adata.stream.read(stream,ref value.type);{if(stream.error()){stream.trace_error("type",-1);return;}}}
      if((tag&4)>0)      {adata.stream.read(stream,ref value.level);{if(stream.error()){stream.trace_error("level",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static void skip_read(adata.zero_copy_buffer stream, ref item value)
    {
      int offset = stream.read_length();
      UInt64 tag = 0;
      adata.stream.read(stream,ref tag);
      if(stream.error()){return;}
      Int32 len_tag = 0;
      adata.stream.read(stream,ref len_tag);
      if(stream.error()){return;}

      if((tag&1)>0)      {Int64 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      if((tag&2)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("type",-1);return;}}}
      if((tag&4)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("level",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static Int32 size_of(item value)
    {
      Int32 size = 0;
      UInt64 tag = 7;
      {
        size += adata.stream.size_of(value.id);
      }
      {
        size += adata.stream.size_of(value.type);
      }
      {
        size += adata.stream.size_of(value.level);
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public static void write(adata.zero_copy_buffer stream , item value)
    {
      UInt64 tag = 7;
      adata.stream.write(stream,tag);
      if(stream.error()){ return; }
      adata.stream.write(stream,size_of(value));
      if(stream.error()){return;}
      {adata.stream.write(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
      {adata.stream.write(stream,value.type);{if(stream.error()){stream.trace_error("type",-1);return;}}}
      {adata.stream.write(stream,value.level);{if(stream.error()){stream.trace_error("level",-1);return;}}}
      return;
    }

    public static void read(adata.zero_copy_buffer stream, ref player_v1 value)
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
        UInt32 len3 = adata.stream.check_read_size(stream,30);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        adata.stream.read(stream,ref value.name,len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)      {adata.stream.read(stream,ref value.age);{if(stream.error()){stream.trace_error("age",-1);return;}}}
      if((tag&8)>0)      {util.vec3_stream.read(stream,ref value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("inventory",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          {read(stream,ref element);}
          value.inventory.Add(element);
          {if(stream.error()){stream.trace_error("inventory",(int)i);return;}}
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("quests",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          {my.game.quest_stream.read(stream,ref element);}
          value.quests.Add(element);
          {if(stream.error()){stream.trace_error("quests",(int)i);return;}}
        }
      }
      if((tag&64)>0)      {adata.stream.read(stream,ref value.factor);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static void skip_read(adata.zero_copy_buffer stream, ref player_v1 value)
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
        UInt32 len3 = adata.stream.check_read_size(stream,30);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        stream.skip_read(len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
      if((tag&8)>0)      {util.vec3 dummy_value = null;util.vec3_stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("inventory",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          {item dummy_value = null;skip_read(stream,ref dummy_value);}
          {if(stream.error()){stream.trace_error("inventory",(int)i);return;}}
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("quests",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          {my.game.quest dummy_value = null;my.game.quest_stream.skip_read(stream,ref dummy_value);}
          {if(stream.error()){stream.trace_error("quests",(int)i);return;}}
        }
      }
      if((tag&64)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static Int32 size_of(player_v1 value)
    {
      Int32 size = 0;
      UInt64 tag = 77;
      if(value.name.Length > 0){tag|=2;}
      if(value.inventory.Count > 0){tag|=16;}
      if(value.quests.Count > 0){tag|=32;}
      {
        size += adata.stream.size_of(value.id);
      }
      if((tag&2)>0)
      {
        UInt32 len4 = (UInt32)value.name.Length;
        size += adata.stream.size_of(len4);
        size += (Int32)len4;
      }
      {
        size += adata.stream.size_of(value.age);
      }
      {
        size += util.vec3_stream.size_of(value.pos);
      }
      if((tag&16)>0)
      {
        UInt32 len4 = (UInt32)value.inventory.Count;
        size += adata.stream.size_of(len4);
        foreach (item i in value.inventory)
        {
          size += size_of(i);
        }
      }
      if((tag&32)>0)
      {
        UInt32 len4 = (UInt32)value.quests.Count;
        size += adata.stream.size_of(len4);
        foreach (my.game.quest i in value.quests)
        {
          size += my.game.quest_stream.size_of(i);
        }
      }
      {
        size += adata.stream.size_of(value.factor);
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public static void write(adata.zero_copy_buffer stream , player_v1 value)
    {
      UInt64 tag = 77;
      if(value.name.Length > 0){tag|=2;}
      if(value.inventory.Count > 0){tag|=16;}
      if(value.quests.Count > 0){tag|=32;}
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
      {adata.stream.write(stream,value.age);{if(stream.error()){stream.trace_error("age",-1);return;}}}
      {util.vec3_stream.write(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = (UInt32)value.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in value.inventory)
        {
          {write(stream,i);}
          {if(stream.error()){stream.trace_error("inventory",count);return;}}
          ++count;
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = (UInt32)value.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in value.quests)
        {
          {my.game.quest_stream.write(stream,i);}
          {if(stream.error()){stream.trace_error("quests",count);return;}}
          ++count;
        }
      }
      {adata.stream.write(stream,value.factor);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
      return;
    }

    public static void read(adata.zero_copy_buffer stream, ref player_v2 value)
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
        UInt32 len3 = adata.stream.check_read_size(stream,30);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        adata.stream.read(stream,ref value.name,len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
      if((tag&8)>0)      {util.vec3_stream.read(stream,ref value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("inventory",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          item element= new item(); 
          {read(stream,ref element);}
          value.inventory.Add(element);
          {if(stream.error()){stream.trace_error("inventory",(int)i);return;}}
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("quests",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          my.game.quest element= new my.game.quest(); 
          {my.game.quest_stream.read(stream,ref element);}
          value.quests.Add(element);
          {if(stream.error()){stream.trace_error("quests",(int)i);return;}}
        }
      }
      if((tag&64)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
      if((tag&128)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("friends",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          Int32 element= 0; 
          {adata.stream.read(stream,ref element);}
          value.friends.Add(element);
          {if(stream.error()){stream.trace_error("friends",(int)i);return;}}
        }
      }
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static void skip_read(adata.zero_copy_buffer stream, ref player_v2 value)
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
        UInt32 len3 = adata.stream.check_read_size(stream,30);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
        stream.skip_read(len3);
        {if(stream.error()){stream.trace_error("name",-1);return;}}
      }
      if((tag&4)>0)      {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
      if((tag&8)>0)      {util.vec3 dummy_value = null;util.vec3_stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("inventory",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          {item dummy_value = null;skip_read(stream,ref dummy_value);}
          {if(stream.error()){stream.trace_error("inventory",(int)i);return;}}
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("quests",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          {my.game.quest dummy_value = null;my.game.quest_stream.skip_read(stream,ref dummy_value);}
          {if(stream.error()){stream.trace_error("quests",(int)i);return;}}
        }
      }
      if((tag&64)>0)      {float dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
      if((tag&128)>0)
      {
        UInt32 len3 = adata.stream.check_read_size(stream);
        {if(stream.error()){stream.trace_error("friends",-1);return;}}
        for (int i = 0 ; i < len3 ; ++i)
        {
          {Int32 dummy_value = 0;adata.stream.skip_read(stream,ref dummy_value);}
          {if(stream.error()){stream.trace_error("friends",(int)i);return;}}
        }
      }
      if(len_tag >= 0)
      {
        UInt32 read_len = (UInt32)(stream.read_length() - offset);
        UInt32 len = (UInt32)len_tag;
        if(len > read_len) stream.skip_read(len - read_len);
      }
    }

    public static Int32 size_of(player_v2 value)
    {
      Int32 size = 0;
      UInt64 tag = 9;
      if(value.name.Length > 0){tag|=2;}
      if(value.inventory.Count > 0){tag|=16;}
      if(value.quests.Count > 0){tag|=32;}
      if(value.friends.Count > 0){tag|=128;}
      {
        size += adata.stream.size_of(value.id);
      }
      if((tag&2)>0)
      {
        UInt32 len4 = (UInt32)value.name.Length;
        size += adata.stream.size_of(len4);
        size += (Int32)len4;
      }
      //value.age deleted , skip write.
      {
        size += util.vec3_stream.size_of(value.pos);
      }
      if((tag&16)>0)
      {
        UInt32 len4 = (UInt32)value.inventory.Count;
        size += adata.stream.size_of(len4);
        foreach (item i in value.inventory)
        {
          size += size_of(i);
        }
      }
      if((tag&32)>0)
      {
        UInt32 len4 = (UInt32)value.quests.Count;
        size += adata.stream.size_of(len4);
        foreach (my.game.quest i in value.quests)
        {
          size += my.game.quest_stream.size_of(i);
        }
      }
      //value.factor deleted , skip write.
      if((tag&128)>0)
      {
        UInt32 len4 = (UInt32)value.friends.Count;
        size += adata.stream.size_of(len4);
        foreach (Int32 i in value.friends)
        {
          size += adata.stream.size_of(i);
        }
      }
      size += adata.stream.size_of(tag);
      size += adata.stream.size_of(size + adata.stream.size_of(size));
      return size;
    }

    public static void write(adata.zero_copy_buffer stream , player_v2 value)
    {
      UInt64 tag = 9;
      if(value.name.Length > 0){tag|=2;}
      if(value.inventory.Count > 0){tag|=16;}
      if(value.quests.Count > 0){tag|=32;}
      if(value.friends.Count > 0){tag|=128;}
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
    //value.age deleted , skip write.
      {util.vec3_stream.write(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
      if((tag&16)>0)
      {
        UInt32 len3 = (UInt32)value.inventory.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (item i in value.inventory)
        {
          {write(stream,i);}
          {if(stream.error()){stream.trace_error("inventory",count);return;}}
          ++count;
        }
      }
      if((tag&32)>0)
      {
        UInt32 len3 = (UInt32)value.quests.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (my.game.quest i in value.quests)
        {
          {my.game.quest_stream.write(stream,i);}
          {if(stream.error()){stream.trace_error("quests",count);return;}}
          ++count;
        }
      }
    //value.factor deleted , skip write.
      if((tag&128)>0)
      {
        UInt32 len3 = (UInt32)value.friends.Count;
        adata.stream.write(stream,len3);
        int count = 0;
        foreach (Int32 i in value.friends)
        {
          {adata.stream.write(stream,i);}
          {if(stream.error()){stream.trace_error("friends",count);return;}}
          ++count;
        }
      }
      return;
    }

  }


}}

