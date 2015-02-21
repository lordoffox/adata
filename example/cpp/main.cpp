///
/// Copyright (c) 2014-2015 Ning Ding (lordoffox@gmail.com)
/// Copyright (c) 2015 Nous Xiong (348944179@qq.com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/lordoffox/adata for latest version.
///

#include <my/game/player.adl.h>
#include <iostream>
#include <string>
#include <cassert>

namespace util
{
  inline bool operator==(vec3 const& lhs, vec3 const& rhs)
  {
    return
      lhs.x == rhs.x &&
      lhs.y == rhs.y &&
      lhs.z == rhs.z
      ;
  }
}

namespace my
{
  namespace game
  {
    inline bool operator==(item const& lhs, item const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.level == rhs.level &&
        lhs.id == rhs.id
        ;
    }

    inline bool operator==(quest const& lhs, quest const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.name == rhs.name &&
        lhs.description == rhs.description
        ;
    }

    inline bool operator==(player_v1 const& lhs, player_v1 const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.age == rhs.age &&
        lhs.factor == rhs.factor &&
        lhs.pos == rhs.pos &&
        lhs.inventory == rhs.inventory &&
        lhs.quests == rhs.quests
        ;
    }

    inline bool operator==(player_v1 const& lhs, player_v2 const& rhs)
    {
      return
        lhs.id == rhs.id &&
        lhs.pos == rhs.pos &&
        lhs.inventory == rhs.inventory &&
        lhs.quests == rhs.quests
        ;
    }
  }
}

int main(int argc, char* argv[])
{
  try
  {
    char buff[4096];

    my::game::player_v1 pv1;

    pv1.id = 1;
    pv1.age = 22;
    pv1.factor = 2.0f;
    pv1.name = "pv1";

    my::game::item itm;
    itm.id = 11;
    itm.level = 321110;
    itm.type = 3;
    pv1.inventory.push_back(itm);
    itm.id = 12;
    pv1.inventory.push_back(itm);

    my::game::quest qst;
    qst.id = 50;
    qst.name = "quest1";
    qst.description = "There are something unusual...";
    pv1.quests.push_back(qst);

    adata::zero_copy_buffer stream;
    int32_t buf_len = 0;

    // serialize
    buf_len = adata::size_of(pv1);
    stream.set_write(buff, buf_len);
    adata::write(stream, pv1);
    assert(!stream.bad());

    my::game::player_v1 pv1_other;

    // deserialize
    stream.set_read(buff, buf_len);
    adata::read(stream, pv1_other);
    assert(!stream.bad());
    assert(pv1 == pv1_other);

    stream.clear();

    my::game::player_v2 pv2;

    // forward compat (old data, new struct)
    adata::read(stream, pv2);
    assert(!stream.bad());

    assert(pv1 == pv2);

    stream.clear();
    pv1 = my::game::player_v1();

    // backward compat (new data, old struct)
    pv2.id = 6543;
    pv2.name = "pv2";
    pv2.friends.push_back(2);
    pv2.friends.push_back(100);
    buf_len = adata::size_of(pv2);

    stream.set_write(buff, buf_len);
    adata::write(stream, pv2);
    assert(!stream.bad());

    stream.set_read(buff, buf_len);
    adata::read(stream, pv1);
    assert(!stream.bad());

    assert(pv1 == pv2);

    std::cout << "done." << std::endl;
  }
  catch (std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}

