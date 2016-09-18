var Long = require('long');
var File = require('File');
var FileReader = require('filereader');

(function (global) {
  "use strict";
  var const_tag_as_value = 0x7f;
  var const_tag_as_type = 0x80;
  var const_interger_byte_msak = 0x1f;
  var const_negative_bit_value = 0x20;
  var const_store_postive_integer_byte_mask = 0x80 - 2;

  var adata = function (size) {
    'use asm';
    if (size == null) {
      size = 65536;
    }
    if (size <= 0)
      size = 1;
    this.b = new Uint8Array(size);
    this.d = new DataView(this.b.buffer);
    this.r = 0;
    this.w = 0;
    this.wb = 0;
    this.l = size;
  };
  
  adata.prototype.set = function (buf, len) {
    'use asm';
    this.b = new Uint8Array(buf,0);
    this.l = len;
    this.r = 0;
    this.w = 0;
    this.wb = 0;
    this.d = new DataView(this.b.buffer);
  };
  
  adata.prototype.clear = function () {
    'use asm';
    this.r = 0;
    this.w = 0;
    this.wb = 0;
  };
  
  adata.prototype.skip_read = function (n) {
    if ((this.r + n) > this.l) { throw "stream_buffer_overflow"; }
    this.r += n;
  }

  adata.prototype.sk_rd_u8 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1) {
        throw "error_value_uint8";
      }
      if ((this.r + 1) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r++;
    }
  }

  adata.prototype.sk_rd_i8 = function () {
    'use asm';
    if ((this.r + 1)> this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 1) {
        throw "error_value_int8";
      }
      if ((this.r + 1) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r++;
    }
  }
  
  adata.prototype.sk_rd_u16 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2) {
        throw "error_value_uint16";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.sk_rd_i16 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 2) {
        throw "error_value_int16";
      }
      if (this.r + read_bytes > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.sk_rd_u32 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 4) {
        throw "error_value_uint32";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.sk_rd_i32 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if ((tag < const_tag_as_type)) { return; }
    else {
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 4) {
        throw "error_value_int32";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.sk_rd_u64 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 8) {
        throw "error_value_uint64";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.sk_rd_i64 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return; }
    else {
      var read_bytes = (value & const_interger_byte_msak) + 1;
      if (read_bytes > 8) {
        throw "error_value_int64";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.r += read_bytes;
    }
  }
  
  adata.prototype.rd_fu8 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    return this.b[this.r++];
  };
  
  adata.prototype.rd_fi8 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var v = this.b[this.r++];
    return (v > 127) ? v - 256 : v;
  };
  
  adata.prototype.rd_fu16 = function () {
    'use asm';
    if ((this.r + 2) > this.l) {
      throw "stream_buffer_overflow";
    }
    var v = this.b[this.r++];
    v += (this.b[this.r++] << 8);
    return v;
  };
  
  adata.prototype.rd_fi16 = function () {
    'use asm';
    if ((this.r + 2) > this.l) {
      throw "stream_buffer_overflow";
    }
    var v = this.b[this.r++];
    v += (this.b[this.r++] << 8);
    return (v > 32767) ? v - 65536 : v;
  };
  
  adata.prototype.rd_fu32 = function () {
    'use asm';
    if ((this.r + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    var v = this.b[this.r++];
    v += (this.b[this.r++] << 8);
    v += (this.b[this.r++] << 16);
    v += (this.b[this.r++] << 24);
    return v;
  };
  
  adata.prototype.rd_fi32 = function (s) {
    'use asm';
    if ((this.r + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    var v = this.b[this.r++];
    v += (this.b[this.r++] << 8);
    v += (this.b[this.r++] << 16);
    v += (this.b[this.r++] << 24);
    return (v > 2147483647) ? v - 4294967296 : v;
  };
  
  adata.prototype.rd_fu64 = function () {
    'use asm';
    if ((this.r + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.r;
    this.r += 8;
    return new Long(this.d.getInt32(p, true), this.d.getInt32(p + 4, true), true);
  };
  
  adata.prototype.rd_fi64 = function () {
    'use asm';
    if ((this.r + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.r;
    this.r += 8;
    return new Long(this.d.getInt32(p, true), this.d.getInt32(p + 4, true), false);
  };
  
  adata.prototype.rd_f32 = function () {
    'use asm';
    if ((this.r + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.r;
    this.r += 4;
    return this.d.getFloat32(p);
  };
  
  adata.prototype.rd_d64 = function () {
    'use asm';
    if ((this.r + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.r;
    this.r += 8;
    return this.d.getFloat64(p);
  };
  
  adata.prototype.rd_u8 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 1) {
        throw "error_value_uint8";
      }
      if ((this.r + 1) > this.l) {
        throw "stream_buffer_overflow";
      }
      return this.b[this.r++];
    }
  }
  
  adata.prototype.rd_i8 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 1) {
        throw "error_value_int8";
      }
      if ((this.r + 1) > this.l) {
        throw "stream_buffer_overflow";
      }
      if ((tag & const_negative_bit_value) > 0) { return -this.b[this.r++]; }
      return this.b[this.r++];
    }
  }
  
  adata.prototype.rd_u16 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 2) {
        throw "error_value_uint16";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      var v = this.b[this.r++];
      switch (read_bytes) {
        case 2:
          v += (this.b[this.r++] << 8);
      }
      return v;
    }
  }
  
  adata.prototype.rd_i16 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 2) {
        throw "error_value_int16";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      var v = this.b[this.r++];
      switch (read_bytes) {
        case 2:
          v += (this.b[this.r++] << 8);
      }
      if ((tag & const_negative_bit_value) > 0) { return -v; }
      return v;
    }
  }
  
  adata.prototype.rd_u32 = function () {
    'use asm';
    if (this.r + 1 > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 4) {
        throw "error_value_uint32";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      var v = this.b[this.r];
      switch (read_bytes) {
        case 4:
          v += (this.b[this.r + 3] << 24);
        case 3:
          v += (this.b[this.r + 2] << 16);
        case 2:
          v += (this.b[this.r + 1] << 8);
      }
      this.r += read_bytes;
      return v;
    }
  }
  
  adata.prototype.rd_i32 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 4) {
        throw "error_value_int32";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      var v = this.b[this.r];
      switch (read_bytes) {
        case 4:
          v += (this.b[this.r + 3] << 24);
        case 3:
          v += (this.b[this.r + 2] << 16);
        case 2:
          v += (this.b[this.r + 1] << 8);
      }
      this.r += read_bytes;
      if ((tag & const_negative_bit_value) > 0) { return -v; }
      return v;
    }
  }
  
  adata.prototype.rd_u64 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      if ((tag & const_negative_bit_value) > 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 8) {
        throw "error_value_uint64";
      }
      if (this.r + read_bytes > this.l) {
        throw "stream_buffer_overflow";
      }
      var h = 0;
      var v = this.b[this.r];
      switch (read_bytes) {
        case 8: h += (this.b[this.r + 7] << 24);
        case 7: h += (this.b[this.r + 6] << 16);
        case 6: h += (this.b[this.r + 5] << 8);
        case 5: h += (this.b[this.r + 4]);
        case 4: v += (this.b[this.r + 3] << 24);
        case 3: v += (this.b[this.r + 2] << 16);
        case 2: v += (this.b[this.r + 1] << 8);
      }
      this.r += read_bytes;
      if (h > 0) { return new Long(v, h, true); }
      return v;
    }
  }
  
  adata.prototype.rd_i64 = function () {
    'use asm';
    if ((this.r + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var tag = this.b[this.r++];
    if (tag < const_tag_as_type) { return tag; }
    else {
      var read_bytes = (tag & const_interger_byte_msak) + 1;
      if (read_bytes > 8) {
        throw "error_value_int64";
      }
      if ((this.r + read_bytes) > this.l) {
        throw "stream_buffer_overflow";
      }
      var v = this.b[this.r];
      var h = 0;
      switch (read_bytes) {
        case 8: h += (this.b[this.r + 7] << 24);
        case 7: h += (this.b[this.r + 6] << 16);
        case 6: h += (this.b[this.r + 5] << 8);
        case 5: h += (this.b[this.r + 4]);
        case 4: v += (this.b[this.r + 3] << 24);
        case 3: v += (this.b[this.r + 2] << 16);
        case 2: v += (this.b[this.r + 1] << 8);
      }
      this.r += read_bytes;
      if ((tag & const_negative_bit_value) > 0) { h = -h; }
      if (h !== 0) { return new Long(v, h, false); }
      return v;
    }
  }
  
  adata.prototype.wt_fu8 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') { }
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_uint8"; }
    if (v >= 0 && v <= 255) { this.b[this.w++] = v; }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fi8 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') { }
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_int8"; }
    if (v >= -127 && v <= 127) { this.b[this.w++] = v; }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fu16 = function (v) {
    'use asm';
    if ((this.w + 2) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') { }
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_uint16";}
    if (v >= 0 && v <= 65535) {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = v >> 8;
    }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fi16 = function (v) {
    'use asm';
    if ((this.w + 2) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') { }
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_int16"; }
    if (v >= -32767 && v <= 32767) {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = v >> 8;
    }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fu32 = function (v) {
    'use asm';
    if ((this.w + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') { }
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_uint32"; }
    if (v >= 0 && v <= 4294967295) {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = (v >> 8) & 0xff;
      this.b[this.w++] = (v >> 16) & 0xff;
      this.b[this.w++] = (v >> 24);
    }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fi32 = function (v) {
    'use asm';
    if ((this.w + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') {}
    else if (Long.isLong(v)) { v = v.toInt(); }
    else { throw "error_value_fix_int32"; }

    if (v >= -2147483647 && v <= 2147483647) {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = (v >> 8) & 0xff;
      this.b[this.w++] = (v >> 16) & 0xff;
      this.b[this.w++] = (v >> 24);
    }
    else { throw "value_too_large_to_integer_number"; }
  };
  
  adata.prototype.wt_fu64 = function (v) {
    'use asm';
    if ((this.w + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = (v >> 8) & 0xff;
      this.b[this.w++] = (v >> 16) & 0xff;
      this.b[this.w++] = (v >> 24);
    }
    else if (Long.isLong(v)) {
      var v32 = v.low();
      this.b[this.w++] = v32 & 0xff;
      this.b[this.w++] = (v32 >> 8) & 0xff;
      this.b[this.w++] = (v32 >> 16) & 0xff;
      this.b[this.w++] = (v32 >> 24);
      v32 = v.high();
      this.b[this.w++] = v32 & 0xff;
      this.b[this.w++] = (v32 >> 8) & 0xff;
      this.b[this.w++] = (v32 >> 16) & 0xff;
      this.b[this.w++] = (v32 >> 24);
    }
    else {
      if (typeof v !== 'number') {
        throw "error_value_fix_uint64";
      }
    }
  };
  
  adata.prototype.wt_fi64 = function (v) {
    'use asm';
    if ((this.w + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (typeof v === 'number') {
      this.b[this.w++] = v & 0xff;
      this.b[this.w++] = (v >> 8) & 0xff;
      this.b[this.w++] = (v >> 16) & 0xff;
      this.b[this.w++] = (v >> 24);
    }
    else if (Long.isLong(v)) {
      var v32 = v.low();
      this.b[this.w++] = v32 & 0xff;
      this.b[this.w++] = (v32 >> 8) & 0xff;
      this.b[this.w++] = (v32 >> 16) & 0xff;
      this.b[this.w++] = (v32 >> 24);
      v32 = v.high();
      this.b[this.w++] = v32 & 0xff;
      this.b[this.w++] = (v32 >> 8) & 0xff;
      this.b[this.w++] = (v32 >> 16) & 0xff;
      this.b[this.w++] = (v32 >> 24);
    }
    else {
      if (typeof v !== 'number') {
        throw "error_value_fix_int64";
      }
    }
  };
  
  adata.prototype.wt_f32 = function (v) {
    'use asm';
    if ((this.w + 4) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.w;
    this.w += 4;
    return this.d.setFloat32(p, v);
  };
  
  adata.prototype.wt_d64 = function (v) {
    'use asm';
    if ((this.w + 8) > this.l) {
      throw "stream_buffer_overflow";
    }
    var p = this.w;
    this.w += 8;
    return this.d.setFloat64(p, v);
  };
  
  adata.prototype.szof_u8 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        throw "error_value_int8";
      }
    }
    v = v & 0xff;
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { return 1; }
    else if (v < 0x100) { return 2; }
  };
  
  adata.prototype.szof_i8 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xff;
    if (0 <= v) { return 1; }
    else { return 2; }
  };
  
  adata.prototype.szof_u16 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xffff;
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { return 1; }
    else {
      if (v < 0x100) { return 2; }
      else { return 3; }
    }
  };
  
  adata.prototype.szof_i16 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xffff;
    if (0 <= v && v < const_tag_as_type) { return 1; }
    else {
      if (v < 0) { v = -v; }
      if (v < 0x100) { return 2; }
      else { return 3; }
    }
  };
  
  adata.prototype.szof_u32 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { return 1; }
    else {
      if (v < 0x100) { return 2; }
      else if (v < 0x10000) { return 3; }
      else if (v < 0x1000000) { return 4; }
      else { return 5; }
    }
  };
  
  adata.prototype.szof_i32 = function (v) {
    'use asm';
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xffff;
    if (0 <= v && v < const_tag_as_type) { return 1; }
    else {
      if (v < 0) { v = -v; }
      if (v < 0x100) { return 2; }
      else if (v < 0x10000) { return 3; }
      else if (v < 0x1000000) { return 4; }
      else { return 5; }
    }
  };
  
  var u2B = Long.fromString('100', true, 16);
  var u3B = Long.fromString('10000', true, 16);
  var u4B = Long.fromString('1000000', true, 16);
  var u5B = Long.fromString('100000000', true, 16);
  var u6B = Long.fromString('10000000000', true, 16);
  var u7B = Long.fromString('1000000000000', true, 16);
  var u8B = Long.fromString('100000000000000', true, 16);
  var u9B = Long.fromString('10000000000000000', true, 16);

  adata.prototype.szof_u64 = function (v) {
    'use asm';
    var h = 0;
    if (Long.isLong(v)) {
      h = v.high;
      if (h === 0) {
        v = v.low;
      }
    }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
      if (v < 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
    }
    if (h === 0 && v < const_tag_as_type) { return 1; }
    else {
      if (h === 0) {
        if (v < 0x100) { return 2; }
        else if (v < 0x10000) { return 3; }
        else if (v < 0x1000000) { return 4; }
        else { return 5; }
      }
      else {
        if (v.compare(u6B) < 0) { return 6; }
        else if (v.compare(u7B) < 0) { return 7; }
        else if (v.compare(u8B) < 0) { return 8; }
        else { return 9; }
      }
    }
  };
    
  var i2B = Long.fromString('100', false, 16);
  var i3B = Long.fromString('10000', false, 16);
  var i4B = Long.fromString('1000000', false, 16);
  var i5B = Long.fromString('100000000', false, 16);
  var i6B = Long.fromString('10000000000', false, 16);
  var i7B = Long.fromString('1000000000000', false, 16);
  var i8B = Long.fromString('100000000000000', false, 16);
  var i9B = Long.fromString('8000000000000000', false, 16);
    
  adata.prototype.szof_i64 = function (v) {
    'use asm';
    var h = 0;
    var nb = 0;
    if (Long.isLong(v)) {
      if (v.isNegative()) {
        nb = 1;
        v = v.multiply(-1);
      }

      h = v.high;
      if (h === 0) {
        v = v.low;
      }
      if (v < 0) { h = -h; nb = 1;}
    }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
      if (v < 0) { v = -v; }
    }
    if (nb === 0 && h === 0 && v < const_tag_as_type) {
      return 1;
    }
    if (h === 0) {
      if (v < 0x100) { return 2; }
      else if (v < 0x10000) { return 3; }
      else if (v < 0x1000000) { return 4; }
      else { return 5; }
    }
    else if (v.compare(i6B) < 0) { return 6; }
    else if (v.compare(i7B) < 0) { return 7; }
    else if (v.compare(i8B) < 0) { return 8; }
    else { return 9; }
  };
    
  adata.prototype.wt_u8 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xff;
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { this.b[this.w++] = v; }
    else if (v < 0x100) {
      if ((this.w + 2) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.b[this.w++] = 0x80;
      this.b[this.w++] = v;
    }
  };
  
  adata.prototype.wt_i8 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xff;
    if (0 <= v) { this.b[this.w++] = v; }
    else {
      if ((this.w + 2) > this.l) {
        throw "stream_buffer_overflow";
      }
      this.b[this.w++] = 0x80 | const_negative_bit_value;
      this.b[this.w++] = -v;
    }
  };
  
  adata.prototype.wt_u16 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xffff;
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { this.b[this.w++] = v; }
    else {
      if (v < 0x100) {
        if ((this.w + 2) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x80;
        this.b[this.w++] = v;
      }
      else {
        if ((this.w + 3) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x81;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
      }
    }
  };
  
  adata.prototype.wt_i16 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    v = v & 0xffff;
    if (0 <= v && v < const_tag_as_type) { this.b[this.w++] = v; }
    else {
      var nb = 0;
      if (v < 0) {
        nb = const_negative_bit_value;
        v = -v; 
      }
      if (v < 0x100) {
        if ((this.w + 2) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x80+nb;
        this.b[this.w++] = v;
      }
      else {
        if ((this.w + 3) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x81+nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
      }
    }
  };
  
  adata.prototype.wt_u32 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
    }
    if (v < 0) {
      throw "negative_assign_to_unsigned_integer_number";
    }
    if (v < const_tag_as_type) { this.b[this.w++] = v; }
    else {
      if (v < 0x100) {
        if ((this.w + 2) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x80;
        this.b[this.w++] = v;
      }
      else if ( v< 0x10000) {
        if ((this.w + 3) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x81;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
      }
      else if (v < 0x1000000) {
        if ((this.w + 4) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x82;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
      }
      else{
        if ((this.w + 5) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x83;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
      }
    }
  };
  
  adata.prototype.wt_i32 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    if (Long.isLong(v)) { v = v.toInt(); }
    v = v & 0xffff;
    if (0 <= v && v < const_tag_as_type) { this.b[this.w++] = v; }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }

      var nb = 0;
      if (v < 0) {
        nb = const_negative_bit_value;
        v = -v;
      }
      if (v < 0x100) {
        if ((this.w + 2) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x80 + nb;
        this.b[this.w++] = v;
      }
      else if (v < 0x10000) {
        if ((this.w + 3) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x81 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
      }
      else if (v < 0x1000000) {
        if ((this.w + 4) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x82 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
      }
      else {
        if ((this.w + 5) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x83 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
      }
    }
  };
  
  adata.prototype.wt_u64 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var h = 0;
    if (Long.isLong(v)) {
      if (v.isNegative()) {
        throw "negative_assign_to_unsigned_integer_number";
      }
      h = v.high;
      v = v.toInt();
    }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }

      if (v < 0) {
        throw "negative_assign_to_unsigned_integer_number";
      }
    }
    if (h === 0 && v < const_tag_as_type) { this.b[this.w++] = v; }
    else {
      if (h === 0) {
        if (v < 0x100) {
          if ((this.w + 2) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x80;
          this.b[this.w++] = v;
        }
        else if (v < 0x10000) {
          if ((this.w + 3) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x81;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
        }
        else if (v < 0x1000000) {
          if ((this.w + 4) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x82;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
          this.b[this.w++] = v >> 16;
        }
        else {
          if ((this.w + 5) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x83;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
          this.b[this.w++] = v >> 16;
          this.b[this.w++] = v >> 24;
        }
      }
      else if (h < 0x100) {
        if ((this.w + 6) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x84;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
      }
      else if (h < 0x10000) {
        if ((this.w + 7) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x85;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
      }
      else if (h < 0x1000000) {
        if ((this.w + 8) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x86;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
        this.b[this.w++] = h >> 16;
      }
      else {
        if ((this.w + 9) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x87;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
        this.b[this.w++] = h >> 16;
        this.b[this.w++] = h >> 24;
      }
    }
  };

  adata.prototype.wt_i64 = function (v) {
    'use asm';
    if ((this.w + 1) > this.l) {
      throw "stream_buffer_overflow";
    }
    var h = 0;
    var nb = 0;
    if (Long.isLong(v)) {
      if (v.isNegative()) {
        nb = const_negative_bit_value;
      }
      h = v.high;
      v = v.toInt();
      if (nb === const_negative_bit_value) {
        h = -h;
      }
    }
    else {
      if (typeof v !== 'number') {
        v = parseInt(v);
      }
      if (v < 0) {
        nb = const_negative_bit_value;
        v = -v;
      }
    }
    if ((nb === 0) && (h === 0) && (v < const_tag_as_type)) { this.b[this.w++] = v; }
    else {
      if (h === 0) {
        if (v < 0x100) {
          if ((this.w + 2) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x80 + nb;
          this.b[this.w++] = v;
        }
        else if (v < 0x10000) {
          if ((this.w + 3) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x81 + nb;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
        }
        else if (v < 0x1000000) {
          if ((this.w + 4) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x82 + nb;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
          this.b[this.w++] = v >> 16;
        }
        else {
          if ((this.w + 5) > this.l) {
            throw "stream_buffer_overflow";
          }
          this.b[this.w++] = 0x83 + nb;
          this.b[this.w++] = v & 0xff;
          this.b[this.w++] = v >> 8;
          this.b[this.w++] = v >> 16;
          this.b[this.w++] = v >> 24;
        }
      }
      else if (h < 0x100) {
        if ((this.w + 6) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x84 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
      }
      else if (h < 0x10000) {
        if ((this.w + 7) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x85 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
      }
      else if (h < 0x1000000) {
        if ((this.w + 8) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x86 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
        this.b[this.w++] = h >> 16;
      }
      else {
        if ((this.w + 9) > this.l) {
          throw "stream_buffer_overflow";
        }
        this.b[this.w++] = 0x87 + nb;
        this.b[this.w++] = v & 0xff;
        this.b[this.w++] = v >> 8;
        this.b[this.w++] = v >> 16;
        this.b[this.w++] = v >> 24;
        this.b[this.w++] = h & 0xff;
        this.b[this.w++] = h >> 8;
        this.b[this.w++] = h >> 16;
        this.b[this.w++] = h >> 24;
      }
    }
  };
  
  var cs = [], ds = [];
  var _tos = String.fromCharCode;
  var v;
  for (var i = 0; i < 256; ++i) {
    v = _tos(i);
    ds[v] = i; // "\00" -> 0x00
    cs[i] = v; // 0x00 -> "\00"
  }
  
  adata.prototype.sk_rd_s = function (n) {
    'use asm';
    var l = this.rd_i32();
    if ((n > 0) && (l > n)) {
      throw "number_of_element_not_macth";
    }
    if ((this.r + l) > this.l) {
      throw "stream_buffer_overflow";
    }
    this.r += l;
  };
  
  adata.prototype.rd_s = function (n) {
    'use asm';
    var l = this.rd_i32();
    if ((n > 0) && (l > n)) {
      throw "number_of_element_not_macth";
    }
    if ((this.r + l) > this.l) {
      throw "stream_buffer_overflow";
    }
    var s = [];
    var c = cs;
    for (var i = 0; i < l; ++i) {
      s.push(c[this.b[this.r++]]);
    }
    return s.join("");
  };
  
  adata.prototype.szof_s = function (s, n) {
    'use asm';
    if (typeof s !== 'string') {
      throw "error_value_string";
    }
    var l = s.length;
    l += this.szof_i32(l);
    return l;
  };
  
  adata.prototype.wt_s = function (s, n) {
    'use asm';
    if (typeof s !== 'string') {
      throw "error_value_string";
    }
    var l = s.length;
    if ((n > 0) && (l > n)) {
      throw "number_of_element_not_macth";
    }
    this.wt_i32(l);
    if ((this.w + l) > this.l) {
      throw "stream_buffer_overflow";
    }
    for (var i = 0; i < l; ++i) {
      this.b[this.w++] = ds[s[i]];
    }
  };
  
  var adata_et_unknow = 0;
  var adata_et_fix_int8 = 1;
  var adata_et_fix_uint8 = 2;
  var adata_et_fix_int16 = 3;
  var adata_et_fix_uint16 = 4;
  var adata_et_fix_int32 = 5;
  var adata_et_fix_uint32 = 6;
  var adata_et_fix_int64 = 7;
  var adata_et_fix_uint64 = 8;
  var adata_et_int8 = 9;
  var adata_et_uint8 = 10;
  var adata_et_int16 = 11;
  var adata_et_uint16 = 12;
  var adata_et_int32 = 13;
  var adata_et_uint32 = 14;
  var adata_et_int64 = 15;
  var adata_et_uint64 = 16;
  var adata_et_float32 = 17;
  var adata_et_float64 = 18;
  var adata_et_string = 19;
  var adata_et_list = 20;
  var adata_et_map = 21;
  var adata_et_type = 22;

  adata.prototype.sk_rd_v = function (type) {
    'use asm';
    switch (type.type) {
      case 1: { return this.skip_read(1); }//fix_int8
      case 2: { return this.skip_read(1); }//fix_uint8
      case 3: { return this.skip_read(2); }//fix_int16
      case 4: { return this.skip_read(2); }//fix_uint16
      case 5: { return this.skip_read(4); }//fix_int32
      case 6: { return this.skip_read(4); }//fix_uint32
      case 7: { return this.skip_read(8); }//fix_int64
      case 8: { return this.skip_read(8); }//fix_uint64
      case 9: { return this.sk_rd_i8(); }//int8
      case 10: { return this.sk_rd_u8(); }//uint8
      case 11: { return this.sk_rd_i16(); }//int16
      case 12: { return this.sk_rd_u16(); }//uint16
      case 13: { return this.sk_rd_i32(); }//int32
      case 14: { return this.sk_rd_u32(); }//uint32
      case 15: { return this.sk_rd_i64(); }//int64
      case 16: { return this.sk_rd_u64(); }//uint64
      case 17: { return this.skip_read(4); }//float
      case 18: { return this.skip_read(8); }//double
      case 19: { return this.sk_rd_s(type.size); }//string
      case 22: { return this.sk_rd(type.type_def); }//type
    }
  };
  
  adata.prototype.sk_rd_m = function (mb , n) {
    'use asm';
    switch (mb.type) {
      case 20: {
        var l = this.rd_i32();
        if ((n > 0) && (l > n)) {
          throw "number_of_element_not_macth";
        }
        var pdef = mb.params[0];
        for (var i = 0; i < l; ++i) {
          this.sk_rd_v(pdef);
        }
        return a;
      }//list
      case 21: {
        var l = this.rd_i32();
        if ((n > 0) && (l > n)) {
          throw "number_of_element_not_macth";
        }
        var pdef1 = mb.params[0];
        var pdef2 = mb.params[1];
        for (var i = 0; i < l; ++i) {
          this.sk_rd_v(pdef1);
          this.sk_rd_v(pdef2);
        }
        return m;
      }//map
    }
    this.sk_rd_v(type);
  };
  
  adata.prototype.sk_rd = function (type) {
    'use asm';
    var offset = this.r;
    var read_tag = this.rd_i64();
    var len_tag = this.rd_i32();
    if (len_tag > 0) {
      var read_len = this.r;
      if (len_tag > read_len) {
        this.skip_read(len_tag - read_len);
      }
    }
  };
  
  adata.prototype.rd_v = function (type , v) {
    'use asm';
    switch (type.type) {
      case 1: { return this.rd_fi8(); }//fix_int8
      case 2: { return this.rd_fu8(); }//fix_uint8
      case 3: { return this.rd_fi16(); }//fix_int16
      case 4: { return this.rd_fu16(); }//fix_uint16
      case 5: { return this.rd_fi32(); }//fix_int32
      case 6: { return this.rd_fu32(); }//fix_uint32
      case 7: { return this.rd_fi64(); }//fix_int64
      case 8: { return this.rd_fu64(); }//fix_uint64
      case 9: { return this.rd_i8(); }//int8
      case 10: { return this.rd_u8(); }//uint8
      case 11: { return this.rd_i16(); }//int16
      case 12: { return this.rd_u16(); }//uint16
      case 13: { return this.rd_i32(); }//int32
      case 14: { return this.rd_u32(); }//uint32
      case 15: { return this.rd_i64(); }//int64
      case 16: { return this.rd_u64(); }//uint64
      case 17: { return this.rd_f32(); }//float
      case 18: { return this.rd_d64(); }//double
      case 19: { return this.rd_s(type.size); }//string
      case 22: { return this.rd(type.type_def , v); }//type
    }
  };
    
  adata.prototype.rd_m = function (mb,mv) {
    'use asm';
    switch (mb.type) {
      case 20: {
        var l = this.rd_i32();
        var n = mb.size;
        if ((n > 0) && (l > n)) {
          throw "number_of_element_not_macth";
        }
        var pdef = mb.params[0];
        for (var i = 0; i < l; ++i) {
          var v = construct_value(pdef);
          v = this.rd_v(pdef);
          mv.push(v);
        }
        return mv;
      }//list
      case 21: {
        var l = this.rd_i32();
        var n = mb.size;
        if ((n > 0) && (l > n)) {
          throw "number_of_element_not_macth";
        }
        var pdef1 = mb.params[0];
        var pdef2 = mb.params[1];
        for (var i = 0; i < l; ++i) {
          var k = construct_value(pdef1);
          var v = construct_value(pdef2);
          k = this.rd_v(pdef1,k);
          v = this.rd_v(pdef2,v);
          mv[k]=v;
        }
        return mv;
      }//map
    }
    var v = this.rd_v(mb,mv);
    return v;
  };

  adata.prototype.rd = function (type ,v) {
    'use asm';
    var offset = this.r;
    var read_tag = this.rd_i64();
    var len_tag = this.rd_i32();

    var l = read_tag;
    var h = 0;
    if (Long.isLong(read_tag)) {
      l = read_tag.low;
      h = read_tag.high;
    }
    var c = 0 , mk = 1;
    var sk = 0;
    for (var i = 0; i < type.members.length; ++i) {
      sk = 0;
      var m = type.members[i];
      var name = m.name;
      if ((l & mk) === 0) {
        sk = 1;
      }
      ++c;
      mk = mk << 1;
      if (c === 32) {
        l = h;
        mk = 1;
      }
      if ((sk === 1) && (m.del === 1)) {
        this.sk_rd_m(m);
      }
      else {
        v[name] = this.rd_m(m,v[name]);  
      }
    }
    if (len_tag > 0) {
      var read_len = this.r;
      if (len_tag > read_len) {
        this.skip_read(len_tag - read_len);
      }
    }
    return v;
  };

  adata.prototype.szof_v = function (v , type , ctx) {
    'use asm';
    switch (type.type) {
      case 1: { return 1; }//fix_int8
      case 2: { return 1; }//fix_uint8
      case 3: { return 2; }//fix_int16
      case 4: { return 2; }//fix_uint16
      case 5: { return 4; }//fix_int32
      case 6: { return 4; }//fix_uint32
      case 7: { return 8; }//fix_int64
      case 8: { return 8; }//fix_uint64
      case 9: { return this.szof_i8(v); }//int8
      case 10: { return this.szof_u8(v); }//uint8
      case 11: { return this.szof_i16(v); }//int16
      case 12: { return this.szof_u16(v); }//uint16
      case 13: { return this.szof_i32(v); }//int32
      case 14: { return this.szof_u32(v); }//uint32
      case 15: { return this.szof_i64(v); }//int64
      case 16: { return this.szof_u64(v); }//uint64
      case 17: { return 4; }//float
      case 18: { return 8; }//double
      case 19: { return this.szof_s(v,type.size); }//string
      case 22: { return this.szof(type.type_def , v , ctx); }//type
    }
  };
  
  var _isArray = Array.isArray || (function (mix) {
    return Object.prototype.toString.call(mix) === "[object Array]";
  });
  
  adata.prototype.szof_m = function (v, mb , ctx) {
    'use asm';
    switch (mb.type) {
      case 20: {
        var l = 0;
        var sz = 0;
        if (_isArray(v) === true) {
          l = v.length;
          sz += this.szof_i32(l);
          var pdef = mb.params[0];
          for (var i = 0; i < l; ++i) {
            sz += this.szof_v(v[i], pdef , ctx);
          }
        }
        return sz;
      }//list
      case 21: {
        var l = 0;
        var sz = 0;
        if (v.keys !== null) {
          var keys = Object.keys(v);
          l = keys.length;
          sz += this.szof_i32(l);
          var pdef1 = mb.params[0];
          var pdef2 = mb.params[1];
          for (var k in v) {
            sz += this.szof_v(k, pdef1 , ctx);
            sz += this.szof_v(v[k], pdef2 , ctx);
          }
        }
        return sz;
      }//map
    }
    return this.szof_v(v, mb , ctx);
  };
  
  adata.prototype.szof = function (type, v , ctx) {
    'use asm';
    var c = 0;    
    var mk = 1;
    var sz = 0;
    var h = 0;
    var l = 0;
    var info = {};

    if (ctx != null) {
      ctx.push(info);
    }
    
    for (var i = 0; i < type.members.length; ++i) {
      var m = type.members[i];
      if (m.del === 0) {
        var name = m.name;
        var msz = this.szof_m(v[name], m , ctx);
        if (msz > 0) {
          sz += msz;
          if (c < 32) {
            l = l | mk;
          }
          else {
            h = h | mk;
          }
        }
      }
      mk = mk << 1;
      ++c;
      if (c === 32) {
        mk = 1;
      }
    }
    var tag = new Long(l, h);
    sz += this.szof_i64(tag);
    sz += this.szof_i32(sz);
    info.tag = tag;
    info.sz = sz;
    return sz;
  };
  
  adata.prototype.wt_v = function (v, type , ctx) {
    'use asm';
    switch (type.type) {
      case 1: { return this.wt_fi8(v); }//fix_int8
      case 2: { return this.wt_fu8(v); }//fix_uint8
      case 3: { return this.wt_fi16(v); }//fix_int16
      case 4: { return this.wt_fu16(v); }//fix_uint16
      case 5: { return this.wt_fi32(v); }//fix_int32
      case 6: { return this.wt_fu32(v); }//fix_uint32
      case 7: { return this.wt_fi64(v); }//fix_int64
      case 8: { return this.wt_fu64(v); }//fix_uint64
      case 9: { return this.wt_i8(v); }//int8
      case 10: { return this.wt_u8(v); }//uint8
      case 11: { return this.wt_i16(v); }//int16
      case 12: { return this.wt_u16(v); }//uint16
      case 13: { return this.wt_i32(v); }//int32
      case 14: { return this.wt_u32(v); }//uint32
      case 15: { return this.wt_i64(v); }//int64
      case 16: { return this.wt_u64(v); }//uint64
      case 17: { return this.wt_f32(v); }//float
      case 18: { return this.wt_d64(v); }//double
      case 19: { return this.wt_s(v,type.size); }//string
      case 22: { return this.wt(type.type_def,v , ctx); }//type
    }
  };
  
  adata.prototype.wt_m = function (v, mb , ctx) {
    'use asm';
    switch (mb.type) {
      case 20: {
        var l = v.length;
        var n = mb.size;
        if (n > 0 && l > n) {
          throw "number_of_element_not_macth";
        }
        this.wt_i32(l);
        var pdef = mb.params[0];
        for (var i = 0; i < l; ++i) {
          this.wt_v(v[i], pdef , ctx);
        }
        break;
      }//list
      case 21: {
        var l = Object.keys(v).length;
        var n = mb.size;
        if (n > 0 && l > n) {
          throw "number_of_element_not_macth";
        }
        this.wt_i32(l);
        var pdef1 = mb.params[0];
        var pdef2 = mb.params[1];
        for (var k in v) {
          this.wt_v(k, pdef1 , ctx);
          this.wt_v(v[k], pdef2 , ctx);
        }
        break;
      }//map
    }
    this.wt_v(v, mb , ctx);
  };
  
  adata.prototype.wt = function (type, v , ctx) {
    'use asm';
    var idx = ctx[0]+1;
    var info = ctx[idx];
    ctx[0] = idx;
    this.wt_i64(info.tag);
    this.wt_i32(info.sz);
    var l = info.tag.low;
    var h = info.tag.high;
    var mk = 1;
    for (var i = 0; i < type.members.length; ++i) {
      var m = type.members[i];
      var skip = true;
      if (i == 32) {
        mk = 1;
      }
      if (i < 32) {
        if (l & mk) {
          skip = false;
        }
      }
      else {
        if (h & mk) {
          skip = false;
        }
      }
      mk <<= 1;
      if (skip === false) {
        this.wt_m(v[m.name], m , ctx);
      }      
    }
  };
  
  adata.prototype.read = function (type , obj) {
    var v = this.rd(type , obj);
    return v;
  }

  adata.prototype.size_of = function (type, v) {
    'use ams';
    return this.szof(type, v);
  }
  
  adata.prototype.write = function (type, v) {
    'use ams';
    var ctx = [0];
    this.szof(type, v , ctx);
    this.wt(type, v,ctx);
  }
  
  var types = {};
  var constructors = {};
  
  adata.prototype.type = function (name) {
    return constructors[name];
  }

  var decode_default_value = function (buf, member) {
    var t = member.type;
    var v = null;
    if (t === adata_et_fix_uint64 || t === adata_et_uint64) {
      v = buf.rd_u64();
    }
    else if (t >= adata_et_fix_int8 && t <= adata_et_int64) {
      v = buf.rd_i64();
    }
    else if (t === adata_et_float32) {
      v = buf.rd_f32();
    }
    else if (t === adata_et_float64) {
      v = buf.rd_d64();
    }
    else if (t === adata_et_string) {
      v = "";
    }
    else if (t === adata_et_list) {
      v = [];
    }
    else if (t === adata_et_map) {
      v = {};
    }
    else if (t === adata_et_type) {
      v = constructors[member.typename];
    }
    return v;
  }
  
  var construct_value = function (type){
    if (type.type == adata_et_type) {
      return type.constructor();
    }
    return null;
  }

  var make_type = function (ns_name , type , field_list , constuct_list)
  {
    var new_type = function (){
      var obj = {};
      Object.defineProperty(obj, 'ad_type', {
        value : new_type,
        writable : false,
        enumerable : false,
        configurable : false
      });
      
      Object.defineProperty(obj, 'skip_read', {
        value : function (buf) {
          return buf.skip_rd(type);
        },
        writable : false,
        enumerable : false,
        configurable : false
      });
      
      Object.defineProperty(obj, 'size_of', {
        value : function (buf) {
          return buf.size_of(type, this);
        },
        writable : false,
        enumerable : false,
        configurable : false
      });
      
      Object.defineProperty(obj, 'read', {
        value : function (buf , throw_err) {
          return buf.read(type , this , throw_err);
        },
        writable : false,
        enumerable : false,
        configurable : false
      });
      
      Object.defineProperty(obj, 'write', {
        value : function (buf , throw_err) {
          return buf.write(type, this , throw_err);
        },
        writable : false,
        enumerable : false,
        configurable : false
      });
      
      for (var i = 0; i < field_list.length; ++i) {
        var f = field_list[i];
        var c = constuct_list[i];
        Object.defineProperty(obj, f, {
          value : null,
          writable : true,
          enumerable : true,
          configurable : false
        });
        if (c === null) {
          obj[f] = {};
        }
        else if (_isArray(c) === true) {
          obj[f] = [];
        }
        else if (typeof (c) === "object") {
          obj[f] = {};
        }
        else if (typeof (c) === "function") {
          obj[f] = c();
        }
        else{
          obj[f] = c;
        }
      }

      return obj;
    }
    
    var full_name = ns_name + '.' + type.name;
    constructors[full_name] = new_type;
    type.constructor = new_type();
  }

  var parser_namespace = function (buf) {
    var ns_name = buf.rd_s(0);
    var sp_count = buf.rd_i32();
    var str_idx = [];
    for (var i = 0; i < sp_count; ++i) {
      var s = buf.rd_s(0);
      str_idx.push(s);
    }
    var ns_types = [];
    var ty_count = buf.rd_i32();
    for (var i = 0; i < ty_count; ++i) {
      var type_def = {};
      ns_types[i] = type_def;

      var field_list = [];
      var construct_list = [];
      
      var m_count = buf.rd_i32();
      var p_count = buf.rd_i32();
      var ty_name_sid = buf.rd_i32();

      var members = [];
      type_def.members = members;
      var name = str_idx[ty_name_sid];
      type_def.name = name;
      for (var m = 0; m < m_count; ++m) {
        var member = {};
        members.push(member);
        var member_name_sid = buf.rd_i32();
        member.name = str_idx[member_name_sid];
        var member_type = buf.rd_i32();
        member.type = member_type;
        if (member_type == adata_et_type) {
          var member_typename_sid = buf.rd_i32();
          member.typename = str_idx[member_typename_sid];
          var namespace_idx = buf.rd_i32();
          
          if (namespace_idx == -1) {
            member.typename = member.typename;
            member.type_def = types[member.typename];
          }
          else {
            member.type_def = ns_types[namespace_idx];
          }
        }
        var member_del = buf.rd_i32();
        member.del = member_del;
        if (member_del == 0) {
          field_list.push(member.name);
          var construct_value = decode_default_value(buf, member);
          member.default_value = construct_value;
          construct_list.push(construct_value);
        }
        member.size = buf.rd_i32();
        var params = [];
        member.params = params;
        var param_count = buf.rd_i32();
        for (var p = 0; p < param_count; ++p){
          var ptype = {};
          var p_type = buf.rd_i32();
          ptype.type = p_type;
          if (p_type == adata_et_type) {
            var ptype_typename_sid = buf.rd_i32();
            ptype.typname = str_idx[ptype_typename_sid];
            var namespace_idx = buf.rd_i32();
            if (namespace_idx == -1) {
              ptype.typname = ptype.typname;
              ptype.type_def = types[ptype.typename];
            }
            else {
              ptype.type_def = ns_types[namespace_idx];
            }
          }
          ptype.size = buf.rd_i32();
          params.push(ptype);
        }
      }
      types[ns_name + '.' + name] = type_def;
      ns_types.push(type_def);
      make_type(ns_name, type_def, field_list, construct_list);
    }
  }

  var parser_pack = function (buf){
    var count = buf.rd_i32();
    for (var i = 0; i < count; ++i) {
      parser_namespace(buf);
    }
  }

  var parser_adt = function (buf , data){
    buf.set(data, data.byteLength);
    parser_pack(buf);
  }
  
  adata.prototype.load_from_buf = function (contents){
    parser_adt(this , contents);
  }

  adata.prototype.load = function (filename , cb) {
    var reader = new FileReader();
    var obj = this;
    reader.onload = function (event) {
      var contents = event.target.result;
      parser_adt(obj , contents);
      cb();
    };
    reader.onerror = function (event) {
      console.error(filename + " could not be read! Code " + event.target.error.code);
    };
    reader.readAsArrayBuffer(new File(filename));
  }

  if (typeof exports !== 'undefined') {
    if (typeof module !== 'undefined' && module.exports) {
      exports = module.exports = adata;
    }
    exports.adata = adata;
  }else if (typeof define === 'function' && define["amd"])/* AMD */ 
    define(function () { return adata; });
  else/* Global */ 
    (global["adata"] = global["adata"] || {})["adata"] = adata;
})(this);