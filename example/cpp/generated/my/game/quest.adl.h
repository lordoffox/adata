#ifndef quest_adl_h_adata_header_define
#define quest_adl_h_adata_header_define

#include <adata.hpp>

namespace my {namespace game {
  struct quest
  {
    int32_t id;
    ::std::string name;
    ::std::string description;
    quest()
    :    id(0)
    {}
  };

}}

namespace adata
{
  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::my::game::quest& value)
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
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
      value.name.resize(len);
      stream.read((char *)value.name.data(),len);
      {if(stream.error()){stream.trace_error("name",-1);return;}}
    }
    if(tag&4ULL)
    {
      uint32_t len = check_read_size(stream);
      {if(stream.error()){stream.trace_error("description",-1);return;}}
      value.description.resize(len);
      stream.read((char *)value.description.data(),len);
      {if(stream.error()){stream.trace_error("description",-1);return;}}
    }
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::my::game::quest* value)
  {
    (value);
    skip_read_compatible(stream);
  }

  ADATA_INLINE int32_t size_of(const ::my::game::quest& value)
  {
    int32_t size = 0;
    uint64_t tag = 1ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.description.empty()){tag|=4ULL;}
    {
      size += size_of(value.id);
    }
    if(tag&2ULL)
    {
      int32_t len = (int32_t)(value.name).size();
      size += size_of(len);
      size += len;
    }
    if(tag&4ULL)
    {
      int32_t len = (int32_t)(value.description).size();
      size += size_of(len);
      size += len;
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::my::game::quest&value)
  {
    uint64_t tag = 1ULL;
    if(!value.name.empty()){tag|=2ULL;}
    if(!value.description.empty()){tag|=4ULL;}
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
    if(tag&4ULL)
    {
      uint32_t len = (uint32_t)(value.description).size();
      write(stream,len);
      stream.write((value.description).data(),len);
      {if(stream.error()){stream.trace_error("description",-1);return;}}
    }
    return;
  }

}

#endif
