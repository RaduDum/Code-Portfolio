using Assets.Script.Core.Math.Vector;
using Assets.Script.Core.Physics.Objects;

namespace Assets.Script.Core.Physics
{
    public interface IPhysicalEntity
    {
        public IntVector2 InitialPosition { get; }
        public IntVector2 InitialSize { get; }
        public void AssignPhysicalObject(DynamicPhysicsObject obj);
        public DynamicPhysicsObject Physical { get; }
    }
}
