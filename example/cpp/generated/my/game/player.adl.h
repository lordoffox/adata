#ifndef player_adl_h_adata_header_define
#define player_adl_h_adata_header_define

#include <adata.hpp>
#include <my/game/quest.adl.h>
#include <util/vec3.adl.h>

namespace my {namespace game {
  struct item
  {
    int32_t type;
    int32_t level;
    int64_t id;
    item()
    :    id(0LL),
    type(0),
    level(0)
    {}
  };

  struct player_v1
  {
    int32_t id;
    int32_t age;
    float factor;
    ::std::string name;
    ::util::vec3 pos;
    ::std::vector<::my::game::item> inventory;
    ::std::vector<::my::game::quest> quests;
    player_v1()
    :    id(0),
    age(0),
    factor(1.0f)
    {}
  };

  struct player_v2
  {
    int32_t id;
    //age deleted , skip define.
    //factor deleted , skip define.
    ::std::string name;
    ::util::vec3 pos;
    ::std::vector<::my::game::item> inventory;
    ::std::vector<::my::game::quest> quests;
    ::std::vector<int32_t> friends;
    player_v2()
    :    id(0)
    {}
  };

}}

namespace adata
{
  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::my::game::item& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.type);{if(stream.error()){stream.trace_error("type",-1);return;}}}
    if(tag&4ULL)    {read(stream,value.level);{if(stream.error()){stream.trace_error("level",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::my::game::item* value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {int64_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("type",-1);return;}}}
    if(tag&4ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("level",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  ADATA_INLINE int32_t size_of(const ::my::game::item& value)
  {
    int32_t size = 0;
    uint64_t tag = 7ULL;
    {
      size += size_of(value.id);
    }
    {
      size += size_of(value.type);
    }
    {
      size += size_of(value.level);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::my::game::item&value)
  {
    uint64_t tag = 7ULL;
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    {write(stream,value.type);{if(stream.error()){stream.trace_error("type",-1);return;}}}
    {write(stream,value.level);{if(stream.error()){stream.trace_error("level",-1);return;}}}
    return;
  }

  template<typename stream_ty>
  void read( stream_ty& stream, ::my::game::player_v1& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = check_read_size(stream,30);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
      value.name.resize(len);
      stream.read((char *)value.name.data(),len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    if(tag&4ULL)    {read(stream,value.age);{if(stream.error()){stream.trace_error("age",-1);return;}}}
    if(tag&8ULL)    {read(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("inventory",-1);return;}}
      value.inventory.resize(len);
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {read(stream,value.inventory[i]);}
        {if(stream.error()){stream.trace_error("inventory",(int32_t)i);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("quests",-1);return;}}
      value.quests.resize(len);
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {read(stream,value.quests[i]);}
        {if(stream.error()){stream.trace_error("quests",(int32_t)i);return;}}
      }
    }
    if(tag&64ULL)    {read(stream,value.factor);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  void skip_read(stream_ty& stream, ::my::game::player_v1* value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = check_read_size(stream,30);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
      stream.skip_read(len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    if(tag&4ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
    if(tag&8ULL)    {::util::vec3* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("inventory",-1);return;}}
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {::my::game::item* dummy_value = 0;skip_read(stream,dummy_value);}
        {if(stream.error()){stream.trace_error("inventory",(int32_t)i);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("quests",-1);return;}}
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {::my::game::quest* dummy_value = 0;skip_read(stream,dummy_value);}
        {if(stream.error()){stream.trace_error("quests",(int32_t)i);return;}}
      }
    }
    if(tag&64ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  int32_t size_of(const ::my::game::player_v1& value)
  {
    int32_t size = 0;
    uint64_t tag = 77ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.inventory.empty()){tag|=16ULL;}
    if(!value.quests.empty()){tag|=32ULL;}
    {
      size += size_of(value.id);
    }
    if(tag&2ULL)
    {
      int32_t len = (int32_t)(value.name).size();
      size += size_of(len);
      size += len;
    }
    {
      size += size_of(value.age);
    }
    {
      size += size_of(value.pos);
    }
    if(tag&16ULL)
    {
      int32_t len = (int32_t)(value.inventory).size();
      size += size_of(len);
      for (::std::vector<::my::game::item>::const_iterator i = value.inventory.begin() ; i != value.inventory.end() ; ++i)
      {
        size += size_of(*i);
      }
    }
    if(tag&32ULL)
    {
      int32_t len = (int32_t)(value.quests).size();
      size += size_of(len);
      for (::std::vector<::my::game::quest>::const_iterator i = value.quests.begin() ; i != value.quests.end() ; ++i)
      {
        size += size_of(*i);
      }
    }
    {
      size += size_of(value.factor);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  void write(stream_ty& stream , const ::my::game::player_v1&value)
  {
    uint64_t tag = 77ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.inventory.empty()){tag|=16ULL;}
    if(!value.quests.empty()){tag|=32ULL;}
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = (uint32_t)(value.name).size();
      write(stream,len);
      stream.write((value.name).data(),len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    {write(stream,value.age);{if(stream.error()){stream.trace_error("age",-1);return;}}}
    {write(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = (uint32_t)(value.inventory).size();
      write(stream,len);
      int32_t count = 0;
      for (::std::vector<::my::game::item>::const_iterator i = value.inventory.begin() ; i != value.inventory.end() ; ++i, ++count)
      {
        {write(stream,*i);}
        {if(stream.error()){stream.trace_error("inventory",count);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = (uint32_t)(value.quests).size();
      write(stream,len);
      int32_t count = 0;
      for (::std::vector<::my::game::quest>::const_iterator i = value.quests.begin() ; i != value.quests.end() ; ++i, ++count)
      {
        {write(stream,*i);}
        {if(stream.error()){stream.trace_error("quests",count);return;}}
      }
    }
    {write(stream,value.factor);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
    return;
  }

  template<typename stream_ty>
  void read( stream_ty& stream, ::my::game::player_v2& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = check_read_size(stream,30);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
      value.name.resize(len);
      stream.read((char *)value.name.data(),len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    if(tag&4ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
    if(tag&8ULL)    {read(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("inventory",-1);return;}}
      value.inventory.resize(len);
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {read(stream,value.inventory[i]);}
        {if(stream.error()){stream.trace_error("inventory",(int32_t)i);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("quests",-1);return;}}
      value.quests.resize(len);
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {read(stream,value.quests[i]);}
        {if(stream.error()){stream.trace_error("quests",(int32_t)i);return;}}
      }
    }
    if(tag&64ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
    if(tag&128ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("friends",-1);return;}}
      value.friends.resize(len);
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {read(stream,value.friends[i]);}
        {if(stream.error()){stream.trace_error("friends",(int32_t)i);return;}}
      }
    }
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  void skip_read(stream_ty& stream, ::my::game::player_v2* value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = check_read_size(stream,30);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
      stream.skip_read(len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    if(tag&4ULL)    {int32_t* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("age",-1);return;}}}
    if(tag&8ULL)    {::util::vec3* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("inventory",-1);return;}}
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {::my::game::item* dummy_value = 0;skip_read(stream,dummy_value);}
        {if(stream.error()){stream.trace_error("inventory",(int32_t)i);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("quests",-1);return;}}
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {::my::game::quest* dummy_value = 0;skip_read(stream,dummy_value);}
        {if(stream.error()){stream.trace_error("quests",(int32_t)i);return;}}
      }
    }
    if(tag&64ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("factor",-1);return;}}}
    if(tag&128ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("friends",-1);return;}}
      for (std::size_t i = 0 ; i < len ; ++i)
      {
        {int32_t* dummy_value = 0;skip_read(stream,dummy_value);}
        {if(stream.error()){stream.trace_error("friends",(int32_t)i);return;}}
      }
    }
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  int32_t size_of(const ::my::game::player_v2& value)
  {
    int32_t size = 0;
    uint64_t tag = 9ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.inventory.empty()){tag|=16ULL;}
    if(!value.quests.empty()){tag|=32ULL;}
    if(!value.friends.empty()){tag|=128ULL;}
    {
      size += size_of(value.id);
    }
    if(tag&2ULL)
    {
      int32_t len = (int32_t)(value.name).size();
      size += size_of(len);
      size += len;
    }
    //value.age deleted , skip write.
    {
      size += size_of(value.pos);
    }
    if(tag&16ULL)
    {
      int32_t len = (int32_t)(value.inventory).size();
      size += size_of(len);
      for (::std::vector<::my::game::item>::const_iterator i = value.inventory.begin() ; i != value.inventory.end() ; ++i)
      {
        size += size_of(*i);
      }
    }
    if(tag&32ULL)
    {
      int32_t len = (int32_t)(value.quests).size();
      size += size_of(len);
      for (::std::vector<::my::game::quest>::const_iterator i = value.quests.begin() ; i != value.quests.end() ; ++i)
      {
        size += size_of(*i);
      }
    }
    //value.factor deleted , skip write.
    if(tag&128ULL)
    {
      int32_t len = (int32_t)(value.friends).size();
      size += size_of(len);
      for (::std::vector<int32_t>::const_iterator i = value.friends.begin() ; i != value.friends.end() ; ++i)
      {
        size += size_of(*i);
      }
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  void write(stream_ty& stream , const ::my::game::player_v2&value)
  {
    uint64_t tag = 9ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.inventory.empty()){tag|=16ULL;}
    if(!value.quests.empty()){tag|=32ULL;}
    if(!value.friends.empty()){tag|=128ULL;}
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.id);{if(stream.error()){stream.trace_error("id",-1);return;}}}
    if(tag&2ULL)
    {
      uint32_t len = (uint32_t)(value.name).size();
      write(stream,len);
      stream.write((value.name).data(),len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    //value.age deleted , skip write.
    {write(stream,value.pos);{if(stream.error()){stream.trace_error("pos",-1);return;}}}
    if(tag&16ULL)
    {
      uint32_t len = (uint32_t)(value.inventory).size();
      write(stream,len);
      int32_t count = 0;
      for (::std::vector<::my::game::item>::const_iterator i = value.inventory.begin() ; i != value.inventory.end() ; ++i, ++count)
      {
        {write(stream,*i);}
        {if(stream.error()){stream.trace_error("inventory",count);return;}}
      }
    }
    if(tag&32ULL)
    {
      uint32_t len = (uint32_t)(value.quests).size();
      write(stream,len);
      int32_t count = 0;
      for (::std::vector<::my::game::quest>::const_iterator i = value.quests.begin() ; i != value.quests.end() ; ++i, ++count)
      {
        {write(stream,*i);}
        {if(stream.error()){stream.trace_error("quests",count);return;}}
      }
    }
    //value.factor deleted , skip write.
    if(tag&128ULL)
    {
      uint32_t len = (uint32_t)(value.friends).size();
      write(stream,len);
      int32_t count = 0;
      for (::std::vector<int32_t>::const_iterator i = value.friends.begin() ; i != value.friends.end() ; ++i, ++count)
      {
        {write(stream,*i);}
        {if(stream.error()){stream.trace_error("friends",count);return;}}
      }
    }
    return;
  }

}

#endif
