#include <iostream>

#include "parser.h"

namespace
{

	char string_code[256] = { 0 };

	struct init_helper
	{
		init_helper()
		{
			for (char c = 'A'; c <= 'Z'; ++c)
			{
				string_code[c] = 1;
			}
			for (char c = 'a'; c <= 'z'; ++c)
			{
				string_code[c] = 1;
			}
			string_code['_'] = 1;
		}
	};

	init_helper g_init_helper;

	inline bool is_digest( char v )
	{
		return v >= '0' && v <= '9';
	}

	inline bool is_source_trem(char v)
	{
		return v == '<' || v == '>' || v == ':' || v == ',';
	}

	inline bool is_string_header( char v )
	{
		return string_code[v] > 0;
	}

	inline bool is_number_header(char v)
	{
		return is_digest(v) || v == '-';
	}

	const char lower_case_delta = 'a' - 'A';

	inline char lower_case_char(char v)
	{
		if( v >= 'A' && v <= 'Z')
		{
			return v + lower_case_delta;
		}
		return v;
	}

	inline bool is_ws(char v)
	{
		return v == ' ' || v == '\t';
	}
}

struct parse_execption : std::exception
{
	parse_execption(const char * error_info,int line,int col, std::string include)
  :exception(error_info), m_lines(line), m_cols(col), m_include(std::move(include))
	{}
	int m_lines;
	int m_cols;
  std::string m_include;
};

struct paser_adl
{
	descrip_define& m_define;
	char *	m_doc;
	int			m_len;
	int			m_cols;
	int			m_lines;
  std::string m_include;
	bool		m_eof;

	paser_adl(descrip_define& define, char * doc, int len)
		:m_define(define), m_doc(doc), m_len(len), m_cols(0), m_lines(0), m_eof(false)
	{

	}

	inline char read_char()
	{
		char c = *m_doc;
		if(c > 0)
		{
			++m_doc;
			++m_cols;
		}
		else
		{
			m_eof = true;
		}
		if( c == '\n')
		{
			++m_lines;
			m_cols = 0;
		}
		return c;
	}

	inline char peek_char()
	{
		char c = *m_doc;
		return c;
	}

	char skip_ws()
	{
		char c;
		while ((c = read_char())!=0)
		{
			switch (c)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			{
				break;
			}
      case '/':
      {
        c = read_char();
        if(c == '/')
        {
          while((c = read_char())!=0)
          {
            if(c == '\n')
            {
              break;
            }
          }
        }
        else
        {
          m_doc -= 2;
          m_cols -= 2;
        }
      }
			default:
			{
				return c;
			}
			}
		}
		return c;
	}

	void parser_string_body(char * pos, bool lower_case = true)
	{
		char c;
		while ((c = read_char())!=0)
		{
			if (!is_string_header(c) && !is_digest(c))
			{
				break;
			}
			*pos++ = lower_case ? lower_case_char(c) : c;
		}
		*pos = 0;
		--m_doc;
    --m_cols;
	}

	inline std::string parser_string(bool lower_case = true)
	{
		char c = skip_ws();
		char indetify[256] = { 0 };
		if(c && !is_string_header(c))
		{
			throw parse_execption("unknow identity",m_lines,m_cols, m_include);
		}
		int len = 0;
		indetify[len++] = lower_case ? lower_case_char(c) : c;
		parser_string_body(indetify + 1, lower_case);
		return std::string(indetify);
	}

	void parser_source_string_body(char * pos)
	{
		char c;
		while ((c = read_char()))
		{
			if (!is_string_header(c) && !is_digest(c) && !is_source_trem(c))
			{
				break;
			}
			*pos++ = c;
		}
		*pos = 0;
		--m_doc;
    --m_cols;
	}

	inline std::string parser_source_string()
	{
		char c = skip_ws();
		char indetify[256] = { 0 };
		if (c && !is_string_header(c))
		{
      throw parse_execption("unknow identity", m_lines, m_cols, m_include);
		}
		int len = 0;
		indetify[len++] = c;
		parser_source_string_body(indetify + 1);
		return std::string(indetify);
	}

	void parser_number_body(char * pos)
	{
		int dot_count = 0;
		char c;
		while ((c = read_char())!=0)
		{
			if (!is_digest(c))
			{
				if (c == '.')
				{
					if (dot_count == 1)
					{
						break;
					}
					else
					{
						++dot_count;
					}
				}
				else
				{
					break;
				}
			}
			*pos++ = c;
		}
		*pos = 0;
		--m_doc;
    --m_cols;
	}

	inline std::string parser_number()
	{
		char c = skip_ws();
		char indetify[256] = { 0 };
		if (!is_number_header(c))
		{
      throw parse_execption("unknow identity", m_lines, m_cols, m_include);
		}
		int len = 0;
		indetify[len++] = c;
		parser_number_body(indetify + 1);
		return std::string(indetify);
	}

	inline std::string parser_value()
	{
		char c = skip_ws();
		char indetify[256] = { 0 };
		int len = 0;
		if (is_string_header(c))
		{
			indetify[len++] = c;
			parser_string_body(indetify + 1, false);
		}
		else if(is_number_header(c))
		{
			indetify[len++] = c;
			parser_number_body(indetify + 1);
		}
		else
		{
      throw parse_execption("unknow value format, must be string or number", m_lines, m_cols, m_include);
		}
		return std::string(indetify);
	}

	inline std::string parser_source_value()
	{
		char c = skip_ws();
		char indetify[256] = { 0 };
		int len = 0;
		if (is_string_header(c))
		{
			indetify[len++] = c;
			parser_source_string_body(indetify + 1);
		}
		else if (is_number_header(c))
		{
			indetify[len++] = c;
			parser_number_body(indetify + 1);
		}
		else
		{
      throw parse_execption("unknow value format, must be string or number", m_lines, m_cols, m_include);
		}
		return std::string(indetify);
	}

	void parser_namespace()
	{
		char c = skip_ws();
		if ( c == '=' )
		{
			do 
			{
				std::string identity = parser_string();
				if (identity.empty())
				{
					throw parse_execption("namespace syntax error , usage namespace = data.xyz;", m_lines, m_cols, m_include);
				}
				m_define.m_namespace.m_fullname += identity;
				c = skip_ws();
				if(c == '.')
				{
					m_define.m_namespace.m_fullname += ".";
					m_define.m_namespace.m_names.push_back(identity);
					continue;
				}
				if (c == ';')
				{
					m_define.m_namespace.m_names.push_back(identity);
					return;
				}
			} while (!m_eof);
			throw parse_execption("namespace syntax error , miss ; at line end", m_lines, m_cols, m_include);
		}
		else
		{
			throw parse_execption("namespace syntax error,usage namespace = data.xyz;", m_lines, m_cols, m_include);
		}
	}

	inline void parser_option()
	{
		std::string identity = parser_string();
		if (identity.empty())
		{
			throw parse_execption("option syntax error , option name not define , usage option cpp_alloc = user_alloc;", m_lines, m_cols, m_include);
		}
		char c = skip_ws();
		if (c == '=')
		{
			std::string value = parser_source_value();
				if (value.empty())
				{
					throw parse_execption("option syntax error , option value not define , usage option cpp_alloc = user_alloc;", m_lines, m_cols, m_include);
				}
				c = skip_ws();
				if (c == ';')
				{
					option_value v;
					v.m_value = value;
					v.m_parser_lines = m_lines;
					v.m_parser_cols = m_cols;
					m_define.m_option_values.insert(make_pair(identity,v));
					return;
				}
			throw parse_execption("option syntax error , miss ; at line end", m_lines, m_cols, m_include);
		}
		else
		{
			throw parse_execption("option syntax error,usage option opt1 = 3758;", m_lines, m_cols, m_include);
		}
	}

	void parser_template_parameter(type_define& , member_define& f_define , int parmeter_count)
	{
		char c = skip_ws();
		if( c == '<')
		{
			do 
			{
				std::string paramter_type_name = parser_string();
				if (paramter_type_name.empty())
				{
					throw parse_execption("type syntax error ,member type declaration paramter type, usage list<int32> list_value;", m_lines, m_cols, m_include);
				}

        if (m_define.has_decl_type(paramter_type_name))
				{
					e_base_type p_type = get_type(paramter_type_name);
					if (is_container(p_type))
					{
						throw parse_execption("type member syntax error , container parameter couldn't be container", m_lines, m_cols, m_include);
					}
					member_define p_define;
					p_define.m_typename = paramter_type_name;
					p_define.m_type = p_type;
					p_define.m_parser_lines = m_lines;
					p_define.m_parser_cols = m_cols;
					if(p_type == e_base_type::string)
					{
						c = skip_ws();
						if( c == '(')
						{
							std::string size = parser_number();
							c = skip_ws();
							if(c == ')')
							{
								p_define.m_size = size;
							}
							else
							{
								throw parse_execption("type member syntax error , string size limit option miss ) at end", m_lines, m_cols, m_include);
							}
						}
						else
						{
							--m_doc;
              --m_cols;
						}
					}
					--parmeter_count;
					if(parmeter_count)
					{
						c = skip_ws();
						if( c != ',')
						{
							throw parse_execption("type member syntax error , container parameter type too less", m_lines, m_cols, m_include);
						}
					}
					f_define.m_template_parameters.push_back(p_define);
				}
				else
				{
					throw parse_execption("type member syntax error ,unknow type declaration", m_lines, m_cols, m_include);
				}
			} while (!m_eof && parmeter_count);
			c = skip_ws();
			if(c != '>')
			{
				throw parse_execption("type member syntax error , container type declaration miss > at end", m_lines, m_cols, m_include);
			}
		}
		else
		{
			throw parse_execption("type member syntax error , container type declaration miss <", m_lines, m_cols, m_include);
		}
	}

	inline void parser_member(type_define& t_define, member_define& f_define)
	{
		f_define.m_type = get_type(f_define.m_typename);
		int parmeter_count = 0;
		switch (f_define.m_type)
		{
		case e_base_type::list:
		{
			parmeter_count = 1;
			break;
		}
		case e_base_type::map:
		{
			parmeter_count = 2;
			break;
		}
		default:
		{
			break;
		}
		}
		if(parmeter_count > 0)
		{
			parser_template_parameter(t_define, f_define, parmeter_count);
		}
		char c = read_char();
		if(is_ws(c))
		{
			std::string member_name = parser_string();
			if(member_name.empty())
			{
				throw parse_execption("type syntax error ,member type declaration , usage int32 value = 1;", m_lines, m_cols, m_include);
			}
			if(t_define.has_member(member_name))
			{
				throw parse_execption("type member syntax error , redefine member name", m_lines, m_cols, m_include);
			}
			f_define.m_name = member_name;
			do 
			{
				c = skip_ws();
				if (c == ';')
				{
					break;
				}
				else if( c == '=')
				{
					if(f_define.m_default_value.empty())
					{
						f_define.m_default_value = parser_number();
					}
					else
					{
						throw parse_execption("type member syntax error , redefine default value", m_lines, m_cols, m_include);
					}
				}
				else if( c == '(')
				{
					std::string range_value = parser_number();
					c = skip_ws();
					if( c == ')')
					{
						f_define.m_size = range_value;
					}
					else
					{
						throw parse_execption("type member syntax error , range miss ) at end", m_lines, m_cols, m_include);
					}
				}
				else if (c == '[')
				{
					std::string option_name = parser_string();
					if (option_name.empty())
					{
						throw parse_execption("type syntax error ,member type declaration option, usage int32 value = 1[delete];", m_lines, m_cols, m_include);
					}

					std::string option_value;
					auto find = f_define.m_options.find(option_name);
					if(find != f_define.m_options.end())
					{
						throw parse_execption("type member syntax error , redefine option", m_lines, m_cols, m_include);
					}
					c = skip_ws();
					if( c == '=' )
					{
						option_value = parser_value();
						c = skip_ws();
					}
					if( c == ']')
					{
						f_define.m_options.insert(std::make_pair(option_name, option_value));
					}
					else
					{
						throw parse_execption("type member syntax error , option miss ] at end", m_lines, m_cols, m_include);
					}
				}
			} while (!m_eof);
		}
		else
		{
			throw parse_execption("type member syntax error , miss space after type declaration", m_lines, m_cols, m_include);
		}
	}

	void parser_type(type_define& t_define)
	{
		char c = skip_ws();
		if( c != '{')
		{
			throw parse_execption("type syntax error , miss { after type declaration", m_lines, m_cols, m_include);
		}
		do 
		{
			char c = skip_ws();
			if ( c == '}')
			{
				return;
			}
			if(is_string_header(c))
			{
				--m_doc;
        --m_cols;
				std::string member_type_name = parser_string();
				if(member_type_name.empty())
				{
					throw parse_execption("type syntax error ,member type declaration , usage int32 value = 1;", m_lines, m_cols, m_include);
				}
        
				if (m_define.has_decl_type(member_type_name))
				{
					member_define f_define;
					f_define.m_parser_lines = m_lines;
					f_define.m_parser_cols = m_cols;
					f_define.m_typename = member_type_name;
					t_define.m_members.push_back(f_define);
					parser_member(t_define, t_define.m_members[t_define.m_members.size() - 1]);
				}
				else
				{
					throw parse_execption("type syntax error ,unknow member type declaration", m_lines, m_cols, m_include);
				}
			}
		} while (!m_eof);
	}

	inline bool valid_integer_value_string(const std::string& value)
	{
		for (auto c : value)
		{
			if (c < '0' && c > '9')
			{
				return false;
			}
		}
		return true;
	}

	inline bool valid_float_value_string(const std::string& value)
	{
		int dot_count = 0;
		for (auto c : value)
		{
			if (c == '.')
			{
				if (dot_count > 0)
				{
					return false;
				}
				++dot_count;
				continue;
			}
			if (c < '0' && c > '9')
			{
				return false;
			}
		}
		return true;
	}

	void valid()
	{
		if(m_define.m_namespace.m_names.size())
		{
      bool first_element = true;
			for (auto& name : m_define.m_namespace.m_names)
			{
				m_define.m_namespace.m_cpp_fullname += "::";
				m_define.m_namespace.m_cpp_fullname += name;
        if(first_element)
        {
          first_element = false;
        }
        else
        {
          m_define.m_namespace.m_lua_fullname += "_";
          m_define.m_namespace.m_js_fullname += "_";
          m_define.m_namespace.m_csharp_fullname += ".";
        }
        m_define.m_namespace.m_lua_fullname += name;
        m_define.m_namespace.m_js_fullname += name;
        m_define.m_namespace.m_csharp_fullname += name;
      }
			m_define.m_namespace.m_cpp_fullname += "::";
		}
		for (auto& opt : m_define.m_option_values)
		{
			option_value& v = opt.second;
			if (opt.first == "cpp_alloc")
			{
				if( is_string_header(v.m_value[0]) )
				{
					m_define.m_option.m_cpp_allocator = v.m_value;
				}
				else
				{
					throw parse_execption("option syntax error ,cpp_alloc option parameter invalid", v.m_parser_lines, v.m_parser_cols, v.m_parser_include);
				}
			}
			else
			{
        throw parse_execption("option syntax error ,unknow option parameter", v.m_parser_lines, v.m_parser_cols, v.m_parser_include);
			}
		}

		for( auto& tdefine : m_define.m_types)
		{
      if (tdefine.m_members.size() > 63)
      {
        throw parse_execption("type syntax error ,member of type max is 63", tdefine.m_parser_lines, tdefine.m_parser_cols, tdefine.m_parser_include);
      }
			for( auto& member : tdefine.m_members)
			{
        if (member.m_type == e_base_type::type)
        {
          type_define * tdef = (type_define *)m_define.find_decl_type(member.m_typename);
          member.m_typedef = tdef;
        }
				if (member.is_fixed())
				{
					member.m_type = (e_base_type)(member.m_type + e_base_type::int8 - e_base_type::fix_int8);
					member.m_fixed = true;
				}
				if (member.is_integer())
				{
					if (member.m_default_value.length())
					{
						if (!valid_integer_value_string(member.m_default_value))
						{
              throw parse_execption("member syntax error ,default value is not an integer", member.m_parser_lines, member.m_parser_cols, member.m_parser_include);
						}
					}
					else
					{
						member.m_default_value = "0";
					}
				}
				if (member.is_float())
				{
					if (member.m_default_value.length())
					{
						if (!valid_float_value_string(member.m_default_value))
						{
              throw parse_execption("member syntax error ,default value is not a float", member.m_parser_lines, member.m_parser_cols, member.m_parser_include);
						}
					}
					else
					{
						member.m_default_value = "0.0";
					}
				}
				if(member.is_container())
				{
          tdefine.m_ismulti = true;
					if (member.m_size.length())
					{
						if (!valid_integer_value_string(member.m_size))
						{
              throw parse_execption("member syntax error ,container size limit option is not an integer", member.m_parser_lines, member.m_parser_cols, member.m_parser_include);
						}
						int vmax = atoi(member.m_size.c_str());
						if(vmax <= 0)
						{
              throw parse_execption("member syntax error ,container size limit option should > 0", member.m_parser_lines, member.m_parser_cols, member.m_parser_include);
						}
					}
					for (auto& ptype : member.m_template_parameters)
					{
            if (ptype.m_type == e_base_type::type)
            {
              type_define * tdef = (type_define *)m_define.find_decl_type(ptype.m_typename);
              ptype.m_typedef = tdef;
            }
            if (ptype.m_type == e_base_type::string)
						{
							if (ptype.m_size.length())
							{
								if (!valid_integer_value_string(ptype.m_size))
								{
                  throw parse_execption("member syntax error ,string size limit option is not an integer", ptype.m_parser_lines, ptype.m_parser_cols, ptype.m_parser_include);
								}
								int vmax = atoi(ptype.m_size.c_str());
								if (vmax <= 0)
								{
                  throw parse_execption("member syntax error ,string size limit option should > 0", ptype.m_parser_lines, ptype.m_parser_cols, ptype.m_parser_include);
								}
							}
						}
					}
					if (member.m_template_parameters.size() >= 2)
					{
						member.m_template_parameters[1].m_name = "second";
					}
					if (member.m_template_parameters.size() >= 1)
					{
						member.m_template_parameters[0].m_name = "first";
					}
				}
				for (auto& option : member.m_options)
				{
					if (option.first == "delete")
					{
						member.m_deleted = true;
					}
				}
			}
		}
	}

	void parser()
	{
		while(!m_eof)
		{
			std::string identity = parser_string();
			if(identity.empty())
			{
				return;
			}
			if (identity == "namespace")
			{
				if (m_define.m_namespace.m_fullname.empty())
				{
					parser_namespace();
				}
				else
				{
					throw parse_execption("redefine namespace", m_lines, m_cols, m_include);
				}
			}
			else
			{
				if( m_define.find_decl_type(identity) )
				{
					throw parse_execption("redefine type", m_lines, m_cols, m_include);
				}
				else
				{
					type_define t_define;
					t_define.m_parser_lines = m_lines;
					t_define.m_parser_cols = m_cols;
          t_define.m_parser_include = m_include;
					t_define.m_name = identity;
          t_define.m_namespace = &m_define.m_namespace;
          t_define.m_index = (int)m_define.m_types.size();
					m_define.m_types.push_back(t_define);
					parser_type(m_define.m_types[m_define.m_types.size() - 1]);
				}
			}
		}
	}
private:
  paser_adl& operator = (const paser_adl&);
};

bool load_from_adl(descrip_define& define, const std::string& adl_file, std::string& error_message)
{
	char * read_buffer;
	FILE * fd;
	if (0 != ::fopen_s(&fd, adl_file.c_str(), "r"))
	{
		error_message = "open file error.";
		return false;
	}

	fseek(fd, 0, SEEK_END);

	int size = ftell(fd);
	read_buffer = new char[size + 1];
	fseek(fd, 0, SEEK_SET);
	std::size_t len = fread(read_buffer, 1, size, fd);
	read_buffer[len] = 0;
	try
	{
		paser_adl parser(define, read_buffer, size);
		parser.parser();
		parser.valid();
	}
	catch (parse_execption& e)
	{
    std::cerr << "parser file :" << adl_file << "(";
    if (!e.m_include.empty())
    {
      std::cerr << "include - " << e.m_include << ", ";
    }
    std::cerr << "line " << e.m_lines << ":col " 
      << e.m_cols << ")" << " error:" << e.what() << std::endl;
	}
	delete[] read_buffer;
	return true;
}
