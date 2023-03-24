
using Assets.Script.Core.Math;
using Assets.Script.Core.Math.Vector;
using Assets.Script.Core.Networking;
using System.IO;

namespace Assets.Script.Core.Physics.Objects
{
    public abstract class CollisionBounds : IPhysicsObject , IOnlineSerializeable
    {
        public abstract AABB Bounds { get; }

        public abstract IntVector2 Position {get;}

        public abstract IntVector2 Size { get; }

        public abstract IntVector2 DesiredVelocity { get; }

        public abstract IntVector2 Velocity { get; }
        public abstract string Name { get; }

        public abstract void Serialize(BinaryWriter bw);

        public abstract void Deserialize(BinaryReader bw);

        public int Gravity => 0;

        public int Friction => 0;
    }
}
