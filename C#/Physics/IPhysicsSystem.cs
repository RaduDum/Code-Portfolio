
using Assets.Script.Core.Entities.Data;
using Assets.Script.Core.Math.Vector;
using Assets.Script.Core.Physics.Objects;

namespace Assets.Script.Core.Physics
{

    public interface IPhysicsSystem
    {
        // TODO: pending removal, must be replaced with new Entity2D system
        IPhysicsObject SetSystemBounds(IntVector2 position, IntVector2 size);
        DynamicPhysicsObject AddObject(Entity2D entity, IntVector2 position, IntVector2 size);
        MoveableBounds AddBoundaryObject(IntVector2 position, IntVector2 size);


        void Update();

        // Unity-side rendering
        BoundsRuntimeData GetBoundsRuntimeData();

        // register entities
        void Register(Entity2D entity);
        void Unregister(Entity2D entity);
    }
}
