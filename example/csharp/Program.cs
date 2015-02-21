using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace example
{
  class Program
  {
    static void Main(string[] args)
    {
      PlayerComparer plyCmp = new PlayerComparer();
      var pv1 = new my.game.player_v1();

      pv1.id = 1;
      pv1.age = 22;
      pv1.factor = 2.0f;
      pv1.name = "pv1";

      var itm = new my.game.item();
      itm.id = 11;
      itm.level = 321110;
      itm.type = 3;
      pv1.inventory.Add(itm);
      itm = new my.game.item();
      itm.id = 12;
      pv1.inventory.Add(itm);

      var qst = new my.game.quest();
      qst.id = 50;
      qst.name = "quest1";
      qst.description = "There are something unusual...";
      pv1.quests.Add(qst);

      var bytes = new byte[4096];
      var stream = new adata.zero_copy_buffer(bytes);
      Int32 buf_len = 0;

      // serialize
      buf_len = my.game.player_stream.size_of(pv1);
      my.game.player_stream.write(stream, pv1);
      Debug.Assert(!stream.error(), stream.get_error_msg());

      var pv1_other = new my.game.player_v1();

      // deserialize
      my.game.player_stream.read(stream, ref pv1_other);
      Debug.Assert(!stream.error(), stream.get_error_msg());
      Debug.Assert(plyCmp.Equals(pv1, pv1_other));

      stream.clear();

      var pv2 = new my.game.player_v2();

      // forward compat (old data, new struct)
      my.game.player_stream.read(stream, ref pv2);
      Debug.Assert(!stream.error(), stream.get_error_msg());

      Debug.Assert(plyCmp.Equals(pv1, pv2));

      stream.clear();
      pv1 = new my.game.player_v1();

      // backward compat (new data, old struct)
      pv2.id = 6543;
      pv2.name = "pv2";
      pv2.friends.Add(2);
      pv2.friends.Add(100);
      buf_len = my.game.player_stream.size_of(pv2);

      my.game.player_stream.write(stream, pv2);
      Debug.Assert(!stream.error(), stream.get_error_msg());

      my.game.player_stream.read(stream, ref pv1);
      Debug.Assert(!stream.error(), stream.get_error_msg());

      Debug.Assert(plyCmp.Equals(pv1, pv2));

      Console.WriteLine("done.");
    }
  }
  class Vec3Comparer : IEqualityComparer<util.vec3>
  {
    public bool Equals(util.vec3 lhs, util.vec3 rhs)
    {
      if (Object.ReferenceEquals(lhs, rhs))
      {
        return true;
      }

      if (Object.ReferenceEquals(lhs, null) || Object.ReferenceEquals(rhs, null))
      {
        return false;
      }

      return
        lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.z == rhs.z
        ;
    }
    public int GetHashCode(util.vec3 itm)
    {
      // Check whether the object is null
      if (Object.ReferenceEquals(itm, null)) return 0;

      return
        itm.x.GetHashCode() ^
        itm.y.GetHashCode() ^
        itm.z.GetHashCode()
        ;
    }
  }
  class ItemComparer : IEqualityComparer<my.game.item>
  {
    public bool Equals(my.game.item lhs, my.game.item rhs)
    {
      if (Object.ReferenceEquals(lhs, rhs))
      {
        return true;
      }

      if (Object.ReferenceEquals(lhs, null) || Object.ReferenceEquals(rhs, null))
      {
        return false;
      }

      return
        lhs.id == rhs.id &&
        lhs.level == rhs.level &&
        lhs.type == rhs.type
        ;
    }
    public int GetHashCode(my.game.item itm)
    {
      // Check whether the object is null
      if (Object.ReferenceEquals(itm, null)) return 0;

      return
        itm.id.GetHashCode() ^
        itm.level.GetHashCode() ^
        itm.type.GetHashCode()
        ;
    }
  }
  class QuestComparer : IEqualityComparer<my.game.quest>
  {
    public bool Equals(my.game.quest lhs, my.game.quest rhs)
    {
      if (Object.ReferenceEquals(lhs, rhs))
      {
        return true;
      }

      if (Object.ReferenceEquals(lhs, null) || Object.ReferenceEquals(rhs, null))
      {
        return false;
      }

      return
        lhs.id == rhs.id &&
        lhs.name == rhs.name &&
        lhs.description == rhs.description
        ;
    }
    public int GetHashCode(my.game.quest qst)
    {
      // Check whether the object is null
      if (Object.ReferenceEquals(qst, null)) return 0;

      return
        qst.id.GetHashCode() ^
        qst.name.GetHashCode() ^
        qst.description.GetHashCode()
        ;
    }
  }
  class PlayerComparer : IEqualityComparer<my.game.player_v1>
  {
    private Vec3Comparer vec3Cmp = new Vec3Comparer();
    private ItemComparer itemCmp = new ItemComparer();
    private QuestComparer questCmp = new QuestComparer();
    public bool Equals(my.game.player_v1 lhs, my.game.player_v1 rhs)
    {
      if (Object.ReferenceEquals(lhs, rhs))
      {
        return true;
      }

      if (Object.ReferenceEquals(lhs, null) || Object.ReferenceEquals(rhs, null))
      {
        return false;
      }

      return
        lhs.id == rhs.id &&
        lhs.age == rhs.age &&
        lhs.factor == rhs.factor &&
        lhs.name == rhs.name &&
        vec3Cmp.Equals(lhs.pos, rhs.pos) &&
        lhs.inventory.SequenceEqual(rhs.inventory, itemCmp) &&
        lhs.quests.SequenceEqual(rhs.quests, questCmp)
        ;
    }
    public bool Equals(my.game.player_v1 lhs, my.game.player_v2 rhs)
    {
      if (Object.ReferenceEquals(lhs, rhs))
      {
        return true;
      }

      if (Object.ReferenceEquals(lhs, null) || Object.ReferenceEquals(rhs, null))
      {
        return false;
      }

      return
        lhs.id == rhs.id &&
        lhs.name == rhs.name &&
        vec3Cmp.Equals(lhs.pos, rhs.pos) &&
        lhs.inventory.SequenceEqual(rhs.inventory, itemCmp) &&
        lhs.quests.SequenceEqual(rhs.quests, questCmp)
        ;
    }
    public int GetHashCode(my.game.player_v1 pv1)
    {
      // Check whether the object is null
      if (Object.ReferenceEquals(pv1, null)) return 0;

      return
        pv1.id.GetHashCode() ^
        pv1.age.GetHashCode() ^
        pv1.factor.GetHashCode() ^
        pv1.name.GetHashCode() ^
        vec3Cmp.GetHashCode(pv1.pos) ^
        pv1.inventory.GetHashCode() ^
        pv1.quests.GetHashCode()
        ;
    }
  }
}
