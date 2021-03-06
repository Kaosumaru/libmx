#pragma once
#include "Shape_Policy.h"
#include "utils/Signal.h"
#include "utils/StrongList.h"
#include "utils/Vector2.h"

namespace MX
{
namespace Collision
{

    class Area;
    class Shape;

    class ShapeCollision : public shared_ptr_init<ShapeCollision>
    {
    public:
        Signal<void(void)> onCollisionEnd;
    };

    class Shape : public Linkable, public shared_ptr_init<Shape>
    {
        friend class Area;

    public:
        Shape() { }
        Shape(bool trackCollisions);
        virtual ~Shape();

        void Enable();
        void Disable();
        bool enabled();

        bool linked() const;
        void Unlink() override;

        bool intersectsWith(const Shape& shape);
        float distanceTo(const Shape& shape) const;
        float distanceToSquared(const Shape& shape) const;
        const Rectangle& bounds() const;
        ClassID<>::type shapeID() const;
        ClassID<>::type classID() const;
        virtual bool trackCollisions() const;

        virtual void OnCollision(const Shape::pointer& shape, const ShapeCollision::pointer& collision);
        virtual void OnFirstCollision(const Shape::pointer& shape, const ShapeCollision::pointer& collision);
        //default_signal<void(const Shape& shape1, const Shape& shape2)> on_collision;
        virtual glm::vec2 center() const { return _bounds.center(); }

        virtual void DebugDraw() { }

        virtual const Geometry& geometry() const = 0;

        void SetTrackCollisions(bool track) { _trackCollisions = track; }

    protected:
        void SetID(ClassID<>::type id);
        void SetClassID(ClassID<>::type id);

        virtual void Moved(); //inform link about move

        bool boundsIntersectsWith(const Shape& shape);
        Rectangle _bounds;
        ClassID<>::type _shapeID = ClassID<>::id();
        ClassID<>::type _classID = ClassID<>::id();
        bool _enabled = true;
        bool _trackCollisions = false;
        Area* _area = nullptr;
        int _existingCollisons = 0;
    };

    using PointShape = PointShape_Policy<Shape>;
    using RectangleShape = RectangleShape_Policy<Shape>;
    using CircleShape = CircleShape_Policy<Shape>;
    using OrientedRectangleShape = OrientedRectangleShape_Policy<Shape>;

}
};
