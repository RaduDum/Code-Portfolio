
using Assets.Script.Core.Math;

namespace Assets.Script.Core.Physics
{
    public static class CollisionHelpers
    {

        public const int INVALID_ID = -2;
        public const int BOUNDS_ID = -1;

        public static bool CheckCollisionType(PhysicsCollision collision, PhysicsCollision expected)
        {
            if ((collision & expected) == expected)
            {
                return true;
            }

            return false;
        }

        public static bool AABBCollisionWithCollisionArea(AABB source, AABB target, out AABB collisionArea)
        {
            collisionArea = source;

            // separating axis pass
            // on Y
            if (source.Top < target.Bottom || source.Bottom > target.Top)
            {
                return false;
            }

            // on X
            if (source.Left > target.Right || source.Right < target.Left)
            {
                return false;
            }

            collisionArea.Left = System.Math.Max(target.Left, source.Left);
            collisionArea.Right = System.Math.Min(target.Right, source.Right);
            collisionArea.Top = System.Math.Min(target.Top, source.Top);
            collisionArea.Bottom = System.Math.Max(target.Bottom, source.Bottom);
            collisionArea.UpdatePosAndSize();

            return true;
        }

        public static bool SimpleAABBCollision(AABB source, AABB target)
        {
            // separating axis pass
            // on Y
            if (source.Top < target.Bottom || source.Bottom > target.Top)
            {
                return false;
            }

            // on X
            if (source.Left > target.Right || source.Right < target.Left)
            {
                return false;
            }


            return true;
        }

    }
}
