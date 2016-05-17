#pragma once

#include <cstdint>
//#include <cstring>
//#include <string>

namespace alexer
{
  struct token
  {
    enum e_type
    {
      end = 0,
      alpha = 1,
      str_cat = alpha << 1,
      space = str_cat << 1,
      end_line = space << 1,
      digit = end_line << 1,
      digit_hex = digit << 1,
      punct = digit_hex << 1,
    };

    int type = end;
    int value = 0;

    static inline int get_type(int val)
    {
      static uint8_t table[] = {
        0, 1, 1, 1, 1, 1, 1, 1, 1, 4, 8, 1, 1, 4, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        4, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 66, 64, 64,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 64, 64, 64, 64, 64, 64,
        64, 33, 33, 33, 33, 33, 33, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 64, 64, 3,
        64, 33, 33, 33, 33, 33, 33, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 64, 64, 1 };
      if (val < 0 && val > 127)
        return alpha;
      return table[val];
    }
  };

  struct symbol
  {
    enum e_type
    {
      sym_end = 0,
      sym_string = 1,
      sym_number = sym_string << 1,
      sym_punct = sym_number << 1,
      sym_space = sym_punct << 1,
      sym_endline = sym_space << 1,
      sym_ctrl = sym_endline << 1,
      sym_comment = sym_ctrl << 1,
      sym_literal = sym_comment << 1,
    };

    enum number_type
    {
      nt_none,
      nt_float,
      nt_double,
      nt_bin,
      nt_oct,
      nt_dec,
      nt_hex,
    };

    int type = sym_end;
    bool sign = false;
    number_type ntype = nt_none;
    size_t start_offset = 0;
    size_t length = 0;
    size_t start_file_col = 0;
    size_t start_file_line = 0;
    size_t end_file_col = 0;
    size_t end_file_line = 0;
    char const * text = nullptr;

    union
    {
      uint64_t  u64;
      double    d64;
    } value;

    inline bool cmp(std::string const& parttern)
    {
      if (length != parttern.length())
        return false;
      for (size_t i = 0; 0 < length; ++i)
      {
        if (text[i] != parttern[i])
          return false;
      }
      return true;
    }

    inline bool cmp(char parttern)
    {
      if (length != 1)
        return false;
      return text[0] == parttern;
    }
  };

  class lexer
  {
    char const * buffer = nullptr;
    size_t current_offset = 0;
    size_t end_offset = 0;
    size_t current_file_col = 1;
    size_t current_file_line = 0;

    double decimal = 0.1;
    inline void reset_decimal()
    {
      decimal = 0.1;
    }

  public:
    static const int skip_default = symbol::sym_space | symbol::sym_endline | symbol::sym_ctrl;
    static const int skip_space_line = symbol::sym_space | symbol::sym_endline;
    static const int skip_space = symbol::sym_space;

    static const int space_mark = token::space;
    static const int space_line_mark = token::space | token::end_line;
    static const int string_mark = token::alpha | token::str_cat | token::digit;
    static const int digit_mark = token::digit;
    static const int hex_digit_mark = token::digit | token::digit_hex;

    inline bool match(int t1, int t2)
    {
      return (t1 & t2) > 0;
    }

    lexer(char * s) :lexer{ s, std::strlen(s) }{}

    lexer(char const * s , size_t len)
    {
      reset(s , len);
    }

    inline void reset(char const * s, size_t len)
    {
      buffer = s;
      current_offset = 0;
      end_offset = len;
    }

    inline char peek()
    {
      if (current_offset < end_offset)
      {
        auto val = buffer[current_offset];
        if (val == '\n')
        {
          ++current_file_line;
          current_file_col = 1;
        }
        return val;
      }
      return 0;
    }

    inline void next()
    {
      if (current_offset < end_offset)
      {
        ++current_offset;
        ++current_file_col;
      }
    }

    inline char next_peek()
    {
      next();
      return peek();
    }

    void parser_digit_symbol(symbol& sym, int first)
    {
      auto val = peek();
      auto type = token::get_type(val);
      if (first == '0')
      {
        if (type == token::digit)
        {
          next();
          sym.ntype = symbol::nt_oct;
          parser_oct_digit_symbol(sym);
          return;
        }
        else
        {
          if (val == '.')
          {
            next();
            parser_decimal_digit_symbol(sym);
            return;
          }
          else if (val == 'B' || val == 'b')
          {
            next();
            sym.ntype = symbol::nt_bin;
            parser_bin_digit_symbol(sym);
            return;
          }
          else if (val == 'X' || val == 'x')
          {
            next();
            sym.ntype = symbol::nt_hex;
            parser_hex_digit_symbol(sym);
            return;
          }
          return;
        }
      }
      while (type == token::digit)
      {
        val = next_peek();
        type = token::get_type(val);
        sym.value.u64 *= 8;
        sym.value.u64 += val - '0';
      }
      if (val == '.')
      {
        next();
        parser_decimal_digit_symbol(sym);
      }
      else if (val == 'f' || val == 'F')
      {
        sym.ntype = symbol::nt_float;
        next();
      }
      else if (val == 'u' || val == 'U')
      {
        next();
        parser_int_long_sufix(sym);
      }
      else
      {
        parser_int_long_sufix(sym);
      }
    }

    void parser_bin_digit_symbol(symbol& sym)
    {
      auto val = peek();
      while (val == '0' || val == '1')
      {
        val = next_peek();
        sym.value.u64 *= 2;
        sym.value.u64 += val - '0';
      }
    }

    void parser_oct_digit_symbol(symbol& sym)
    {
      auto val = peek();
      while (val >= '0' && val <= '7')
      {
        val = next_peek();
        sym.value.u64 *= 8;
        sym.value.u64 += val - '0';
      }
    }

    void parser_hex_digit_symbol(symbol& sym)
    {
      auto val = peek();
      while (match(token::get_type(val), digit_mark))
      {
        val = next_peek();
        sym.value.u64 *= 16;
        if(val>= '0' && val <= '9')
          sym.value.u64 += val - '0';
        if (val >= 'a' && val <= 'f')
          sym.value.u64 += val - 'a' + 10;
        if (val >= 'A' && val <= 'F')
          sym.value.u64 += val - 'A' + 10;
      }
    }

    void parser_decimal_digit_symbol(symbol& sym)
    {
      sym.ntype = symbol::nt_double;
      sym.value.d64 = (double)sym.value.u64;
      reset_decimal();
      auto val = peek();
      while (match(token::get_type(val), token::digit))
      {
        val = next_peek();
        sym.value.d64 += decimal * (val - '0');
        decimal *= 0.1;
      }
      if (val == 'f' || val == 'F')
      {
        sym.ntype = symbol::nt_float;        
        next();
      }
    }

    void parser_int_long_sufix(symbol& sym)
    {
      auto val = peek();
      if (val == 'l' || val == 'L')
      {
        val = next_peek();
        if (val == 'l' || val == 'L')
        {
          next();
        }
      }
    }

    void parser_string_symbol(symbol& sym)
    {
      auto val = peek();
      while (match(token::get_type(val), string_mark))
      {
        val = next_peek();
      }
    }

    void parser_literal_symbol(symbol& sym, char quotaion, char escape)
    {
      sym.type = symbol::sym_literal;
      auto val = peek();
      do
      {
        if (val == escape)
        {
          val = next_peek();
          if (val == quotaion)
          {
            val = next_peek();
            continue;
          }
        }
        else if (val == quotaion)
        {
          next();
          break;
        }
        else if (val == 0)
        {
          break;
        }
        val = next_peek();
      } while (true);
    }

    void parser_commet_symbol(symbol& sym)
    {
      auto val = peek();
      if (val != '/')
        return;
      sym.type = symbol::sym_comment;
      while (val != 0 && val != '\n')
      {
        val = next_peek();
      }
    }

    symbol parser_symbol()
    {
      auto val = peek();
      symbol sym;
      sym.start_offset = current_offset;
      sym.start_file_col = current_file_col;
      sym.start_file_line = current_file_line;

      auto type = token::get_type(val);
      if (match(type, digit_mark))
      {
        sym.type = symbol::sym_number;
        sym.ntype = symbol::nt_dec;
        next();
        parser_digit_symbol(sym, val);
      }
      else if (match(type, space_line_mark))
      {
        sym.type = symbol::sym_space;
        val = next_peek();
        while (match(token::get_type(val), space_line_mark))
        {
          val = next_peek();
        }
      }
      else if (match(type, token::alpha))
      {
        sym.type = symbol::sym_string;
        next();
        parser_string_symbol(sym);
      }
      else if (match(type, token::punct))
      {
        sym.type = symbol::sym_punct;
        next();
        if (val == '/')
        {
          parser_commet_symbol(sym);
        }
        else if (val == '\'')
        {
          parser_literal_symbol(sym, '\'', '\\');
        }
        else if (val == '"')
        {
          parser_literal_symbol(sym, '"', '\\');
        }
      }
      else if (val == '\n')
      {
        sym.type = symbol::sym_end;
        next();
      }
      else if (val == 0)
      {
        sym.type = symbol::sym_end;
      }
      sym.length = current_offset - sym.start_offset;
      sym.end_file_col = current_file_col;
      sym.end_file_line = current_file_line;
      sym.text = buffer + sym.start_offset;
      return sym;
    }

    symbol parser(int skip_parttern = skip_space_line)
    {
      auto val = parser_symbol();
      while (match(val.type, skip_parttern))
      {
        val = parser();
      }
      return val;
    }
    
    inline bool expect(symbol& sym , int parttern, int skip_parttern = 0)
    {
      sym = parser(skip_parttern);
      return match(sym.type, parttern);
    }
    
    inline bool expect(symbol& sym, std::string const& parttern, int skip_parttern = 0)
    {
      sym = parser(skip_parttern);
      return sym.cmp(parttern);
    }
    
    inline bool expect(symbol& sym, char parttern, int skip_parttern = 0)
    {
      sym = parser(skip_parttern);
      return sym.cmp(parttern);
    }

    symbol want(int parttern = 0)
    {
      auto val = parser_symbol();
      while (!match(val.type, parttern))
      {
        val = parser_symbol();
      }
      return val;
    }
  };
}

