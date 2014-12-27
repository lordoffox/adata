adl使用说明

先看个简单的例子，来快速浏览一下全貌

sub_type
{
  string str;
}

test_type
{
  int8             i8 = 1;
  fix_uint32       fu32 = 56;
  string           str;
  list<int32>      int_list;
  map<int16,int32> i16_i32_map;  //这是一个字典类型
  sub_type         sub;
}

adata的优势：
1. C++ head only，不需要编译库，只要一个头文件走遍天下，跨平台(windows/linux/macos)
2. Depend nothing, 没有第三方库的依赖 (除了js依赖long.js包)，干净清爽一整天
3. 跨语言，目前支持C++,lua(luajit),js,C#，日后计划加入java，go的支持（欢迎志愿者加入你自己喜欢的语言扩展）
4. 平滑升级，旧的数据定义能在新的数据定义里正确的读取

基础类型
自适应整形类，此类数据在序列化时会按照值的大小进行调整占用的数据空间，而且小的值类型日后能修改为大的值类型，反之不可。
int8
uint8
int16
uint16
int32
uint32
int64
uint64

定长整形类，此类型的数据在序列化时会占用固定的数据空间，而且不能修改值类型
fix_int8
fix_uint8
fix_int16
fix_uint16
fix_int32
fix_uint32
fix_int64
fix_uint64
以上两种类型可以设置初始值

浮点数类
float32
float64
空间占用同样是定长，也不能修改值类型

字符串
string
字符串类最大能有42亿个字符（2^32），不能存宽字节字符（建议utf8）
字符串类能自己定义长度检测，超过将会序列化失败，该功能可以防止在网络协议中指定极大的字符串长度，来导致服务器出错。

列表
list
列表类可以存储连续的多个类型，包括自定义类型
列表类也能指定长度检测

字典
map
字典类可以存储key/value对离散的多个类型对，value包括自定义类型

自定义类型
自定义类型的描述以类型名称开始，"{}"中定义字段信息
如下面的例子
test_type
{
  fix_int32 i = 1;  //this is a comment 可以添加注释
  int64 i64 = 8;
}

test_type就是类型名称
这个类型定义了两个字段
fix_int32 i = 1;  定长的32位有符号整型，初始值为1，注意字段定义后面可以注释
int64 i64 = 8;    自适应长度的64位有符号整型，推荐用这个类型，如果值小于128，序列化时会被压缩到1字节，所以不用担心空间占用的问题


下面定义一个字符串的例子
test_type
{
  string s(30);
}
string s(30);      定义了一个字符串，长度限制为30

下面定义一个列表的例子
test_type
{
  list<int32>      int_list(50);
  list<string(25)> str_list;
}
list<int32>      int_list;   定义了一个自适应长度32位有符号整数的的列表，列表长度的限制为50
list<string(25)> str_list;   定义了一个长度限制为25的字符串的列表

下面定义一个字典的例子
test_type
{
  map<int32,fix_uint64>      int2int_map(50);
}
map<int32,fix_uint64>      int2int_map(50);  定义了一个自适应长度32位有符号整数的key，定长64位无符号整数的字典，长度限制为50
下面定义一个包含自定义类型的例子
sub_type
{
  int32 i = 1;
}
test_type
{
  sub_type sub;
  int64    i64;
}
先定义了一个sub_type的自定义类型
在定义了一个test_type
sub_type sub;  第一个字段为自定义类型sub_type;

版本升级，有时候你的协议需要增加一个字段，那么没问题，旧的协议序列化的数据可以完整的读取到新的协议的对象中
如果你的协议需要删除一个字段，请不要直接在定义处直接删除，你应该这么做

test_type
{
  int32    i32[delete];
  int64    i64;
}
int32    i32[delete];这个字段将会被忽略，不在被读取，在生成代码中也不会出现占位，新的序列化数据中也不再占用空间

自定义数据目前字段限制为63个，将来的版本会进行扩展

adl文件的开头可以定义包，在C++，C#中会被转化为namespace，在lua和js中会作为一个key作为查找键
报的格式是
name1.name2.name3 ...依次类推

idl生成器
adata_gen.exe -I你的adl文件 -G要产生的语言
-G能连续使用
目前支持的语言有cpp,lua,js,csharp
lua分多个语言版本lua51,lua52,lua53,luajit
例子
先定义个一个test.adl

namespace = test.abc;

sub_type
{
  int32 i = 1;  //this is a comment
  int64 i64 = 8;
}

test
{
  fix_int16 i = 1;
  float32 f = 3.0;
  string s(30);
  list<int32> lis;
  map<int32,int64> ms;
  list<string(25)> ss;
  sub_type sub;   
}

test_delete
{
  fix_int16 i = 1;
  float32 f = 3.0 [delete];
  string s(30);
  list<int32> lis [delete];
  map<int32,int64> ms;
  list<string(25)> ss;
  sub_type sub;
}

将上面的内容存到test.adl文件

adata_gen.exe -Itest.adl -Gcpp
这样就会产生一个
test.adl.h
把它包含到你的工程里面，就可以直接使用了
C++的使用例子
#include "test.adl.h"
#include <fstream>

std::ostream& dump_test(const ::test::abc::test& value, std::ostream& os)
{
  os << "i: " << value.i << std::endl;
  os << "f: " << value.f << std::endl;
  os << "s: " << value.s << std::endl;
  os << "is: [";
  for (auto& p : value.lis)
  {
    os << p << ",";
  }
  os << "]" << std::endl;
  os << "ms: {";
  for (auto& p : value.ms)
  {
    os << p.first << ": " << p.second << " , ";
  }
  os << "}" << std::endl;
  os << "ss: [";
  for (auto& s : value.ss)
  {
    os << s << ",";
  }
  os << "]" << std::endl;
  os << "sub: (i: " << value.sub.i << ",i64: " << value.sub.i64 << ")" << std::endl;
  return os;
}

void test_adata_save_data()
{
  ::test::abc::test value;

  value.i = 12;
  value.f = 3.45f;
  value.s = "abcdefg";
  value.lis.push_back(67);
  value.lis.push_back(89);
  value.ms[1] = 11;
  value.ms[2] = 22;
  value.ms[3] = 33;
  value.ss.push_back("abc");
  value.ss.push_back("def");
  value.ss.push_back("hij");
  value.sub.i = 12345;
  value.sub.i64 = 10007199254740992LL;

  dump_test(value,std::cout);
  std::string wb_str;
  adata::zero_copy_buffer stream;
  adata::pre_write(value, stream);
  adata::adata_write_link_string(stream, wb_str);
  if (adata::adata_stream_write(stream, value))
  {
    std::ofstream os("test.bin");
    os.write(wb_str.data(), wb_str.length());
    os.close();
  }
}

int main(int argc, char * argv[])
{
   test_adata_save_data();
}

这个例子是将一个::test::abc::test类型的对象序列化到一个二进制文件中，你也可以将其发送到网络中
这个二进制文件以后可以用来测试其他语言序列化的结果的正确性
我们可以在lua里面打开这个文件，将他放序列化到一个lua对象里面