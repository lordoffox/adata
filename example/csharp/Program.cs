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
      var pv1 = new my.game.PlayerV1();

      pv1.id = 1;
      pv1.age = 22;
      pv1.factor = 2.0f;
      pv1.name = "pv1";

      var itm = new my.game.Item();
      itm.id = 11;
      itm.level = 321110;
      itm.type = 3;
      pv1.inventory.Add(itm);
      itm = new my.game.Item();
      itm.id = 12;
      pv1.inventory.Add(itm);

      var qst = new my.game.Quest();
      qst.id = 50;
      qst.name = "quest1";
      qst.description = "There are something unusual...";
      pv1.quests.Add(qst);

      var bytes = new byte[4096];
      var stream = new adata.ZeroCopyBuffer(bytes);
      Int32 buf_len = 0;

      // serialize
      buf_len = pv1.SizeOf();
      pv1.Write(stream);

      var pv1_other = new my.game.PlayerV1();

      // deserialize
      pv1_other.Read(stream);
      stream.Clear();

      var pv2 = new my.game.PlayerV2();

      // forward compat (old data, new struct)
      pv2.Read(stream);

      Debug.Assert(plyCmp.Equals(pv1, pv2));

      stream.Clear();
      pv1 = new my.game.PlayerV1();

      // backward compat (new data, old struct)
      pv2.id = 6543;
      pv2.name = "pv2";
      pv2.friends.Add(2);
      pv2.friends.Add(100);
      buf_len = pv2.SizeOf();

      pv2.Write(stream);

      pv1.Read(stream);

      Debug.Assert(plyCmp.Equals(pv1, pv2));

      Console.WriteLine("done.");
    }
  }
  class Vec3Comparer : IEqualityComparer<util.Vec3>
  {
    public bool Equals(util.Vec3 lhs, util.Vec3 rhs)
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
    public int GetHashCode(util.Vec3 itm)
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
  class ItemComparer : IEqualityComparer<my.game.Item>
  {
    public bool Equals(my.game.Item lhs, my.game.Item rhs)
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
    public int GetHashCode(my.game.Item itm)
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
  class QuestComparer : IEqualityComparer<my.game.Quest>
  {
    public bool Equals(my.game.Quest lhs, my.game.Quest rhs)
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
    public int GetHashCode(my.game.Quest qst)
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
  class PlayerComparer : IEqualityComparer<my.game.PlayerV1>
  {
    private Vec3Comparer vec3Cmp = new Vec3Comparer();
    private ItemComparer itemCmp = new ItemComparer();
    private QuestComparer questCmp = new QuestComparer();
    public bool Equals(my.game.PlayerV1 lhs, my.game.PlayerV1 rhs)
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
    public bool Equals(my.game.PlayerV1 lhs, my.game.PlayerV2 rhs)
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
    public int GetHashCode(my.game.PlayerV1 pv1)
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
