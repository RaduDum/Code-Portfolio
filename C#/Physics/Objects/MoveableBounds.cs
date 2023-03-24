
using Assets.Script.Core.Math;
using Assets.Script.Core.Math.Vector;
using System.IO;

namespace Assets.Script.Core.Physics.Objects
{
    public class MoveableBounds : CollisionBounds
    {
        private IntVector2 _position;
        private AABB _bounds;
        private IntVector2 _size;

        public override AABB Bounds => _bounds;

        public override IntVector2 Position => _position;

        public override IntVector2 Size => _size;

        public override IntVector2 DesiredVelocity => default;

        public override IntVector2 Velocity => default;

        public override string Name => nameof(MoveableBounds);

        public MoveableBounds(IntVector2 position, IntVector2 size)
        {
            _position = position;
            _size = size;
            _bounds = AABB.Create(position, size);
        }

        public void SetPosition(IntVector2 position)
        {
            _position = position;
        }

        public void SetSize(IntVector2 size)
        {
            _size = size;
        }


        public void SetLeftLimit(int leftLimit)
        {
            _bounds.Left = leftLimit;
        }

        public void SetRightLimit(int rightLimit)
        {
            _bounds.Right = rightLimit;
        }


        public void Update()
        {
            _bounds = AABB.Create(_position, _size);
        }

        public override void Serialize(BinaryWriter bw)
        {
            _bounds.Serialize(bw);
            _position.Serialize(bw);
            _size.Serialize(bw);
        }

        public override void Deserialize(BinaryReader bw)
        {
            Bounds.Deserialize(bw);
            Position.Deserialize(bw);
            Size.Deserialize(bw);
        }
    }
}
