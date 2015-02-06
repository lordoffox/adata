ADATA v1.0
=======

ADATA is an efficient cross platform serialization library for C/C++, with support for Lua, C#, JavaScript and Java.
Note: JavaScript and Java not support yet.

Features Overview
---------------

* Lightweight, fast and efficient serialization implementations, specifically for game development and other performance-critical applications
* Flexible, using keyword "delete" to mark discarded fields, that means forwards compatibility
* Header only, no need to build library, just a fews hpp files
* Strongly typed, errors happen at compile time
* Cross platform C/C++(03,11)/C#/Lua/Java/JavaScript code with no dependencies

Usage in brief
---------------

* Write a schema file that allows you to define the data structures you may want to serialize. Fields can have a scalar type (ints/floats of all sizes), or they can be a: string; array of any type; reference to yet another object
* Use adatac (the ADATA compiler) to generate a C++ header (or Lua/JavaScript script or Java/C#/ classes) with helper classes to access and construct serialized data. This header (say mydata.adl.h) only depends on adata.h/hpp, which defines the core functionality
* Store or send your buffer somewhere

Build the compiler
------------

To build adatac, we need:
* CMake 2.8 and newer
* Boost 1.55.0 and newer (Headers and Boost.Program Options)
* GCC >= 4.9 or VC >= 12.0

Build Boost.Program Options:
* Please ref Boost's doc - http://www.boost.org/doc/libs/1_57_0/more/getting_started/ 
* Note: please build boost using stage mode.

Build adatac:
* Suppose that adata already unzip to dir adata/
* cd adata
* Make a dir named build
* cd build
* cmake -G "Visual Studio 12 2013"(or on unix-like platform: "Unix MakeFiles") -DBOOST_ROOT=[your_boost_root_dir]
* Build whatever generated
* If success, will get a exe named adatac

Use the compiler
-------------------

adatac [-I PATH] [-O PATH] [-P PATH] [-G language] [--help]
* -I PATH: the adl file to generate, e.g. -I/home/username/work/project/xxx.adl
* -O PATH: the path that output generated files, e.g. -O/home/username/work/project/generated
* -P PATH: include path for other adl, could set more than once, e.g. -P/home/username/work/project/adl -P/home/username/work/project/include
* -G language: which language source to generate, could set more than once, e.g. -Gcpp -Glua(lua51,lua52,lua53,luajit) -Gcsharp -Gjs -Gjava
* --help: show help messages

Writing a schema (.adl file)
-------------------

Let's look at an example first

```cpp

namespace = my.country;

person
{
  string name(30); //this is a comment
  int32 age = 18;
  bool married = false;
}

test
{
  fix_int16 i = 1;
  float32 f = 3.0;
  list<int32> lis;
  map<int32,int64> ms;
  sub_type sub;
}

```

amsg::size_of
-------------------

Using amsg::size_of to get object's serialize size

```cpp
struct person
{
  std::string  name;
  int          age;

  bool operator==(person const& rhs) const
  {
    return name == rhs.name && age == rhs.age;
  }
};

AMSG(person, (name)(age));

person obj;
amsg::error_code_t ec = amsg::success;
std::size_t size = amsg::size_of(obj, ec);
assert(ec == amsg::success);
std::cout << "person's serialize size: " << size << std::endl;
```

smax
-------------------

Sometimes you want limit max size of an array of string:

```cpp
struct person
{
  std::string  name;
  int          age;

  bool operator==(person const& rhs) const
  {
    return name == rhs.name && age == rhs.age;
  }
};

AMSG(person, (name&smax(30))(age)); // smax(30) limit name string max size is 30 bytes
```

sfix
-------------------

Sometimes you want serialization size to be fixed:

```cpp
struct person
{
  std::string    name;
  boost::int32_t age;

  bool operator==(person const& rhs) const
  {
    return name == rhs.name && age == rhs.age;
  }
};

AMSG(person, (name)(age&sfix)); // sfix enforce age (int32_t) to be 4bytes after serialization
```

If no sfix, age will dependence its value:
  -128 to 127                     --> 1byte
  -32,768 to 32,767               --> 2byte
  -2,147,483,648 to 2,147,483,647 --> 4byte

note: sfix only effect built-in types(int, short, long, char, float, double and so on).
