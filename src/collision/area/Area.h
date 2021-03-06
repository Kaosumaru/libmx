#pragma once
#include "utils/Bimap.h"
#include "utils/StrongList.h"
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include <functional>
#include <list>
#include <map>
#include <set>

namespace MX
{
namespace Collision
{

    class Shape;
    class ShapeCollision;

    class Area : public shared_ptr_init<Area>
    {
    public:
        friend class Shape;

        using ShapeCollision_bimap = Bimap<Shape*, Shape*, std::shared_ptr<ShapeCollision>>;

        friend class AreaLink;
        Area();
        virtual ~Area();

        virtual void TestForeignShape(const std::shared_ptr<Shape>& shape) = 0;
        virtual void AddShape(const std::shared_ptr<Shape>& shape) = 0;

        void ProcessCollisions(const std::shared_ptr<Shape>& shape, StrongList<Shape>& shapes, bool shapeIsInList);
        void ProcessCollisions(const std::shared_ptr<Shape>& shape, WeakList<Shape>& shapes, bool shapeIsInList);
        void ProcessCollision(const std::shared_ptr<Shape>& shape, const std::shared_ptr<Shape>& other_shape, bool shapeIsInList);

        virtual void for_each(const std::function<void(const std::shared_ptr<Shape>&)>& functor) {};
        virtual void DebugDraw();

    protected:
        void ValidateExistingCollisions(Shape* shape, bool unlink = false);
        void LinkShape(Shape* shape);
        void UnlinkingShape(Shape* shape);

        virtual void ShapeMoved(const std::shared_ptr<Shape>& shape) = 0;

        ShapeCollision_bimap _existingCollisions;
    };

    class SimplestArea : public Area
    {
    public:
        ~SimplestArea();

        void TestForeignShape(const std::shared_ptr<Shape>& shape) override;
        void AddShape(const std::shared_ptr<Shape>& shape) override;

    protected:
        void ShapeMoved(const std::shared_ptr<Shape>& shape) override;
        StrongList<Shape> _shapes;
    };

    class SimplestWeakArea : public Area
    {
    public:
        ~SimplestWeakArea();

        void TestForeignShape(const std::shared_ptr<Shape>& shape) override;
        void AddShape(const std::shared_ptr<Shape>& shape) override;

    protected:
        void ShapeMoved(const std::shared_ptr<Shape>& shape) override;
        WeakList<Shape> _shapes;
    };

    class LayeredArea : public Area
    {
    public:
        using MatchingFunction = std::function<bool(const std::shared_ptr<Shape>& shape)>;

        virtual void DefineLayerCollision(ClassID<>::type layer1, ClassID<>::type layer2) = 0;
        virtual void TestForeignShape(ClassID<>::type type, const std::shared_ptr<Shape>& shape) = 0;
        virtual void AddShape(ClassID<>::type type, const std::shared_ptr<Shape>& shape) = 0;
        virtual std::shared_ptr<Shape> ClosestNeighbor(ClassID<>::type layer, const glm::vec2& point, float max_range = -1, const MatchingFunction& acceptableShapes = nullptr) { return nullptr; }

    protected:
        void TestForeignShape(const std::shared_ptr<Shape>& shape) {};
        void AddShape(const std::shared_ptr<Shape>& shape) {};
    };

    class SimplestWeakLayeredArea : public LayeredArea
    {
    public:
        ~SimplestWeakLayeredArea();

        struct Layer;
        typedef std::map<ClassID<>::type, Layer> Layers;
        struct Layer
        {
            std::list<Layer*> collidingLayers;
            WeakList<Shape> shapes;
        };

        std::size_t shapes_count();
        void DefineLayerCollision(ClassID<>::type layer1, ClassID<>::type layer2) override;
        void TestForeignShape(ClassID<>::type type, const std::shared_ptr<Shape>& shape) override;
        void AddShape(ClassID<>::type type, const std::shared_ptr<Shape>& shape) override;

    protected:
        void ShapeMoved(const std::shared_ptr<Shape>& shape) override;

        Layers _layers;
    };

}
};
