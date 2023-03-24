
using Assets.Script.Core.Entities.Data;
using Assets.Script.Core.Logging;
using Assets.Script.Core.Math.Vector;
using Assets.Script.Core.Math;
using Assets.Script.Core.Physics.Objects;
using System;
using System.Collections.Generic;
using Assets.Script.Core.Networking;
using System.IO;

namespace Assets.Script.Core.Physics
{
    [Flags]
    public enum PhysicsCollision
    {
        None = 0,
        Down = 1,
        Up = 2,
        Left = 4,
        Right = 8,
    }

    internal struct Pair
    {
        internal DynamicPhysicsObject A;
        internal DynamicPhysicsObject B;

        internal Pair(DynamicPhysicsObject a, DynamicPhysicsObject b)
        {
            A = a;
            B = b;
        }

        public static bool operator ==(Pair obj1, Pair obj2)
        {
            return (obj1.A == obj2.A
                        && obj1.B == obj2.B)
                        || (obj1.A == obj2.B
                        && obj1.B == obj2.A);
        }
        public static bool operator !=(Pair obj1, Pair obj2)
        {
            return (obj1.A != obj2.A
                        || obj1.B != obj2.B)
                        && (obj1.A != obj2.B
                        || obj1.B != obj2.A);
        }
    }

    internal struct BoundaryManifold
    {
        internal CollisionBounds Bounds;
        internal DynamicPhysicsObject Object;
        internal int XPenetration;
        internal int YPenetration;
        internal IntVector2 Normal;
    }

    internal struct ObjectManifold
    {
        internal Pair Pair;
        internal int Penetration;
        internal IntVector2 Normal;
    }

    public class PhysicsSystem : IPhysicsSystem, IOnlineSerializeable
    {
        private List<DynamicPhysicsObject> _bodies = new List<DynamicPhysicsObject>();
        private List<MoveableBounds> _extraBounds = new List<MoveableBounds>();

        private PhysicsSimulationBounds _bounds;

        private const int COLLISION_TIMEOUT = 5000;

        private IntVector2 LEFT_NORMAL = new IntVector2(-1, 0);
        private IntVector2 RIGHT_NORMAL = new IntVector2(1, 0);
        private IntVector2 TOP_NORMAL = new IntVector2(0, 1);
        private IntVector2 BOTTOM_NORMAL = new IntVector2(0, -1);

        private List<ObjectManifold> _manifolds = new List<ObjectManifold>();
        private List<BoundaryManifold> _boundaryManifolds = new List<BoundaryManifold>();
        private BoundsRuntimeData _runtimeData = new BoundsRuntimeData();

        public IPhysicsObject SetSystemBounds(IntVector2 position, IntVector2 size)
        {
            _bounds = new PhysicsSimulationBounds(position, size);

            return _bounds;
        }


        public DynamicPhysicsObject AddObject(Entity2D entity, IntVector2 position, IntVector2 size)
        {
            return AddPlayer(entity, position, size);
        }

        public void Register(Entity2D entity)
        {
            if(entity is IPhysicalEntity physical)
            {
                physical.AssignPhysicalObject(AddObject(entity, physical.InitialPosition, physical.InitialSize));
            }
        }

        public void Unregister(Entity2D entity)
        {
            if (entity is IPhysicalEntity physical)
            {
                _bodies.Remove(physical.Physical);
            }
        }

        public void Update()
        {

            // run actual update once we know all the future positions this frame
            foreach (var obj in _bodies)
            {
                if (obj.Enabled)
                {
                    if(obj.Entity is IHitstopEntity hitstop)
                    {
                        if (hitstop.Hitstopped)
                        {
                            continue;
                        }
                    }

                    if (obj.PositionChangedExternally)
                    {
                        _bounds.TryClearCollisionsForId(obj.Entity.ID);
                        obj.SetCollisions(false, false, false, false);
                    }

                    obj.PreUpdate();
                }
            }

            FormAndResolveCollisionPairs();

            UpdateRuntimeData();

        }

        public BoundsRuntimeData GetBoundsRuntimeData()
        {
            return _runtimeData;
        }

        public MoveableBounds AddBoundaryObject(IntVector2 position, IntVector2 size)
        {
            var extraBounds = new MoveableBounds(position, size);
            _extraBounds.Add(extraBounds);

            return extraBounds;
        }

        public void Serialize(BinaryWriter bw)
        {
            foreach(var physicsObject in _bodies)
            {
                physicsObject.Serialize(bw);
            }

            _bounds.Serialize(bw);

            foreach (var physicsObject in _extraBounds)
            {
                physicsObject.Serialize(bw);
            }
        }

        public void Deserialize(BinaryReader br)
        {
            foreach (var physicsObject in _bodies)
            {
                physicsObject.Deserialize(br);
            }

            _bounds.Deserialize(br);

            foreach (var physicsObject in _extraBounds)
            {
                physicsObject.Deserialize(br);
            }
        }

        private void UpdateRuntimeData()
        {
            _runtimeData.SceneBounds = _bounds.Bounds;

#if DEBUG
            if (_runtimeData.SceneBoundsObject == default)
            {
                _runtimeData.SceneBoundsObject = _bounds;
            }
#endif

            var extraCount = _extraBounds.Count;
            if (extraCount == 0)
            {
                return;
            }

            if (_runtimeData.ExtraBounds == default)
            {
                _runtimeData.ExtraBounds = new AABB[extraCount];
            }


#if DEBUG
            if (_runtimeData.ExtraBoundsObjects == default)
            {
                _runtimeData.ExtraBoundsObjects = new IPhysicsObject[extraCount];
            }
#endif

            if (_runtimeData.ExtraBounds != default)
            {
                for (int i = 0; i < extraCount; i++)
                {
                    _runtimeData.ExtraBounds[i] = _extraBounds[i].Bounds;
#if DEBUG
                    if (_runtimeData.ExtraBoundsObjects[i] == default)
                    {
                        _runtimeData.ExtraBoundsObjects[i] = _extraBounds[i];
                    }
#endif
                }
            }
        }

        private void FormAndResolveCollisionPairs()
        {
            // bounds pass
            PopulateBoundaryPairs();
            ResolveBoundaryCollisions();

            // inter object pass
            PopulateCollisionPairs();
            RemoveDuplicatePairs();
            ResolvePairCollisions();

            // secondary boundary pass, to resolve any fucked up collisions
            PopulateBoundaryPairs();
            ResolveBoundaryCollisions();

            ClearPhysicsBoundsInformation();
        }

        private void ClearPhysicsBoundsInformation()
        {
            if(_bounds.LeftWallCollision != default)
            {
                foreach (var physicsObject in _bodies)
                {
                    if (physicsObject.Entity.ID == _bounds.LeftWallCollision)
                    {
                        if (physicsObject.LeftWallCollision == false && physicsObject.Enabled)
                        {
                            _bounds.ClearLeftWallCollision();
                        }
                    }
                }
            }

            if (_bounds.RightWallCollision != default)
            {
                foreach (var physicsObject in _bodies)
                {
                    if (physicsObject.Entity.ID == _bounds.RightWallCollision) 
                    {
                        if (physicsObject.RightWallCollision == false && physicsObject.Enabled)
                        {
                            _bounds.ClearRightWallCollision();
                        }
                    } 
                } 
            }
        }

        private void PopulateBoundaryPairs()
        {
            _boundaryManifolds.Clear();

            var bodiesCount = _bodies.Count;
            var extraBoundCount = _extraBounds.Count;
            for (int i = 0; i < bodiesCount; i++)
            {
                if (_bodies[i].CollisionType == CollisionPermissions.None)
                {
                    continue;
                }

                // create manifolds for boundaries, if any collisions exist
                BoundaryManifold boundaryManifold = new BoundaryManifold();
                boundaryManifold.Bounds = _bounds;
                boundaryManifold.Object = _bodies[i];

                if (AABBvsAABBbounds(ref boundaryManifold))
                {
                    _boundaryManifolds.Add(boundaryManifold);
                }

                for (int j = 0; j < extraBoundCount; j++)
                {
                    // create manifolds for boundaries, if any collisions exist
                    BoundaryManifold extraBoundsManifold = new BoundaryManifold();
                    extraBoundsManifold.Bounds = _extraBounds[j];
                    extraBoundsManifold.Object = _bodies[i];

                    if (AABBvsAABBbounds(ref extraBoundsManifold))
                    {
                        _boundaryManifolds.Add(extraBoundsManifold);
                    }
                }
            }
        }

        private void PopulateCollisionPairs()
        {
            _manifolds.Clear();

            var bodiesCount = _bodies.Count;
            for (int i = 0; i < bodiesCount; i++)
            {
                for (int j = 0; j < bodiesCount; j++)
                {
                    var bodyA = _bodies[i];
                    var bodyB = _bodies[j];

                    if (bodyA == bodyB)
                    {
                        continue;
                    }

                    if (bodyA.CollisionType != CollisionPermissions.All)
                    {
                        continue;
                    }

                    if (bodyB.CollisionType != CollisionPermissions.All)
                    {
                        continue;
                    }

                    // create a manifold and assign the potential pair before verifying collision
                    ObjectManifold manifold = new ObjectManifold();
                    var potentialPair = new Pair(bodyA, bodyB);
                    manifold.Pair = potentialPair;

                    if (AABBvsAABB(ref manifold))
                    {
                        _manifolds.Add(manifold);
                    }
                }

                // create manifolds for boundaries, if any collisions exist
                BoundaryManifold boundaryManifold = new BoundaryManifold();
                boundaryManifold.Bounds = _bounds;
                boundaryManifold.Object = _bodies[i];

                if(AABBvsAABBbounds(ref boundaryManifold))
                {
                    _boundaryManifolds.Add(boundaryManifold);
                }
            }

        }

        private void RemoveDuplicatePairs()
        {
            var manifoldCount = _manifolds.Count;
            for (int i = 0; i < manifoldCount; i++)
            {
                for (int j = 0; j < manifoldCount; j++)
                {
                    // skip identical names
                    if(i == j)
                    {
                        continue;
                    }

                    // if the pairs match, remove it
                    if(_manifolds[i].Pair == _manifolds[j].Pair)
                    {
                        _manifolds.RemoveAt(j);
                        j--;
                        manifoldCount = _manifolds.Count;
                    }
                }
            }
        }

        private void ResolvePairCollisions()
        {
            var manifoldCount = _manifolds.Count;
            for (int i = 0; i < manifoldCount; i++)
            {
                ResolvePlayerOnPlayerCollision(_manifolds[i]);
            }

        }

        private void ResolvePlayerOnPlayerCollision(ObjectManifold manifold)
        {
            var normalF = new IntVector2(manifold.Normal.X, manifold.Normal.Y);

            var relativeVelocity = manifold.Pair.B.Velocity - manifold.Pair.A.Velocity;
            int velocityAlongNormal = IntVector2.Dot(relativeVelocity, normalF);

            // Do not resolve if velocities are separating
            if (velocityAlongNormal > 0)
            {
                return;
            }

            bool resolveNormally = false;

            // compute various micro collisions
            var aBoundsCollision = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, manifold.Pair.A.Bounds);
            var bBoundsCollision = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, manifold.Pair.B.Bounds);

            var aIsOnGround = CollisionHelpers.CheckCollisionType(aBoundsCollision, PhysicsCollision.Down);
            var bIsOnGround = CollisionHelpers.CheckCollisionType(bBoundsCollision, PhysicsCollision.Down);

            var aIsTouchingAWall = CollisionHelpers.CheckCollisionType(aBoundsCollision, PhysicsCollision.Left)
                || CollisionHelpers.CheckCollisionType(aBoundsCollision, PhysicsCollision.Right);
            var bIsTouchingAWall = CollisionHelpers.CheckCollisionType(bBoundsCollision, PhysicsCollision.Left)
                || CollisionHelpers.CheckCollisionType(bBoundsCollision, PhysicsCollision.Right);

            // detect side collision and resolve
            if (manifold.Normal.X != 0)
            {
                // both on the ground
                if (aIsOnGround
                    && bIsOnGround)
                {
                    Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} both on the ground");

                    // is next to wall
                    if (aIsTouchingAWall
                        || bIsTouchingAWall)
                    {
                        if (aIsTouchingAWall && bIsTouchingAWall)
                        {
                            if (manifold.Pair.A.Entity.ID == _bounds.RightWallCollision)
                            {
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.A, -1);
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.B);
                            }
                            else if (manifold.Pair.B.Entity.ID == _bounds.RightWallCollision)
                            {
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.A);
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.B, -1);
                            }
                            else if (manifold.Pair.A.Entity.ID == _bounds.LeftWallCollision)
                            {
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.A);
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.B, -1);
                            }
                            else if (manifold.Pair.B.Entity.ID == _bounds.LeftWallCollision)
                            {
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.A, -1);
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.B);
                            }
                        }
                        else
                        {
                            Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} one is next to wall");
                            if (aIsTouchingAWall)
                            {
                                // correct B position
                                Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} correct {manifold.Pair.B.Entity.ID} position");
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.B, -1);
                            }
                            else if (bIsTouchingAWall)
                            {
                                // correct A position
                                Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} correct {manifold.Pair.A.Entity.ID} position");
                                PositionalCorrectionAtSideWall(manifold, manifold.Pair.A);
                            }
                        }
                    }
                    else
                    {
                        resolveNormally = true;
                    }
                }
                // one of them is on the ground
                else
                {
                    resolveNormally = true;
                }

            }

            // detect top collision
            if (manifold.Normal.Y != 0)
            {
                Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} top collision");

                var objectLanding = manifold.Pair.A;
                var objectLandedOn = manifold.Pair.B;

                // determine which object to move
                if(manifold.Normal.Y > 0)
                {
                    objectLandedOn = manifold.Pair.A;
                    objectLanding = manifold.Pair.B;
                }

                var offset = GetClosestXOffsetForObjectBasedOnPosition(objectLanding, objectLandedOn);

                // store original landed position
                var objectPosition = objectLandedOn.Position;

                // did not land perfectly on top of each other
                if (offset != 0)
                {
                    // apply offset X
                    objectPosition.X = objectLanding.Position.X + offset;
                }
                // landed perfectly on top of each other
                else
                {
                    var landedOnBounds = objectLandedOn.Bounds;
                    var landingBounds = objectLanding.Bounds;

                    // need to check if we're at a corner
                    var objectLandedOnCollisions = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, landedOnBounds);
                    var objectLandingCollisions = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, landingBounds);

                    var objectAreTouchingLeftWall = CollisionHelpers.CheckCollisionType(objectLandedOnCollisions, PhysicsCollision.Left)
                        && CollisionHelpers.CheckCollisionType(objectLandingCollisions, PhysicsCollision.Left);
                    var objectsAreTouchingRightwall = CollisionHelpers.CheckCollisionType(objectLandedOnCollisions, PhysicsCollision.Right)
                        && CollisionHelpers.CheckCollisionType(objectLandingCollisions, PhysicsCollision.Right);

                    if (objectAreTouchingLeftWall || objectsAreTouchingRightwall)
                    {
                        // offset by the width, cus we're touching a wall
                        offset = (objectLandedOn.Bounds.Size.X + objectLanding.Bounds.Size.X) / 2;
                        if (objectsAreTouchingRightwall)
                        {
                            offset *= -1;
                        }
                    }

                    // objects are on top of each other - need to see which one was here first
                    if (objectAreTouchingLeftWall)
                    {
                        if (objectLandedOn.Entity.ID == _bounds.LeftWallCollision)
                        {
                            // offset
                            objectPosition.X = objectLanding.Position.X + offset;
                            objectPosition.Y = objectLanding.Position.Y;

                            objectLanding.UpdatePosition(objectPosition);
                            objectLanding.SetXVelocity(0);
                            objectLanding.SetLeftCollision(false);
                        }
                        else if (objectLanding.Entity.ID == _bounds.LeftWallCollision)
                        {
                            // offset
                            objectPosition.X = objectLandedOn.Position.X + offset;
                            objectPosition.Y = objectLandedOn.Position.Y;

                            objectLandedOn.UpdatePosition(objectPosition);
                            objectLandedOn.SetXVelocity(0);
                            objectLandedOn.SetLeftCollision(false);
                        }
                    }
                    else if (objectsAreTouchingRightwall)
                    {
                        if (objectLandedOn.Entity.ID == _bounds.RightWallCollision)
                        {
                            // offset
                            objectPosition.X = objectLanding.Position.X + offset;
                            objectPosition.Y = objectLanding.Position.Y;

                            objectLanding.UpdatePosition(objectPosition);
                            objectLanding.SetXVelocity(0);
                            objectLanding.SetRightCollision(false);
                        }
                        else if (objectLanding.Entity.ID == _bounds.RightWallCollision)
                        {
                            // offset
                            objectPosition.X = objectLandedOn.Position.X + offset;
                            objectPosition.Y = objectLandedOn.Position.Y;

                            objectLandedOn.UpdatePosition(objectPosition);
                            objectLandedOn.SetXVelocity(0);
                            objectLandedOn.SetRightCollision(false);
                        }
                    }
                    else
                    {
                        objectPosition.X = objectLanding.Position.X + (objectLandedOn.Bounds.Size.X + objectLanding.Bounds.Size.X) / 2;
                        objectPosition.Y = objectLanding.Position.Y;

                        objectLanding.UpdatePosition(objectPosition);
                    }

                    // exit early for bespoke handling
                    return;
                }

                // verify objectPosition result isn't colliding w/ anything
                // if it isn't, perform the move
                // otherwise, do different move

                var bounds = AABB.Create(objectPosition, objectLandedOn.Size);

                var movedObjectCollisions = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, bounds);

                var movedObjectIsTouchingWall = CollisionHelpers.CheckCollisionType(movedObjectCollisions, PhysicsCollision.Left)
                    || CollisionHelpers.CheckCollisionType(movedObjectCollisions, PhysicsCollision.Right);

                if (movedObjectIsTouchingWall) 
                {
                    objectPosition = objectLanding.Position;

                    // apply offset X
                    objectPosition.X = objectLandedOn.Position.X - offset;

                    objectLanding.UpdatePosition(objectPosition);
                    objectLanding.SetXVelocity(0);
                }
                else
                {
                    objectLandedOn.UpdatePosition(objectPosition);
                    // TODO: INVESTIGATE ME
                    // this has been removed because of a bug when hitting a crossup jump normal
                    // x velocity is set to 0, but this invalidates the pushback
                    // i dont know if we ever needed this velocity set to 0 here, BUT it may be worth investigating to see if we do
                    //objectLandedOn.SetXVelocity(0);
                }

            }

            if (resolveNormally)
            {
                Log.Print(LogLevel.Debug2, LogChannels.Physics, "{nameof(ResolvePlayerOnPlayerCollision)} resolve normally");

                GroundedPositionalCorrection(manifold);

             }
        }

        private void GroundedPositionalCorrection(ObjectManifold manifold)
        {
            var convertedVelocity = manifold.Pair.A.Velocity + manifold.Pair.B.Velocity;
            var penetration = manifold.Penetration;

            IntVector2 correction = new IntVector2(manifold.Normal.X, manifold.Normal.Y);
            correction.X = System.Math.Abs(correction.X) * penetration;
            correction.Y = System.Math.Abs(correction.Y) * penetration;

            var absAX = System.Math.Abs(manifold.Pair.A.Velocity.X);
            var absBX = System.Math.Abs(manifold.Pair.B.Velocity.X);

            // potential situations:
            // 1 : only A moving, speed X > if a is moving, push b by penetration and set b's velocity to whatever a's velocity is 
            // 2 : only b moving, speed X > if b is moving, push a by penetration and set a's velocity to whatever b's velocity is
            // 3 : both a and B moving, same speed X, X > push both by half penetration
            // 4 : both a and B moving, different speed X, Y > push both by half penetration and push slower object by difference 
            // 5 : a and b still // should be pre-resolved

            var convertedVelocitySign = System.Math.Sign(convertedVelocity.X);

            // a is moving
            if (absAX > absBX)
            {
                ResolveObjectPushingOther(manifold.Pair.B, manifold.Pair.A, correction * convertedVelocitySign);
            }
            // b is moving
            else if (absAX < absBX)
            {
                ResolveObjectPushingOther(manifold.Pair.A, manifold.Pair.B, correction * convertedVelocitySign);
            }
            else
            {
                var positionA = manifold.Pair.A.Position;
                var positionB = manifold.Pair.B.Position;

                var aSign = System.Math.Sign(manifold.Pair.A.Velocity.X);
                var bSign = System.Math.Sign(manifold.Pair.B.Velocity.X);

                if(aSign == 0)
                {
                    aSign = System.Math.Sign(positionB.X - positionA.X);
                }
                else if (bSign == 0)
                {
                    aSign = System.Math.Sign(positionA.X - positionB.X);
                }

                positionA.X -= correction.X / 2 * aSign;
                positionB.X -= correction.X / 2 * bSign;

                CorrectPotentialPositionRelativeToCorner(manifold.Pair.A, positionA, out var result);
                manifold.Pair.A.UpdatePosition(result);

                CorrectPotentialPositionRelativeToCorner(manifold.Pair.B, positionB, out result);
                manifold.Pair.B.UpdatePosition(result);
            }

            
        }

        private void ResolveObjectPushingOther(DynamicPhysicsObject pushed, DynamicPhysicsObject pusher, IntVector2 correction)
        {
            var positionA = pushed.Position;
            positionA += correction;

            var aHasSnappedToWall = CorrectPotentialPositionRelativeToCorner(pushed, positionA, out var result);

            pushed.UpdatePosition(result);

            if (!aHasSnappedToWall)
            {
                var positionB = pusher.Position;

                CorrectPotentialPositionRelativeToCorner(pusher, positionB, out result);
                pusher.UpdatePosition(result);
                CorrectObjectsForResistance(pushed, pusher);
            }
            else
            {
                var positionB = pushed.Position;
                positionB.X -= (pusher.Bounds.Size.X + pushed.Bounds.Size.X) / 2 * System.Math.Sign(correction.X);
                positionB.Y = pusher.Position.Y;

                CorrectPotentialPositionRelativeToCorner(pusher, positionB, out result);
                pusher.UpdatePosition(result);
            }
        }

        private void CorrectObjectsForResistance(DynamicPhysicsObject pushed, DynamicPhysicsObject pusher)
        {
            // account for resistance
            var totalXVelocity = pusher.Velocity.X - pushed.Velocity.X;

            // 4th of total velocity should be fine
            // TODO: replace this w/ friction computation - will also remove magic number use in the process
            var resistance = totalXVelocity / 4;

            var positionPushed = pushed.Position;
            positionPushed.X -= resistance;

            // push the object back based on the calculated resistance
            pushed.UpdatePosition(positionPushed);

            var positionPusher = pusher.Position;
            positionPusher.X -= resistance;

            // push the object back based on the calculated resistance
            pusher.UpdatePosition(positionPusher);
        }


        private void ResolveBoundaryCollisions()
        {
            var boundaryManifoldCould = _boundaryManifolds.Count;
            for (int i = 0; i < boundaryManifoldCould; i++)
            {
                var manifold = _boundaryManifolds[i];

                var normalF = new IntVector2(manifold.Normal.X, manifold.Normal.Y);

                var relativeVelocity = manifold.Object.Velocity;
                int velocityAlongNormal = IntVector2.Dot(relativeVelocity, normalF);

                // do not resolve if velocities continous
                if (velocityAlongNormal < 0)
                {
                    continue;
                }

                
                // TODO: only interpret up/down collisions based on simualtion limits
                if (manifold.Normal.Y > 0)
                {
                    manifold.Object.SetUpCollision(true);
                }

                if (manifold.Normal.Y < 0)
                {
                    manifold.Object.SetBottomCollision(true);
                }

                // need to inform the physics bounds that theres an object at the wall
                if (manifold.Bounds is PhysicsSimulationBounds simulation)
                {
                    if (manifold.Normal.X < 0)
                    {
                        manifold.Object.SetLeftCollision(true);
                    }

                    if (manifold.Normal.X > 0)
                    {
                        manifold.Object.SetRightCollision(true);
                    }

                    if (manifold.Normal.X < 0)
                    {
                        simulation.SetLeftWallCollision(manifold.Object.Entity.ID);
                    }
                    if (manifold.Normal.X > 0)
                    {
                        simulation.SetRightWallCollision(manifold.Object.Entity.ID); 
                    }

                }

                BoundaryPositionCorrection(manifold);

                Log.Print(LogLevel.Debug3, LogChannels.Physics, "Boundary- obj position X = {manifold.Object.Position.X}, Y = {manifold.Object.Position.Y} ");
                Log.Print(LogLevel.Debug3, LogChannels.Physics, "Boundary resolution - obj velocity X = {manifold.Object.Velocity.X}, Y = {manifold.Object.Velocity.Y} ");

            }
        }

        private void PositionalCorrectionAtSideWall(ObjectManifold manifold, DynamicPhysicsObject playerObj, int direction = 1)
        {
            IntVector2 correction = new IntVector2(manifold.Normal.X, manifold.Normal.Y);
            correction.X = correction.X * manifold.Penetration * direction;
            correction.Y = correction.Y * manifold.Penetration * direction;

            var position = playerObj.Position;

            playerObj.UpdatePosition(position - correction);

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="player"></param>
        /// <param name="potentialPosition"></param>
        /// <param name="resultPosition"></param>
        /// <returns>whether the position has caused the potential position to snap to the physics space boundaries</returns>
        private bool CorrectPotentialPositionRelativeToCorner(IPhysicsObject player, IntVector2 potentialPosition, out IntVector2 resultPosition)
        {
            resultPosition = potentialPosition;
            var potentialBounds = AABB.Create(potentialPosition, player.Size);
            var collisions = CheckAABBCollisionAgainstSimulationBounds(_bounds.Bounds, potentialBounds);

            var isTouchingLeftWall = CollisionHelpers.CheckCollisionType(collisions, PhysicsCollision.Left);
            var isTouchingRightwall = CollisionHelpers.CheckCollisionType(collisions, PhysicsCollision.Right);

            // revert to old X position
            if (isTouchingLeftWall || isTouchingRightwall)
            {
                int boundsXLimit = 0;
                int direction = 1;

                if (isTouchingLeftWall)
                {
                    boundsXLimit = _bounds.Bounds.Left;
                    direction = 1;

                    if (player is PlayerPhysicsObject physicsObject)
                    {
                        physicsObject.SetLeftCollision(true);
                        _bounds.SetLeftWallCollision(physicsObject.Entity.ID);
                    }
                }
                else if (isTouchingRightwall)
                {
                    boundsXLimit = _bounds.Bounds.Right;
                    direction = -1;

                    if (player is PlayerPhysicsObject physicsObject)
                    {
                        physicsObject.SetRightCollision(true);
                        _bounds.SetRightWallCollision(physicsObject.Entity.ID);
                    }
                }

                int resultXPosition = boundsXLimit + player.Size.X / 2 * direction;
                resultPosition.X = resultXPosition;

                return true;
            }


            return false;
        }

        private void BoundaryPositionCorrection(BoundaryManifold manifold)
        {
            IntVector2 correction = new IntVector2(manifold.XPenetration * manifold.Normal.X, manifold.YPenetration * manifold.Normal.Y);

            manifold.Object.UpdatePosition(manifold.Object.Position - correction);
        }


        private int GetClosestXOffsetForObjectBasedOnPosition(IPhysicsObject mainObject, IPhysicsObject secondaryObject)
        {
            // break me down
            // offset to closest valid side
            int l0 = secondaryObject.Bounds.Size.X / 2;
            var l1 = mainObject.Bounds.Size.X / 2;
            int x0 = secondaryObject.Position.X;
            var x1 = mainObject.Position.X;

            // determine which way we are moving the object?
            var direction = System.Math.Sign(x0 - x1);

            if(direction == 1)
            {
                Log.Print(LogLevel.Debug2, LogChannels.Physics, "Positive direction");
            }
            else if(direction == -1)
            { 
                Log.Print(LogLevel.Debug2, LogChannels.Physics, "negative direction");
            }

            var offset = (l0 + l1) * direction;
            return offset;
        }

        private DynamicPhysicsObject AddPlayer(Entity2D entity, IntVector2 position, IntVector2 size)
        {
            var player = new PlayerPhysicsObject(entity, position, size);

            _bodies.Add(player);

            return player;
        }

        private MoveableBounds AddMoveableBounds(IntVector2 position, IntVector2 size)
        {
            var bounds = new MoveableBounds(position, size);

            _extraBounds.Add(bounds);

            return bounds;
        }

        #region COLLISION DETECTION HELPERS

        private PhysicsCollision CheckAABBCollisionAgainstSimulationBounds(AABB bounds, AABB objectBounds)
        {
            PhysicsCollision result = PhysicsCollision.None;

            if (objectBounds.Top >= bounds.Top)
            {
                result = result | PhysicsCollision.Up;
            }

            if (objectBounds.Bottom <= bounds.Bottom)
            {
                result = result | PhysicsCollision.Down;
            }

            if (objectBounds.Left <= bounds.Left)
            {
                result = result | PhysicsCollision.Left;
            }
            if (objectBounds.Right >= bounds.Right)
            {
                result = result | PhysicsCollision.Right;
            }

            return result;
        }

        private bool AABBvsAABBbounds(ref BoundaryManifold manifold)
        {
            DynamicPhysicsObject obj = manifold.Object;
            CollisionBounds bound = manifold.Bounds;

            if(obj.Enabled == false || obj.CollisionType == CollisionPermissions.None)
            {
                return false;
            }

            // vector from A to B
            IntVector2 normal = obj.Position - bound.Position;

            AABB abox = obj.Bounds;
            AABB bbox = bound.Bounds;

            // calculate half extents along x axis for each object
            int aExtent = (abox.Right - abox.Left) / 2;
            int bExtent = (bbox.Right - bbox.Left) / 2;

            // calculate overlap on x axis
            int xOverlap = aExtent + bExtent - System.Math.Abs(normal.X);

            bool foundCollision = false;

            // SAT = separating Axis Theorem
            // SAT test on x axis
            if (xOverlap <= abox.Size.X)
            {
                if (normal.X < 0)
                {
                    Log.Print(LogLevel.Debug3, LogChannels.Physics, "[AABBvsAABBbounds] {obj.Entity.ID} LeftCollision with Bounds");
                    manifold.Normal.X = LEFT_NORMAL.X;
                }
                else
                {
                    Log.Print(LogLevel.Debug3, LogChannels.Physics, "[AABBvsAABBbounds] {obj.Entity.ID} RightCollision with Bounds");
                    manifold.Normal.X= RIGHT_NORMAL.X;
                }
                manifold.XPenetration = abox.Size.X - xOverlap;
                foundCollision = true;
            }

            aExtent = (abox.Top - abox.Bottom) / 2;
            bExtent = (bbox.Top - bbox.Bottom) / 2;

            int yOverlap = aExtent + bExtent - System.Math.Abs(normal.Y);

            // SAT text on y axis
            if (yOverlap <= abox.Size.Y)
            {
                if (normal.Y < 0)
                {
                    Log.Print(LogLevel.Debug3, LogChannels.Physics, "[AABBvsAABBbounds] {obj.Entity.ID} bottomCollision with Bounds");
                    manifold.Normal.Y = BOTTOM_NORMAL.Y;
                }
                else
                {
                    Log.Print(LogLevel.Debug3, LogChannels.Physics, "[AABBvsAABBbounds] {obj.Entity.ID} topCollision with Bounds");
                    manifold.Normal.Y = TOP_NORMAL.Y;
                }
                manifold.YPenetration = abox.Size.Y - yOverlap;
                foundCollision = true;
            }

            return foundCollision;
        }

        private bool AABBvsAABB(ref ObjectManifold manifold)
        {
            DynamicPhysicsObject a = manifold.Pair.A;
            DynamicPhysicsObject b = manifold.Pair.B;

            if (a.Enabled == false || a.CollisionType != CollisionPermissions.All)
            {
                    return false;
            }

            if (b.Enabled == false || b.CollisionType != CollisionPermissions.All)
            {
                return false;
            }

            // vector from A to B
            IntVector2 normal = b.Position - a.Position;

            AABB abox = a.Bounds;
            AABB bbox = b.Bounds;

            // calculate half extents along x axis for each object
            int aExtent = (abox.Right - abox.Left) / 2;
            int bExtent = (bbox.Right - bbox.Left) / 2;

            // calculate overlap on x axis
            int xOverlap = aExtent + bExtent - System.Math.Abs(normal.X);

            // SAT = separating Axis Theorem
            // SAT test on x axis
            if(xOverlap > 0)
            {
                aExtent = (abox.Top - abox.Bottom) / 2;
                bExtent = (bbox.Top - bbox.Bottom) / 2;

                int yOverlap = aExtent + bExtent - System.Math.Abs(normal.Y);

                // SAT text on y axis
                if(yOverlap > 0)
                {
                    // find out which axis is axis of least penetration
                    if(xOverlap < yOverlap)
                    {
                        if(normal.X < 0)
                        {
                            manifold.Normal = LEFT_NORMAL;
                        }
                        else
                        {
                            manifold.Normal = RIGHT_NORMAL;
                        }
                        manifold.Penetration = xOverlap;
                        return true;
                    }
                    else
                    {
                        if (normal.Y < 0)
                        {
                            manifold.Normal = BOTTOM_NORMAL;
                        }
                        else
                        {
                            manifold.Normal = TOP_NORMAL;
                        }
                        manifold.Penetration = yOverlap;
                        return true;

                    }
                }
            }

            return false;
        }

        private bool GetSimpleCollision(AABB source, AABB target)
        {
            return !(source.Left > target.Right ||
                source.Right < target.Left ||
                source.Bottom > target.Top ||
                source.Top < target.Bottom);
        }

        #endregion
    }
}
