#ifndef vec3_adl_h_adata_header_define
#define vec3_adl_h_adata_header_define

#include <adata.hpp>

namespace util {
  struct vec3
  {
    float x;
    float y;
    float z;
    vec3()
    :    x(0.0f),
    y(0.0f),
    z(0.0f)
    {}
  };

}

namespace adata
{
  template<typename stream_ty>
  ADATA_INLINE void read( stream_ty& stream, ::util::vec3& value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {read(stream,value.x);{if(stream.error()){stream.trace_error("x",-1);return;}}}
    if(tag&2ULL)    {read(stream,value.y);{if(stream.error()){stream.trace_error("y",-1);return;}}}
    if(tag&4ULL)    {read(stream,value.z);{if(stream.error()){stream.trace_error("z",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  template <typename stream_ty>
  ADATA_INLINE void skip_read(stream_ty& stream, ::util::vec3* value)
  {
    ::std::size_t offset = stream.read_length();
    uint64_t tag = 0;
    read(stream,tag);
    if(stream.error()){return;}
    int32_t len_tag = 0;
    read(stream,len_tag);
    if(stream.error()){return;}

    if(tag&1ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("x",-1);return;}}}
    if(tag&2ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("y",-1);return;}}}
    if(tag&4ULL)    {float* dummy_value = 0;skip_read(stream,dummy_value);{if(stream.error()){stream.trace_error("z",-1);return;}}}
    if(len_tag >= 0)
    {
      ::std::size_t read_len = stream.read_length() - offset;
      ::std::size_t len = (::std::size_t)len_tag;
      if(len > read_len) stream.skip_read(len - read_len);
    }
  }

  ADATA_INLINE int32_t size_of(const ::util::vec3& value)
  {
    int32_t size = 0;
    uint64_t tag = 7ULL;
    {
      size += size_of(value.x);
    }
    {
      size += size_of(value.y);
    }
    {
      size += size_of(value.z);
    }
    size += size_of(tag);
    size += size_of(size + size_of(size));
    return size;
  }

  template<typename stream_ty>
  ADATA_INLINE void write(stream_ty& stream , const ::util::vec3&value)
  {
    uint64_t tag = 7ULL;
    write(stream,tag);
    if(stream.error()){return;}
    write(stream,size_of(value));
    if(stream.error()){return;}
    {write(stream,value.x);{if(stream.error()){stream.trace_error("x",-1);return;}}}
    {write(stream,value.y);{if(stream.error()){stream.trace_error("y",-1);return;}}}
    {write(stream,value.z);{if(stream.error()){stream.trace_error("z",-1);return;}}}
    return;
  }

}

#endif
