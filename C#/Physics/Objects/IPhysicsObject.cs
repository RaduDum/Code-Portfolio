
using Assets.Script.Core.Math;
using Assets.Script.Core.Math.Vector;
using System.IO;

namespace Assets.Script.Core.Physics.Objects
{
    public interface IPhysicsObject
    {
        public IntVector2 Position { get; }
        public IntVector2 Size { get; }
        public IntVector2 Velocity { get; }

        public AABB Bounds { get; }
        public int Gravity { get; }
        public int Friction { get; }

    }
}
