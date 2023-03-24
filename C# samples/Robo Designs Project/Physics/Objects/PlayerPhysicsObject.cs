
using Assets.Script.Core.Entities.Data;
using Assets.Script.Core.Math;
using Assets.Script.Core.Logging;
using Assets.Script.Core.Math.Vector;
using System.IO;

namespace Assets.Script.Core.Physics.Objects
{
    public class PlayerPhysicsObject : DynamicPhysicsObject
    {
        public override IntVector2 Size => _bounds.Size;

        // bad, store just an internal ID
        public override Entity2D Entity => _entity;

        private Entity2D _entity;

        public override IntVector2 Velocity => _currentVelocity;
        public override IntVector2 Position => _position;

        public override AABB Bounds => _bounds;

        public override bool IsOnFloor => _onFloorCollision;

        public override bool LeftWallCollision => _leftWallCollision;
        public override bool RightWallCollision => _rightWallCollision;

        internal override bool PositionChangedExternally => _positionChangedExternally;

        // Used for CCD
        private IntVector2 _position;
        private bool _positionChangedExternally;
        private IntVector2 _currentVelocity;

        private IntVector2 _desiredVelocity;

        private IntVector2 _acceleration;

        public override int Friction => _friction;
        public override int Gravity => _gravity;

        public override bool Enabled => _enabled;
        public override CollisionPermissions CollisionType => _collisionType;

        private int _gravity = 60;
        private int _friction = 1000;

        private AABB _bounds;

        private bool _applyGravity;
        private bool _applyFriction;

        private bool _leftWallCollision;
        private bool _rightWallCollision;
        private bool _onFloorCollision;
        private bool _onRoofCollision;

        private bool _skipFrictionThisFrame;

        private bool _enabled = true;

        private CollisionPermissions _collisionType;

        public PlayerPhysicsObject(Entity2D entity, IntVector2 position, IntVector2 size)
        {
            _entity = entity;
            _position = position;
            _bounds = AABB.Create(position, size);
        }
        
        // when velocity is set, we need to reset collisions and compute everything
        public override void SetXVelocity(int vel)
        {
            _desiredVelocity.X = vel;
            _currentVelocity.X = vel;
            _skipFrictionThisFrame = true;
            Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[POS][SetXVelocity] {_entity.ID} - setting velocity to {vel}");
        }

        public override void SetYVelocity(int vel)
        {
            _desiredVelocity.Y = vel;
            _currentVelocity.Y = vel;
        }
        
        // internal update function for ENGINE updates
        internal override void UpdatePosition(IntVector2 position)
        {
            _position = position;
            Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[POS][UpdatePosition] {_entity.ID} - setting position to {position.X}, {position.Y} ");
            UpdateBounds();
        }

        // external update function for GAMEPLAY-level updates
        public override void SetPosition(IntVector2 position)
        {
            _position = position;
            Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[POS][SetPosition] {_entity.ID} - setting position to {position.X}, {position.Y} ");
            _positionChangedExternally = true;
            UpdateBounds();
        }


        public override void SetGravity(int gravity)
        {
            _gravity = gravity;
        }

        public override void SetFriction(int friction)
        {
            _friction = friction;
        }

        internal override void SetCollisions(bool onRoofCollision, bool onFloorCollision, bool leftWallCollision, bool rightWallCollision)
        {
            _onRoofCollision = onRoofCollision;
            _onFloorCollision = onFloorCollision;
            _leftWallCollision = leftWallCollision;
            _rightWallCollision = rightWallCollision;
        }

        internal override void SetLeftCollision(bool leftWallCollision)
        {
            _leftWallCollision = leftWallCollision;
        }

        internal override void SetRightCollision(bool rightWallCollision)
        {
            _rightWallCollision = rightWallCollision;
        }
        
        internal override void SetUpCollision(bool ceilingCollision)
        {
            _onRoofCollision = ceilingCollision;
        }

        internal override void SetBottomCollision(bool floorCollision)
        {
            _onFloorCollision = floorCollision;
        }


        internal override void PreUpdate()
        {
            if (!_enabled)
            {
                return;
            }

            if (!_entity.Active)
            {
                return;
            }

            // resolve collisions before we move
            ResolveCollisions();

            // update accel
            var acceleration = GetAccelerationThisStep(_applyGravity, _applyFriction && !_skipFrictionThisFrame);
            _currentVelocity = _currentVelocity + acceleration;

            // update position
            _position = _position + _currentVelocity;

            UpdateBounds();

            // reset collisions if we've hit 0
            if(_currentVelocity.X != 0)
            {
                _leftWallCollision = false;
                _rightWallCollision = false;
            }
            if(_currentVelocity.Y != 0)
            {
                _onRoofCollision = false;
                _onFloorCollision = false;
            }

            _applyGravity = true;
            _applyFriction = false;

            _skipFrictionThisFrame = false;
            _positionChangedExternally = false;
        }

        private void ResolveCollisions()
        {
            if (_collisionType == CollisionPermissions.None)
            {
                _applyGravity = false;
                _applyFriction = false;
                return;
            }

            if (_onRoofCollision)
            {
                Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[PostUpdate] {_entity.ID} is colliding w/ the ceiling. ");

                // top collision detected, stop moving up
                if (_currentVelocity.Y > 0)
                {
                    _currentVelocity.Y = 0;
                    _acceleration.Y = 0;
                }
            }

            if (_onFloorCollision)
            {
                Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[PostUpdate] {_entity.ID} is colliding w/ the floor. ");

                // bottom collision detected, stop moving down
                if (_currentVelocity.Y <= 0)
                {
                    _currentVelocity.Y = 0;
                    _acceleration.Y = 0;

                    _applyGravity = false;
                    _applyFriction = true;
                }
                else
                {
                    _applyGravity = true;
                    _applyFriction = false;
                }
            }

            if (_leftWallCollision)
            {
                Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[PostUpdate] {_entity.ID} is colliding w/ the left wall. ");

                // bottom collision detected, stop moving down
                if (_currentVelocity.X < 0)
                {
                    _currentVelocity.X = 0;
                    _acceleration.X = 0;
                }
            }

            if (_rightWallCollision)
            {
                Log.Print(LogLevel.Debug3, LogChannels.Physics, $"[PostUpdate] {_entity.ID} is colliding w/ the right wall. ");

                // bottom collision detected, stop moving down
                if (_currentVelocity.X > 0)
                {
                    _currentVelocity.X = 0;
                    _acceleration.X = 0;
                }
            }

            // apply friction until we reach 0
            if (_currentVelocity.X != 0 && _applyGravity == false)
            {
                _applyFriction = true;
            }

            if (!_onFloorCollision)
            {
                _applyFriction = false;
            }
        }


        private IntVector2 GetAccelerationThisStep(bool applyGravity, bool applyFriction)
        {
            IntVector2 vector = IntVector2.zero;

            var resultAccel = _acceleration;

            if (applyGravity)
            {
                vector.Y -= _gravity;
            }

            var absVelocity = System.Math.Abs(_currentVelocity.X);

            if (absVelocity > 0 && applyFriction)
            {
                var frictionValue = System.Math.Min(_friction, absVelocity);

                 vector.X -= frictionValue * System.Math.Sign(_currentVelocity.X);
            }


            return resultAccel + vector;
        }

        private void UpdateBounds()
        {
            _bounds.UpdateBounds(_position);
        }

        public override void SetEnabled(bool v)
        {
            if (v != _enabled)
            {
                _enabled = v;
                // need to reset our known collisions, as we dont know where the object is
                SetCollisions(false, false, false, false);
            }
        }

        public override void SetCollisionType(CollisionPermissions typeToSet)
        {
            // only set if we're changing the permissions
            if (typeToSet != _collisionType)
            {
                _collisionType = typeToSet;
                // need to reset our known collisions, as we dont know where the object is
                SetCollisions(false, false, false, false);
            }
        }

        public override void SetSize(IntVector2 initialSize)
        {
            _bounds = AABB.Create(_position, initialSize);
        }

        public override void Serialize(BinaryWriter bw)
        {
            bw.Write(_enabled);

            _position.Serialize(bw);
            _currentVelocity.Serialize(bw);
            _acceleration.Serialize(bw);

            bw.Write(_friction);
            bw.Write(_gravity);
            bw.Write((int)_collisionType);

            bw.Write(_skipFrictionThisFrame);
            bw.Write(_positionChangedExternally);

            // bools
            bw.Write(_applyGravity);
            bw.Write(_applyFriction);
            bw.Write(_skipFrictionThisFrame);

            // collisions
            bw.Write(_leftWallCollision);
            bw.Write(_rightWallCollision);
            bw.Write(_onFloorCollision);
            bw.Write(_onRoofCollision);

            _bounds.Serialize(bw);
        }


        // TODO: do a pass on enums so they're set to short, INT16
        public override void Deserialize(BinaryReader bw)
        {
            _enabled = bw.ReadBoolean();

            _position.Deserialize(bw);
            _currentVelocity.Deserialize(bw);
            _acceleration.Deserialize(bw);

            _friction = bw.ReadInt32();
            _gravity = bw.ReadInt32();
            _collisionType = (CollisionPermissions) bw.ReadInt32();

            _skipFrictionThisFrame = bw.ReadBoolean();
            _positionChangedExternally = bw.ReadBoolean();

            _applyGravity = bw.ReadBoolean();
            _applyFriction = bw.ReadBoolean();
            _skipFrictionThisFrame = bw.ReadBoolean();

            _leftWallCollision = bw.ReadBoolean();
            _rightWallCollision = bw.ReadBoolean();
            _onFloorCollision = bw.ReadBoolean();
            _onRoofCollision = bw.ReadBoolean();

            _bounds.Deserialize(bw);
        }

    }
}
