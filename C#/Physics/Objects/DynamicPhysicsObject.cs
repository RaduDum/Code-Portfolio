
using Assets.Script.Core.Entities.Data;
using Assets.Script.Core.Math.Vector;
using Assets.Script.Core.Math;
using System.IO;
using Assets.Script.Core.Networking;

namespace Assets.Script.Core.Physics.Objects
{
    public enum CollisionPermissions
    {
        All,
        BoundsOnly,
        None
    }

    public abstract class DynamicPhysicsObject : IPhysicsObject, IOnlineSerializeable
    {
        public abstract IntVector2 Position { get; }
        public abstract IntVector2 Size { get; }
        public abstract IntVector2 Velocity { get; }
        public abstract int Friction { get; }

        public abstract Entity2D Entity { get; }

        public abstract AABB Bounds { get; }

        public abstract bool IsOnFloor { get; }
        public abstract int Gravity { get; }
        public abstract bool LeftWallCollision { get; }
        public abstract bool RightWallCollision { get; }
        internal abstract bool PositionChangedExternally { get; }
        public abstract bool Enabled { get; }
        public abstract CollisionPermissions CollisionType { get; }

        public abstract void SetPosition(IntVector2 position);
        internal abstract void UpdatePosition(IntVector2 position);
        public abstract void SetXVelocity(int x);
        public abstract void SetYVelocity(int y);
        public abstract void SetGravity(int jumpGravity);
        public abstract void SetFriction(int friction);

        public abstract void SetEnabled(bool v);
        public abstract void SetCollisionType(CollisionPermissions v);
        public abstract void SetSize(IntVector2 initialSize);

        internal abstract void SetCollisions(bool onRoofCollision, bool onFloorCollision, bool leftWallCollision, bool rightWallCollision);

        internal abstract void PreUpdate();

        internal abstract void SetLeftCollision(bool v);

        internal abstract void SetRightCollision(bool v);

        internal abstract void SetUpCollision(bool v);

        internal abstract void SetBottomCollision(bool v);

        public abstract void Serialize(BinaryWriter bw);

        public abstract void Deserialize(BinaryReader bw);
    }
}
