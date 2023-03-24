
using Assets.Script.Core.Math;
using Assets.Script.Core.Math.Vector;
using System;
using System.IO;

namespace Assets.Script.Core.Physics.Objects
{
    class PhysicsSimulationBounds : CollisionBounds
    {
        private IntVector2 _position;
        private AABB _bounds;
        private IntVector2 _size;

        private int _rightWallCollision;
        private int _leftWallCollision;

        public override AABB Bounds => _bounds;

        public override IntVector2 Position => _position;

        public override IntVector2 Size => _size;

        public override IntVector2 DesiredVelocity => default;

        public override IntVector2 Velocity => default;

        public override string Name => nameof(PhysicsSimulationBounds);

        public int RightWallCollision => _rightWallCollision;
        public int LeftWallCollision => _leftWallCollision;

        public PhysicsSimulationBounds(IntVector2 position, IntVector2 size)
        {
            _position = position;
            _size = size;
            _bounds = AABB.Create(position, size);
        }

        public void ClearLeftWallCollision()
        {
            _leftWallCollision = default;
        }


        public void ClearRightWallCollision()
        {
            _rightWallCollision = default;
        }

        public void SetLeftWallCollision(int obj)
        {
            if (_leftWallCollision == default)
            {
                _leftWallCollision = obj;
            }
        }

        public void SetRightWallCollision(int obj)
        {
            if (_rightWallCollision == default)
            {
                _rightWallCollision = obj;
            }
        }
        public override void Serialize(BinaryWriter bw)
        {
            bw.Write(_leftWallCollision);
            bw.Write(_rightWallCollision);
            _bounds.Serialize(bw);
            _position.Serialize(bw);
            _size.Serialize(bw);
        }

        public override void Deserialize(BinaryReader bw)
        {
            _leftWallCollision = bw.ReadInt32();
            _rightWallCollision = bw.ReadInt32();

            Bounds.Deserialize(bw);
            Position.Deserialize(bw);
            Size.Deserialize(bw);
        }

        internal void TryClearCollisionsForId(int id)
        {
            if(_leftWallCollision == id)
            {
                ClearLeftWallCollision();
            }

            if (_rightWallCollision == id)
            {
                ClearRightWallCollision();
            }
        }
    }
}
