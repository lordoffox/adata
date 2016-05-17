#pragma once
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <vector>

namespace aarg
{
  struct option
  {
    bool set = false;
    std::string short_opt;
    std::string long_opt;
    std::vector<std::string> args;
    std::function<void (option&)> cb;
    std::string help;

    std::string opt_name() const
    {
      std::string name;
      if (!this->short_opt.empty())
      {
        name += this->short_opt;
        name += '|';
      }
      name += this->long_opt;
      return name;
    }

    inline void parse(bool& v)
    {
      if (this->args.size() > 1)
      {
        throw std::invalid_argument("option:" + opt_name() + " wrong arguments!");
      }
      if (this->args.size() == 1)
      {
        auto& arg = this->args[0];
        if(arg.length() == 2)
        {
          if( (arg[0] == 'O' || arg[0] == 'o') && 
            (arg[1] == 'N' || arg[1] == 'n'))
          {
            v = true;
            return;
          }
        }
        if (arg.length() == 3)
        {
          if ((arg[0] == 'Y' || arg[0] == 'y') &&
            (arg[1] == 'E' || arg[1] == 'e')&&
            (arg[2] == 'S' || arg[2] == 's'))
          {
            v = true;
            return;
          }
        }
        if (arg.length() == 4)
        {
          if ((arg[0] == 'T' || arg[0] == 't') &&
            (arg[1] == 'R' || arg[1] == 'r') &&
            (arg[2] == 'U' || arg[2] == 'u') &&
            (arg[3] == 'E' || arg[3] == 'e'))
          {
            v = true;
            return;
          }
        }
        v = false;
        return;
      }
      else
      {
        v = set;
      }
      return;
    }

    inline bool check_single_arg()
    {
      return this->args.size() == 1;
    }

    template<typename value_type>
    inline typename ::std::enable_if<::std::is_signed<value_type>::value && ::std::is_integral<value_type>::value, void>::type
      convert(std::string const& str , value_type& v)
    {
      v = (value_type)std::strtoll(str.c_str(), nullptr, 10);
    }

    template<typename value_type>
    inline typename ::std::enable_if<::std::is_unsigned<value_type>::value && ::std::is_integral<value_type>::value, void>::type
      convert(std::string const& str, value_type& v)
    {
      v = (value_type)std::strtoull(str.c_str(), nullptr, 10);
    }

    template<typename value_type>
    inline typename ::std::enable_if<::std::is_floating_point<value_type>::value, void>::type
      convert(std::string const& str, value_type& v)
    {
      v = (value_type)std::strtod(arg.c_str(), nullptr, 10);
    }

    void convert(std::string const& str, std::string& v)
    {
      v = str;
    }

    template<typename value_type>
    void parse(value_type& v)
    {
      if(check_single_arg())
      {
        convert(this->args[0], v);
      }
    }

    template<typename value_type , typename alloc_ty>
    void parse(std::vector<value_type, alloc_ty>& v)
    {
      v.clear();
      for (auto& arg : this->args)
      {
        v.emplace_back();
        convert(arg, v.back());
      }
    }
  };

  class argopt
  {
  private:
    std::vector<option> options_;
    std::map<std::string, option *> opt_map_;
  public:
    std::vector<std::string> unknow_opthins;

    template<typename ty>
    argopt& add(char short_opt , char const * long_opt , char const * help , ty& value)
    {
      if(short_opt == 0 && long_opt == nullptr)
      {
        throw std::invalid_argument("argopt bad argument.");
      }
      this->options_.emplace_back();
      option& opt = this->options_.back();
      opt.short_opt = short_opt;
      opt.long_opt = long_opt;
      opt.help = help;
      opt.cb = [&](option& opt_ ){ opt_.parse(value); };
      return *this;
    }

    void parse(int argc , char ** argv)
    {
      this->opt_map_.clear();
      for(auto& opt : this->options_)
      {
        if (!opt.short_opt.empty())
        {
          this->opt_map_[opt.short_opt] = &opt;
        }
        if (!opt.long_opt.empty())
        {
          this->opt_map_[opt.long_opt] = &opt;
        }
      }
      int count = 1;
      option * current_opt = nullptr;
      while (count < argc )
      {
        char * arg = argv[count++];
        do 
        {
          char * ptr = arg;
          auto c = *ptr;
          if (c == '-')
          {
            ++ptr;
            c = *ptr;
            if (c == 0)
            {
              this->unknow_opthins.push_back(arg);
              break;
            }
            if (c == '-')
            {
              ++ptr;
              auto p = this->opt_map_.find(ptr);
              if (p == this->opt_map_.end())
              {
                this->unknow_opthins.push_back(arg);
              }
              else
              {
                current_opt = p->second;
                current_opt->set = true;
              }
            }
            else
            {
              std::string key(ptr, 1);
              ++ptr;
              auto p = this->opt_map_.find(key);
              if (p == this->opt_map_.end())
              {
                this->unknow_opthins.push_back(arg);
              }
              else
              {
                current_opt = p->second;
                current_opt->set = true;
                current_opt->args.push_back(ptr);
              }
            }
            break;
          }
          else
          {
            if (current_opt != nullptr)
            {
              current_opt->args.push_back(arg);
            }
            else
            {
              this->unknow_opthins.push_back(arg);
            }
          }
        } while (0);
      }
      for (auto& opt : this->options_)
      {
        if(opt.set == true)
        {
          opt.cb(opt);
        }
      }
    }

    std::string desc() const
    {
      std::stringstream ss;
      for (auto& opt : this->options_)
      {
        if(!opt.short_opt.empty())
        {
          ss << '-' << opt.short_opt << " ";
        }
        if(!opt.long_opt.empty())
        {
          ss << "--" << opt.long_opt << " ";
        }
        ss << opt.help << std::endl;
      }
      return ss.str();
    }

    inline bool be_set(char const * opt)
    {
      auto p = this->opt_map_.find(opt);
      if (p != this->opt_map_.end())
      {
        return p->second->set;
      }
      return false;
    }
  };

}
