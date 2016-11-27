#pragma once
#include<functional>
#include "collision/Area/Area.h"
#include "utils/shapes/Rectangle.h"
#include "utils/StrongList.h"

namespace MX
{
namespace Collision
{

class Quadtree;
class Shape;



class Quad
{
public:
	using EnumCollidingShapes_Function = std::function<void (const std::shared_ptr<Shape> &other_shape)>;
	using EnumCollidingShapes_FunctionCancellable = std::function<bool (const std::shared_ptr<Shape> &other_shape)>;
	friend class Quadtree;
	friend class QuadFriend;

	Quad();
	void Set(const MX::Rectangle &rect, unsigned level, Quad *parent);

	Quad* upper_left_quad() {return _quads[0].get();}
	Quad* upper_right_quad() {return _quads[1].get();}
	Quad* lower_left_quad() {return _quads[2].get();}
	Quad* lower_right_quad() {return _quads[3].get();}
	
	WeakList<Shape> &list() {return _shapes;}

	Quad* AddShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape);

	unsigned shapes_count() { return _objectsInChildrens; }
	bool empty() { return shapes_count() == 0 && !_quads[0] && !_quads[1] && !_quads[2] && !_quads[3]; }

	void EnumCollidingShapes(const std::shared_ptr<Shape> &shape, const EnumCollidingShapes_Function& function);
	bool EnumCollidingShapesCancellable(const std::shared_ptr<Shape> &shape, const EnumCollidingShapes_FunctionCancellable& function);
	void EnumAllShapes( const EnumCollidingShapes_Function& function);


	void EnumCollidingShapesInThis(const std::shared_ptr<Shape> &shape, const EnumCollidingShapes_Function& function);

	virtual bool contains(const Rectangle& rect) { return _rectangle.contains(rect); }
	void SetParent(Quad *parent) { _parentQuad = parent; }
protected:
    void releaseChildren();
	virtual void onShapeRemoved();
	void SetParentTree(Quadtree* tree) { _tree = tree; }

	//TODO list().size() = inefficient
	virtual bool shouldDivideQuad() { return list().size() >= maxShapes && _level <= maxLevel; }
	virtual Quad* inWhichQuad(const MX::Rectangle &rect);
	virtual Quad* ShapeMoved(const std::shared_ptr<Shape> &shape, bool native_shape = true);

	Quad* createSubQuad(const MX::Rectangle& rect);
	void removeSubQuad(Quad* quad);


	int _level = 0;
	Quadtree             *_tree = nullptr;
    WeakList<Shape>       _shapes;
	Quad                 *_parentQuad = nullptr;
	std::unique_ptr<Quad> _quads[4];
	Rectangle               _rectangle;
	Rectangle               _quadRectangles[4];
	int                  _objectsInChildrens = 0;



	static const unsigned maxShapes = 10;
	static const unsigned maxLevel = 7;

	static std::list<Quad*> __cachedQuads;
	//Rectangle;
};

class QuadFriend
{
public:
	QuadFriend() {  }

	void SetQuad(Quad *quad)
	{
		_quad = quad;
	}

	Quad *quad() { return _quad; }

	void removeFromQuad()
	{
		if (_quad)
        {
			_quad->onShapeRemoved();
            _quad = nullptr;
        }
	}
	
protected:
	Quad *_quad = nullptr;
};


class ExclusionQuad : public Quad
{
public:
	void SetChildQuad(Quad* quad) { _childQuad = quad;  }

	void onShapeRemoved() {}
	bool shouldDivideQuad() { return true; }
	bool contains(const Rectangle& rect) { return true; }
	Quad* inWhichQuad(const MX::Rectangle &rect)
	{
		if (_childQuad->contains(rect))
			return _childQuad;
		return nullptr;
	}

protected:
	Quad *_childQuad = nullptr;
};

class Quadtree
{
public:
	typedef std::function<void (const std::shared_ptr<Shape> &other_shape)> EnumCollidingShapes_Function;
	typedef std::function<bool (const std::shared_ptr<Shape> &other_shape)> EnumCollidingShapes_FunctionCancellable;

	Quadtree();
	~Quadtree();

	void Set(const MX::Rectangle &rect) { _quad.Set(rect, 0, 0); }
	bool AddShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape); //returns false if _excludeShapesOutsideBounds and shape is outside bounds 

	bool ShapeMoved(const std::shared_ptr<Shape> &shape); //returns false if _excludeShapesOutsideBounds and shape is outside bounds 
	void EnumCollidingShapes(const std::shared_ptr<Shape> &shape, const EnumCollidingShapes_Function& function);
	void EnumCollidingShapesCancellable(const std::shared_ptr<Shape> &shape, const EnumCollidingShapes_FunctionCancellable& function);
	void EnumAllShapes(const EnumCollidingShapes_Function& function);

	void SetExclusionOutsideBounds(bool exclude);

	std::shared_ptr<Shape> ClosestNeighbor(const glm::vec2 &point, float max_range = -1, const LayeredArea::MatchingFunction& acceptableShapes = nullptr);
protected:
	bool shouldExclude(const std::shared_ptr<Shape> &shape, Quad* quad);

	Quad _quad;

	ExclusionQuad _excludingQuad;
	bool _excludeShapesOutsideBounds = false;
};



    
    
class QuadtreeWeakLayeredArea : public LayeredArea
{
public:
	QuadtreeWeakLayeredArea(const Rectangle &rect, bool excludeShapesOutsideBounds = false);
	QuadtreeWeakLayeredArea(float width, float height, bool excludeShapesOutsideBounds = false);
    ~QuadtreeWeakLayeredArea();
    
    struct Layer;
    typedef std::map<ClassID<>::type, std::shared_ptr<Layer>> Layers;
    struct Layer : virtual public disable_copy_constructors
    {
		Layer(const Rectangle &rect, bool excludeShapesOutsideBounds)
		{
			tree.Set(rect);
			tree.SetExclusionOutsideBounds(excludeShapesOutsideBounds);
		}
        std::list<Layer*> collidingLayers;
        Quadtree tree;
    };
    
    
    unsigned shapes_count();
	void DefineLayerCollision(ClassID<>::type layerType1, ClassID<>::type layerType2);
    void TestForeignShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) override;
    void AddShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) override;
	std::shared_ptr<Shape> ClosestNeighbor(ClassID<>::type layerType, const glm::vec2 &point, float max_range = -1, const MatchingFunction& acceptableShapes = nullptr) override;

	void for_each(const std::function<void(const std::shared_ptr<Shape>&)> &functor) override
	{
		for (auto& layer : _layers)
			layer.second->tree.EnumAllShapes(functor);
	}
protected:
	std::shared_ptr<Layer>& layer(ClassID<>::type layerType);
	
	void ShapeMoved(const std::shared_ptr<Shape> &shape) override;
    
	bool _excludeShapesOutsideBounds = false;
	Rectangle               _rectangle;
    Layers _layers;
};
    

}
}
