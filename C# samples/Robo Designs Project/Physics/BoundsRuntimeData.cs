using Assets.Script.Core.Math;
using Assets.Script.Core.Physics.Objects;

namespace Assets.Script.Core.Physics
{
    public struct BoundsRuntimeData
    {
        public AABB SceneBounds;

        public AABB[] ExtraBounds;

#if DEBUG
        public IPhysicsObject SceneBoundsObject { get; set; }
        public IPhysicsObject[] ExtraBoundsObjects { get; set; }
#endif 
    }
}
