ADATA v1.0
=======

ADATA is an efficient cross platform serialization library for C/C++, with support for Lua, C#, JavaScript and Java.
Note: JavaScript and Java not support yet.

Features Overview
---------------

* Lightweight, fast and efficient serialization implementations, specifically for game development and other performance-critical applications
* Flexible, using keyword "delete" to mark discarded fields and a tag to record data length, that means forwards and backwards compatibility
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

```

include = util.vec3;
include = my.game.quest;

namespace = my.game;

item
{
  int64 id;
  int32 type;
  int32 level;
}

player_v1
{
  int32 id; //player unique id
  string name(30); //player name
  int32 age;
  util.vec3 pos;
  list<item> inventory;
  list<my.game.quest> quests; //player's quest list
  float32 factor = 1.0;
}

player_v2
{
  int32 id; //player unique id
  string name(30); //player name
  int32 age [delete];
  util.vec3 pos;
  list<item> inventory;
  list<my.game.quest> quests; //player's quest list
  float32 factor = 1.0 [delete];
  list<int32> friends; //friend id list
}

```

### Data struct

Don't like others, adata's data struct defining has no keyword, just name(here item, player_v1 and player_v2). This is only way of defining objects in adata. Data struct consist of a name (here item, player_v1 and player_v2) and a list of fields. Each field has a type, a name, and optionally a default value.

Data struct is forward and backwards compatibility. User can use **[delete]** on a field to mark it as deprecated as in the example above(player_v2), which will prevent the generation of accessors in the generated code, as a way to enforce the field not being used any more. For compatibility, user can add new fields in the schema **only** at the end of a data struct definition. 

In the example above, player_v1 is an old version and player_v2 is new version, between them, diff are:

* Fields age and factor deprecated.
* New field friends.

User can:

* use player_v2 read player_v1's serialzation data. (forward compatibility)
* use player_v1 read player_v2's serialzation data. (backward compatibility)

if and only if user 1) add new fields only at the end of a data struct definition. 2) remove old fields by mark them [delete] 3) don't change old fields.

### Types

Built-in scalar types are:

* 8 bit: int8 uint8
* 16 bit: int16 uint16
* 32 bit: int32 uint32
* 64 bit: int64 uint64

Built-in non-scalar types:

* string: it's encoding depend on language. 
* list: list<type>, an array of data. Nesting lists is not supported, instead you can wrap the inner list in a data struct.
* map: map<key, value>, an directory of data. Nesting maps is not supported, instead you can wrap the inner map in a data struct.

User can optionally use "(num)" to limit max size of all non-scalar types. User can't change types of fields once they're used.

### Namespaces

These will generate the corresponding namespace in C++/C# for all helper code, and tables in Lua. You can use . to specify nested namespaces/tables.

### Includes

You can include other schemas files in your current one, e.g.:

```
include = file_path.file_name;
```

Actually, include is an adl file, adatac using -P arg to find these adl files for include, e.g.:

* User set -P to "C:\work".
* Under C:\work, there are "code\common" directory, that have one adl file named "test.adl"
* User can write "include = code.common.test" to include test.adl's defination data structs.

### Comments

Write "//" behind a field, don't support single one line.

### Attributes

Attributes may be attached to a declaration, behind a field. These may either have a value or not. Currently, just one: "[delete]".

Use in C++
-------------------

Assuming you have written a schema using the above language in say player.adl, you've generated a C++ header called player.adl.h(rule: [name].adl.h), you can now start using this in your program by including the header. As noted, this header relies on adata/cpp/adata.hpp, which should be in your include path.

Note: generated C++ code support both C++98 and C++11.

### Serialization

First of all, adata::zero_copy_buffer must be created:

```cpp

adata::zero_copy_buffer stream;

```

For serializing data, user need an byte array:

```cpp

#define ENOUGH_SIZE 4096
uint8_t bytes[ENOUGH_SIZE];
int32_t buf_len = ENOUGH_SIZE;

```

If user want know how length an object after serialization, using adata::size_of:

```cpp

// create a player_v1 object
my::game::player_v1 pv1;

// set pv1's value
pv1.id = 152001;
pv1.name = "alex";
pv1.age = 22;

// get the size of serialization(depend on its value, diff value may have diff size)
buf_len = adata::size_of(pv1);

```

Optionally, user can use this length to dyn create byte array:

```cpp

uint8_t* bytes = new uint8_t[len];

```

Then set the bytes to stream:

```cpp

stream.set_write(bytes, buf_len);

```

The last step to serialize is:

```cpp

adata::stream_write(stream, pv1);
if (stream.bad())
{
  // some error, print it
  std::cerr << stream.message() << std::endl;
  std::abort();
}

// serialize success, data already write into bytes

```

### Deserialization

First set read data to stream:

```cpp

// User can copy bytes to other byte array or create other stream whatever.
stream.set_read(bytes, buf_len);

```

Create another player_v1 object, will deserialize data to it

```cpp

my::game::player_v1 pv1_other;

```

Now deserialize:

```cpp

adata::stream_read(stream, pv1_other);
if (stream.bad())
{
  // some error, print it
  std::cerr << stream.message() << std::endl;
  std::abort();
}

// deserialize success, pv1_other should equals with pv1
// omit operator== for player_v1
//namespace my{ namespace game{
//bool operator==(player_v1 const& lhs, player_v1 const& rhs){
//  ...
//}}}

assert(pv1 == pv1_other);

``` 

And size_of pv1_other should also equals with pv1:

```cpp

assert (adata::size_of(pv1) == adata::size_of(pv1_other));

```

### Threading

Either read and write, adata::zero_copy_buffer is not threading-safe. Don't share stream between threads (recommended), or manually wrap it in synchronisation primites.

Creating adata::zero_copy_buffer doesn't using any dyn memory, it is cheap, so feel free to create it. 

Use in Lua
-------------------

Assuming you have written a schema using the above language in say player.adl, you've generated a lua script file called player_adl.lua(rule: [name]_adl.lua), you can now start using this in your program by require it. As noted, this script file relies on adata/lua/adata_core.lua, which should be in your lua path.

Note: adatac support lua four version: lua51 lua52 lua53 luajit(v2.0.3). User need set adatac arg to generate it: adatac -Glua51/lua52/lua53/luajit .

Note: right now adata only support embedde lua in to c/cpp.

### Serialization

First in c/cpp, user need include adata's header and call adata::lua::init_adata_corec: 

```cpp

#include <lua.hpp>

#ifndef ADATA_USE_LUAJIT
# ifdef LUAJIT_VERSION
#   define ADATA_USE_LUAJIT
# endif
#endif

#ifdef ADATA_USE_LUAJIT
# include <adata_jit_corec.hpp>
#else
# include <adata_corec.hpp>
#endif

int main()
{
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  adata::lua::init_adata_corec(L);
  // run script ...
}

```

Then in lua, require adata, player_adl.lua and quest_adl.lua:

```lua

local adata = require("adata_core")
local player_adl = require("player_adl")
local quest_adl = require("quest_adl")

```

And create stream:

```lua

local buf_len = 4096
local stream = adata.new_buf(buf_len)

```

Don't like cpp, in lua, zero_copy_buffer already have a buffer to write when created.

Now create player_v1 object and set its value to serialize:

```lua

-- create a player_v1 object
local pv1 = player_adl.player_v1()

-- set its value
pv1.id = 1
pv1.age = 22
pv1.factor = 2.0
pv1.name = "pv1"

local itm = player_adl.item()
itm.id = 11
itm.level = 321110
itm.type = 3
table.insert(pv1.inventory, itm)
itm = player_adl.item()
itm.id = 12
table.insert(pv1.inventory, itm)

local qst = quest_adl.quest()
qst.id = 50
qst.name = "quest1"
qst.description = "There are something unusual..."
table.insert(pv1.quests, qst)

```

Then serialize:

```lua

local ec = pv1:write(stream)
if ec > 0 then 
  -- some error, print it
  error(ec, adata.trace_info(stream))
end

-- serialize success, data already write into stream's write buffer

```

User can get write data:

```lua

-- data actually is a string
local data = adata.get_write_data(stream)

```

### Deserialization

First set read data to stream:

```lua

adata.set_read_data(stream, data)

```

Create another player_v1 object, will deserialize data to it

```lua

local pv1_other = player_adl.player_v1()

```

Now deserialize:

```lua

local ec = pv1_other:read(stream)
if ec > 0 then 
  -- some error, print it
  error(ec, adata.trace_info(stream))
end

-- deserialize success, pv1_other should equals with pv1

```

Use in CSharp
-------------------

Assuming you have written a schema using the above language in say player.adl, you've generated a C# file called player.adl.cs(rule: [name].adl.cs), you can now start using this in your program by adding the file. As noted, this file relies on adata/csharp/adata.cs, which should be added in your project.

### Serialization

First, create adata.zero_copy_buffer:

```csharp

var bytes = new bytes[4096];
var stream = new adata.zero_copy_buffer(bytes);

```

Now create player_v1 object and set its value to serialize:

```csharp

// create a my.game.player_v1 object
var pv1 = new my.game.player_v1();

// set its value
pv1.id = 152001
pv1.name = "alex"
pv1.age = 22

```

Serialize: 

```csharp

// rule: adata.[adl file name]_stream.stream_write
my.game.player_stream.write(stream, pv1);
if (stream.error())
{
  // some error, print
  System.Console.WriteLine(stream.get_error_msg());
  System.Diagnostics.Debug.Assert(false);
}

// serialize success

```

### Deserialization

C#'s adata.zero_copy_buffer read and write shared same byte array, so this just use stream to read:

```csharp

// create other player_v1 to deserialize
var pv1_other = new my.game.player_v1();

// deserialize
my.game.player_stream.read(stream, ref pv1_other);
if (stream.error())
{
  // some error, print
  System.Console.WriteLine(stream.get_error_msg());
  System.Diagnostics.Debug.Assert(false);
}

// deserialize success

```

pv1's value should equals with pv1_other and size_of also as well.
