#pragma once
#include <set>
#include <map>
#include <list>
#include <functional>

#if WIP
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
namespace bm = boost::bimaps;
#endif
#include "Utils/Utils.h"
#include "Utils/StrongList.h"
#include "Utils/Vector2.h"


namespace MX
{
namespace Collision
{

class Shape;
class ShapeCollision;

class Area : public virtual shared_ptr_init<Area>
{
public:
	friend class Shape;

#ifdef WIP
	typedef bm::bimap<
    bm::unordered_multiset_of< Shape* >,
    bm::unordered_multiset_of< Shape* >,
	bm::set_of_relation<>,
	bm::with_info< std::shared_ptr<ShapeCollision> >  // abstract
	> ShapeCollision_bimap;

	typedef ShapeCollision_bimap::value_type ShapeCollisionRelation;
#endif

	friend class AreaLink;
	Area();
	virtual ~Area();

	virtual void TestForeignShape(const std::shared_ptr<Shape> &shape) = 0;
	virtual void AddShape(const std::shared_ptr<Shape> &shape) = 0;


	void ProcessCollisions(const std::shared_ptr<Shape> &shape, StrongList<Shape> &shapes, bool shapeIsInList);
	void ProcessCollisions(const std::shared_ptr<Shape> &shape, WeakList<Shape> &shapes, bool shapeIsInList);
	void ProcessCollision(const std::shared_ptr<Shape> &shape, const std::shared_ptr<Shape> &other_shape, bool shapeIsInList);

	virtual void for_each(const std::function<void(const std::shared_ptr<Shape>&)> &functor) {};
	virtual void DebugDraw();
protected:
	void ValidateExistingCollisions(Shape *shape, bool unlink = false);
	void LinkShape(Shape *shape);
	void UnlinkingShape(Shape *shape);

	virtual void ShapeMoved(const std::shared_ptr<Shape> &shape) = 0;

#ifdef WIP
	ShapeCollision_bimap _existingCollisions;
#endif
};





class SimplestArea : public Area
{
public:
	~SimplestArea();

	void TestForeignShape(const std::shared_ptr<Shape> &shape);
	void AddShape(const std::shared_ptr<Shape> &shape);
protected:
	void ShapeMoved(const std::shared_ptr<Shape> &shape);
	StrongList<Shape> _shapes;
	
};

class SimplestWeakArea : public Area
{
public:
	~SimplestWeakArea();

	void TestForeignShape(const std::shared_ptr<Shape> &shape);
	void AddShape(const std::shared_ptr<Shape> &shape);
protected:
	void ShapeMoved(const std::shared_ptr<Shape> &shape);
	WeakList<Shape> _shapes;
	
};

class LayeredArea : public Area
{
public:
	using MatchingFunction = std::function<bool(const std::shared_ptr<Shape>& shape)>;

	virtual void DefineLayerCollision(ClassID<>::type layer1, ClassID<>::type layer2) = 0;
	virtual void TestForeignShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) = 0;
	virtual void AddShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) = 0;
	virtual std::shared_ptr<Shape> ClosestNeighbor(ClassID<>::type layer, const glm::vec2 &point, float max_range = -1, const MatchingFunction& acceptableShapes = nullptr) { return nullptr; }
protected:
	void TestForeignShape(const std::shared_ptr<Shape> &shape){};
	void AddShape(const std::shared_ptr<Shape> &shape){};
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
	void TestForeignShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) override;
	void AddShape(ClassID<>::type type, const std::shared_ptr<Shape> &shape) override;


protected:
	void ShapeMoved(const std::shared_ptr<Shape> &shape);
	
	Layers _layers;
};


}
};
