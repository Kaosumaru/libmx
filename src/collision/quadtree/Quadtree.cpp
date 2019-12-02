#include "Quadtree.h"
#include "collision/shape/SignalizingShape.h"

using namespace MX;
using namespace Collision;

template <typename T>
class QuadStrongList_Link : public StrongList_Link<T>, public QuadFriend
{
public:
    void unlink()
    {
        QuadFriend::removeFromQuad();
        StrongList_Link<T>::unlink();
    }
};

//TODO deal with corner cases
//TODO remove unused quads

MX::Collision::Quad::Quad()
{
}

void MX::Collision::Quad::Set(const MX::Rectangle& rect, unsigned level, MX::Collision::Quad* parent)
{
    _level = level;
    _rectangle = rect;
    _parentQuad = parent;

    auto x1 = _rectangle.x1, x2 = _rectangle.x2, y1 = _rectangle.y1, y2 = _rectangle.y2;

    auto center = _rectangle.center();

    _quadRectangles[0] = MX::Rectangle(x1, y1, center.x, center.y);
    _quadRectangles[1] = MX::Rectangle(center.x, y1, x2, center.y);

    _quadRectangles[2] = MX::Rectangle(x1, center.y, center.x, y2);
    _quadRectangles[3] = MX::Rectangle(center.x, center.y, x2, y2);
}

void MX::Collision::Quad::onShapeRemoved()
{
    assert(_objectsInChildrens != -666);
    _objectsInChildrens--;
    releaseChildren();
    if (_parentQuad)
        _parentQuad->onShapeRemoved();
}

MX::Collision::Quad* MX::Collision::Quad::AddShape(ClassID<>::type type, const Shape::pointer& shape)
{
    assert(_objectsInChildrens != -666);

    if (_parentQuad && !_rectangle.contains(shape->bounds()))
    {
        _parentQuad->AddShape(type, shape);
        return nullptr;
    }

    _objectsInChildrens++;
    if (shouldDivideQuad())
    {
        auto quad = inWhichQuad(shape->bounds());
        if (quad)
            return quad->AddShape(type, shape);
    }

    //insert shape to this node
    auto* link = new QuadStrongList_Link<WeakList<Shape>::ListType>();
    link->SetQuad(this);
    link->SetType(type);
    _shapes.push_back(shape, link);
    return this;
}

MX::Collision::Quad* MX::Collision::Quad::inWhichQuad(const MX::Rectangle& rect)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (_quadRectangles[i].contains(rect))
            break;
    }

    if (i == 4)
        return nullptr;

    if (!_quads[i])
        _quads[i].reset(createSubQuad(_quadRectangles[i]));
    return _quads[i].get();
    //return -1;
}

void MX::Collision::Quad::releaseChildren()
{
    for (auto& quad : _quads)
        if (quad && quad->_objectsInChildrens == 0)
        {
            quad->releaseChildren();
            removeSubQuad(quad.release());
        }
}

MX::Collision::Quad* MX::Collision::Quad::ShapeMoved(const Shape::pointer& shape, bool native_shape)
{
    releaseChildren();

    auto& shape_bounds = shape->bounds();

    //move to parent
    if (_parentQuad && !_rectangle.contains(shape_bounds))
    {
        assert(_objectsInChildrens != -666);
        assert(_objectsInChildrens != 0);

        auto quad = _parentQuad->ShapeMoved(shape, false);
        _objectsInChildrens--;
        releaseChildren();
        return quad;
    }

    if (shouldDivideQuad())
    {
        auto quad = inWhichQuad(shape_bounds);
        if (quad)
        {
            //it fit into children quad, so we increment it's _objectsInChildrens
            assert(quad->_objectsInChildrens != -666);
            quad->_objectsInChildrens++;

            auto final_quad = quad->ShapeMoved(shape, false);
            releaseChildren();
            return final_quad;
        }

        //it doesn't fit into any of children quads, but it fit into us
    }

    if (!native_shape)
    {
        auto link = dynamic_cast<QuadStrongList_Link<WeakList<Shape>::ListType>*>(shape->link());
        link->SetQuad(0);
        _shapes.move_back(shape);
        link->SetQuad(this);
    }
    releaseChildren();
    return this;
}

MX::Collision::Quad* MX::Collision::Quad::createSubQuad(const MX::Rectangle& rect)
{
    MX::Collision::Quad* quad;
    if (!__cachedQuads.empty())
    {
        quad = __cachedQuads.front();
        assert(quad->_objectsInChildrens == -666);
        quad->_objectsInChildrens = 0;
        __cachedQuads.pop_front();
    }
    else
        quad = new Quad();
    quad->Set(rect, _level + 1, this);
    return quad;
}

void MX::Collision::Quad::removeSubQuad(Quad* quad)
{
    assert(quad->_objectsInChildrens == 0);
    quad->_objectsInChildrens = -666;
    __cachedQuads.push_back(quad);
}

std::list<MX::Collision::Quad*> MX::Collision::Quad::__cachedQuads;

void MX::Collision::Quad::EnumCollidingShapes(const Shape::pointer& shape, const EnumCollidingShapes_Function& function)
{
    EnumCollidingShapesInThis(shape, function);
    for (int i = 0; i < 4; i++)
        if (_quads[i] && shape->bounds().intersects(_quadRectangles[i]))
            _quads[i]->EnumCollidingShapes(shape, function);
}

void MX::Collision::Quad::EnumCollidingShapesInThis(const Shape::pointer& shape, const EnumCollidingShapes_Function& function)
{
    auto guard = _shapes.guard();
    auto it = _shapes.begin();
    while (it != _shapes.end())
    {
        Shape::pointer other_shape = it->lock();
        if (!other_shape || shape == other_shape)
        {
            _shapes.temp_advance(it);
            continue;
        }
        function(other_shape);
        _shapes.temp_advance(it);
    }
}

bool MX::Collision::Quad::EnumCollidingShapesCancellable(const Shape::pointer& shape, const EnumCollidingShapes_FunctionCancellable& function)
{
    auto guard = _shapes.guard();
    auto it = _shapes.begin();
    while (it != _shapes.end())
    {
        Shape::pointer other_shape = it->lock();
        if (!other_shape || shape == other_shape)
        {
            _shapes.temp_advance(it);
            continue;
        }
        if (!function(other_shape))
            return false;
        _shapes.temp_advance(it);
    }

    for (int i = 0; i < 4; i++)
        if (_quads[i] && shape->bounds().intersects(_quadRectangles[i]))
            if (!_quads[i]->EnumCollidingShapesCancellable(shape, function))
                return false;
    return true;
}

void MX::Collision::Quad::EnumAllShapes(const EnumCollidingShapes_Function& function)
{
    auto guard = _shapes.guard();
    auto it = _shapes.begin();
    while (it != _shapes.end())
    {
        Shape::pointer other_shape = it->lock();
        if (!other_shape)
            continue;
        function(other_shape);
        _shapes.temp_advance(it);
    }

    for (int i = 0; i < 4; i++)
        if (_quads[i])
            _quads[i]->EnumAllShapes(function);
}

Quadtree::Quadtree()
{
    _quad.SetParentTree(this);
    _excludingQuad.SetParentTree(this);
    _excludingQuad.SetChildQuad(&_quad);
}

Quadtree::~Quadtree()
{
    auto unlinkShapes = [&](const Shape::pointer& shape) {
        shape->Unlink();
    };

    EnumAllShapes(unlinkShapes);
    _excludingQuad.EnumAllShapes(unlinkShapes);
}

void Quadtree::SetExclusionOutsideBounds(bool exclude)
{
    _excludeShapesOutsideBounds = exclude;
    if (_excludeShapesOutsideBounds)
        _quad.SetParent(&_excludingQuad);
    else
        _quad.SetParent(nullptr);
}

bool Quadtree::shouldExclude(const Shape::pointer& shape, MX::Collision::Quad* quad)
{
    //check if exclusion is enabled and if shape belongs to outermost quad
    if (quad == nullptr)
        return true;
    if (quad == &_excludingQuad)
        return true;
    return false;
}

bool Quadtree::AddShape(ClassID<>::type type, const Shape::pointer& shape)
{
    auto quad = _quad.AddShape(type, shape);
    return !shouldExclude(shape, quad);
}

bool Quadtree::ShapeMoved(const Shape::pointer& shape)
{
    auto link = dynamic_cast<QuadStrongList_Link<WeakList<Shape>::ListType>*>(shape->link());
    if (!link)
    {
        assert(false);
        return false;
    }
    auto quad = link->quad();
    quad = quad->ShapeMoved(shape);
    return !shouldExclude(shape, quad);
}

void Quadtree::EnumCollidingShapes(const Shape::pointer& shape, const Quadtree::EnumCollidingShapes_Function& function)
{
    //TODO we can optimize it a bit, if this is a quadtree to which shape belongs
    _quad.EnumCollidingShapes(shape, function);
}

void Quadtree::EnumCollidingShapesCancellable(const Shape::pointer& shape, const EnumCollidingShapes_FunctionCancellable& function)
{
    //TODO we can optimize it a bit, if this is a quadtree to which shape belongs
    _quad.EnumCollidingShapesCancellable(shape, function);
}

void Quadtree::EnumAllShapes(const Quadtree::EnumCollidingShapes_Function& function)
{
    //TODO we can optimize it a bit, if this is a quadtree to which shape belongs
    _quad.EnumAllShapes(function);
}

//TODO optimize further
Shape::pointer Quadtree::ClosestNeighbor(const glm::vec2& point, float max_range, const LayeredArea::MatchingFunction& acceptableShapes)
{
    //should be infinity
    float radius = max_range == -1 ? 10000 : max_range;
    auto circle = std::make_shared<Collision::SignalizingCircleShape>();
    circle->Set(point, radius);

    Shape::pointer retShape;
    float currentDistanceSquared = radius * radius;

    //search main quad
    {
        auto function = [&](const Shape::pointer& shape) {
            if (shape == retShape)
                return;

            if (acceptableShapes && !acceptableShapes(shape))
                return;

            float distanceSquared = circle->distanceToSquared(*shape);
            if (distanceSquared < currentDistanceSquared)
            {
                currentDistanceSquared = distanceSquared;
                retShape = shape;
                return;
            }
        };
        _quad.EnumCollidingShapesInThis(circle, function);
    }

    //search subnodes...
    bool found_potential_match = true;
    int shapes_to_consider_before_changing_shape = 5;

    auto function = [&](const Shape::pointer& shape) {
        if (shape == retShape)
            return true;

        if (acceptableShapes && !acceptableShapes(shape))
            return true;

        float distanceSquared = circle->distanceToSquared(*shape);
        if (distanceSquared < currentDistanceSquared)
        {
            currentDistanceSquared = distanceSquared;
            retShape = shape;
            found_potential_match = true;
        }

        //if we found potential match during search, accept few more shapes and adjust sweeping circle radius
        if (found_potential_match)
        {
            shapes_to_consider_before_changing_shape--;
            if (shapes_to_consider_before_changing_shape == 0)
                return false;
        }

        return true;
    };

    while (found_potential_match)
    {
        shapes_to_consider_before_changing_shape = 5;
        found_potential_match = false;

        radius = sqrt(currentDistanceSquared);
        circle->Set(point, radius);

        for (int i = 0; i < 4; i++)
            if (_quad._quads[i] && circle->bounds().intersects(_quad._quadRectangles[i]))
                if (!_quad._quads[i]->EnumCollidingShapesCancellable(circle, function))
                    break;

        if (!found_potential_match)
            break;
    }

    return retShape;
}

QuadtreeWeakLayeredArea::QuadtreeWeakLayeredArea(const MX::Rectangle& rect, bool excludeShapesOutsideBounds)
    : _rectangle(rect)
    , _excludeShapesOutsideBounds(excludeShapesOutsideBounds)
{
}

QuadtreeWeakLayeredArea::QuadtreeWeakLayeredArea(float width, float height, bool excludeShapesOutsideBounds)
    : _rectangle(0, 0, width, height)
    , _excludeShapesOutsideBounds(excludeShapesOutsideBounds)
{
}

QuadtreeWeakLayeredArea::~QuadtreeWeakLayeredArea()
{
}

std::shared_ptr<QuadtreeWeakLayeredArea::Layer>& QuadtreeWeakLayeredArea::layer(ClassID<>::type layerType)
{
    auto& home_layer = _layers[layerType];
    if (!home_layer)
        home_layer = std::make_shared<Layer>(_rectangle, _excludeShapesOutsideBounds);
    return home_layer;
}

void QuadtreeWeakLayeredArea::DefineLayerCollision(ClassID<>::type layerType1, ClassID<>::type layerType2)
{
    auto layer1 = layer(layerType1);
    auto layer2 = layer(layerType2);

    layer1->collidingLayers.push_back(layer2.get());
    layer2->collidingLayers.push_back(layer1.get());
}

void QuadtreeWeakLayeredArea::TestForeignShape(ClassID<>::type type, const Shape::pointer& shape)
{
    auto home_layer = layer(type);
    auto& layers = home_layer->collidingLayers;
    for (auto& layer : layers)
        layer->tree.EnumCollidingShapes(shape, [&](const Shape::pointer& other_shape) {
            ProcessCollision(shape, other_shape, false);
        });
}

void QuadtreeWeakLayeredArea::AddShape(ClassID<>::type type, const Shape::pointer& shape)
{
    auto home_layer = layer(type);
    auto& layers = home_layer->collidingLayers;
    auto& tree = home_layer->tree;
    LinkShape(&*shape);

    if (!tree.AddShape(type, shape))
        return;

    for (auto& layer : layers)
        layer->tree.EnumCollidingShapesCancellable(shape, [&](const Shape::pointer& other_shape) {
            ProcessCollision(shape, other_shape, true);
            return shape->linked();
        });
}

void QuadtreeWeakLayeredArea::ShapeMoved(const Shape::pointer& shape)
{
    auto type = shape->link()->type();
    auto& home_layer = _layers[type];
    auto& layers = home_layer->collidingLayers;

    if (!home_layer->tree.ShapeMoved(shape))
        return;
    for (auto& layer : layers)
        layer->tree.EnumCollidingShapesCancellable(shape, [&](const Shape::pointer& other_shape) {
            ProcessCollision(shape, other_shape, true);
            return shape->linked();
        });

    ValidateExistingCollisions(shape.get());
}

unsigned QuadtreeWeakLayeredArea::shapes_count()
{
    //TODO optimize
    unsigned count = 0; /*
	for (auto &layer : _layers)
		count += layer.second.shapes.size();*/
    return count;
}

Shape::pointer QuadtreeWeakLayeredArea::ClosestNeighbor(ClassID<>::type layerType, const glm::vec2& point, float max_range, const MatchingFunction& acceptableShapes)
{
    auto home_layer = layer(layerType);
    auto& tree = home_layer->tree;
    return tree.ClosestNeighbor(point, max_range, acceptableShapes);
}