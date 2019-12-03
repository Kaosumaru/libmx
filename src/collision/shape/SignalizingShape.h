#pragma once
#include "Shape.h"
#include "utils/Signal.h"
#include <map>
namespace MX
{
namespace Collision
{

    using SignalizingShapeCollisionSignal = Signal<void(const Shape::pointer& shape, const ShapeCollision::pointer& collision)>;

    class SignalizingShape : public Shape
    {
    public:
        class ClassCollisionData
        {
        public:
            SignalizingShapeCollisionSignal onCollided;
            SignalizingShapeCollisionSignal onFirstCollided;
        };

        const std::shared_ptr<ClassCollisionData>& with(ClassID<>::type classID)
        {
            auto it = _classIDToSignal.find(classID);
            if (it == _classIDToSignal.end())
            {
                auto info = std::make_shared<ClassCollisionData>();
                auto fit = _classIDToSignal.insert(std::make_pair(classID, info));
                return fit.first->second;
            }
            return it->second;
        }

        template <typename Class>
        const std::shared_ptr<ClassCollisionData>& with()
        {
            auto it = _classIDToSignal.find(ClassID<Class>::id());
            if (it == _classIDToSignal.end())
            {
                auto info = std::make_shared<ClassCollisionData>();
                auto fit = _classIDToSignal.insert(std::make_pair(ClassID<Class>::id(), info));
                return fit.first->second;
            }
            return it->second;
        }

    protected:
        std::map<ClassID<>::type, std::shared_ptr<ClassCollisionData>> _classIDToSignal;
    };

    class SignalizingShapeWrapper : public SignalizingShape
    {
    public:
        SignalizingShapeWrapper()
        {
            _trackCollisions = false;
        }
        SignalizingShapeWrapper(bool trackCollisions)
        {
            _trackCollisions = trackCollisions;
        }

        void OnCollision(const Shape::pointer& shape, const ShapeCollision::pointer& collision) override
        {
            auto it = _classIDToSignal.find(shape->classID());
            if (it != _classIDToSignal.end())
                it->second->onCollided(shape, collision);
            SignalizingShape::OnCollision(shape, collision);
        }

        void OnFirstCollision(const Shape::pointer& shape, const ShapeCollision::pointer& collision) override
        {
            auto it = _classIDToSignal.find(shape->classID());
            if (it != _classIDToSignal.end())
                it->second->onFirstCollided(shape, collision);
            SignalizingShape::OnFirstCollision(shape, collision);
        }
    };

    class SignalizingShapeDelayed : public SignalizingShapeWrapper
    {
    public:
        SignalizingShapeDelayed()
        {
        }
        SignalizingShapeDelayed(bool trackCollisions)
            : SignalizingShapeWrapper(trackCollisions)
        {
        }

        void Moved()
        {
            auto c = center();
            if (distanceBetweenSquared(c, _old_center) >= _offset)
            {
                _old_center = c;
                SignalizingShapeWrapper::Moved();
            }
        }

    protected:
        float _offset = 4.0f;
        glm::vec2 _old_center;
    };

    using SignalizingPointShape = PointShape_Policy<SignalizingShapeWrapper>;
    using SignalizingRectangleShape = RectangleShape_Policy<SignalizingShapeWrapper>;
    using SignalizingCircleShape = CircleShape_Policy<SignalizingShapeWrapper>;
    using SignalizingOrientedRectangleShape = OrientedRectangleShape_Policy<SignalizingShapeWrapper>;
#ifdef WIPPOLYGON
    using SignalizingPolygonShape = PolygonShape_Policy<SignalizingShapeWrapper>;
#endif

    using SignalizingPointShapeDelayed = PointShape_Policy<SignalizingShapeDelayed>;
    using SignalizingRectangleShapeDelayed = RectangleShape_Policy<SignalizingShapeDelayed>;
    using SignalizingCircleShapeDelayed = CircleShape_Policy<SignalizingShapeDelayed>;
#ifdef WIPPOLYGON
    using SignalizingOrientedRectangleShapeDelayed = OrientedRectangleShape_Policy<SignalizingShapeDelayed>;
#endif

}
};
