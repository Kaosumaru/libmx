#include "Area.h"
#include "collision/shape/Shape.h"

using namespace MX;
using namespace Collision;


Area::Area()
{

}

Area::~Area()
{

}


void Area::ProcessCollisions(const Shape::pointer &shape, StrongList<Shape> &shapes, bool shapeIsInList)
{
	if (!shape->enabled())
		return;
	auto guard = shapes.guard();
	auto it = shapes.begin();
	while ( it != shapes.end())
	{
		auto &shape2 = *it;
		if (!shape)
			return;
		ProcessCollision(shape, shape2, shapeIsInList);
		shapes.temp_advance(it);
	}

}

void Area::ProcessCollisions(const Shape::pointer &shape, WeakList<Shape> &shapes, bool shapeIsInList)
{
	if (!shape->enabled())
		return;

	auto guard = shapes.guard();
	auto it = shapes.begin();
	while ( it != shapes.end())
	{
		Shape::pointer other_shape = it->lock();
		if (!other_shape)
			continue;
		if (!shape)
			return;
		ProcessCollision(shape, other_shape, shapeIsInList);
		shapes.temp_advance(it);

	}
}


void Area::ProcessCollision(const Shape::pointer &shape, const Shape::pointer &other_shape, bool shapeIsInList)
{
	if (!shape || !other_shape->enabled())
		return;

	if (shape == other_shape)
		return;

	ShapeCollision::pointer emptyCollision;

	bool linked = other_shape->linked();
	bool track = (shape->trackCollisions() || other_shape->trackCollisions()) && shapeIsInList;
	bool collided = false;
	bool firstCollision = false;

	if (linked && other_shape != shape && shape->intersectsWith(*other_shape))
		collided = true;

	ShapeCollision::pointer *collision = &emptyCollision;
	if (track)
	{
		Shape* shape1 = std::min(shape.get(), other_shape.get());
		Shape* shape2 = std::max(shape.get(), other_shape.get());
		if (collided)
		{
			collision = &(_existingCollisions.insert( shape1, shape2 ).value);
			if (*collision == nullptr)
			{
				*collision = std::make_shared<ShapeCollision>();
				firstCollision = true;
				shape1->_existingCollisons++;
				shape2->_existingCollisons++;
			}
		}
		else
		{
			auto fit = _existingCollisions.find(shape1, shape2);
			if (fit != _existingCollisions.end())
			{
				collision = &(fit->value);
				(*collision)->onCollisionEnd();
				_existingCollisions.erase(fit);
				shape1->_existingCollisons--;
				shape2->_existingCollisons--;
			}
		}
	}

	//TODO probably needed, as shape->OnCollision(other_shape, *collision); can set shape to null
	auto &ref_shape = shape;
	auto &ref_other_shape = other_shape;


	if (firstCollision)
	{
		ref_shape->OnFirstCollision(ref_other_shape, *collision);
		ref_other_shape->OnFirstCollision(ref_shape, *collision);
	}

	if (collided)
	{
		ref_shape->OnCollision(ref_other_shape, *collision);
		ref_other_shape->OnCollision(ref_shape, *collision);
	}
}


void Area::ValidateExistingCollisions(Shape *shape, bool unlink)
{
	if (shape->_existingCollisons == 0)
		return;

	bool found = false;
	_existingCollisions.enumerate(shape, [shape, unlink, &found](auto &other, auto &data) 
	{
		if (!data.value)
			return;
		auto &other_shape = other;

		if (!unlink && !shape->intersectsWith(*other_shape))
			return;
		//mark for erasure
		data.value->onCollisionEnd();
		data.value = nullptr;
		found = true;
	});

	_existingCollisions.remove_if([](auto &data) { return data.value == nullptr; });
}

void Area::LinkShape(Shape *shape)
{
	shape->_area = this;
}

void Area::UnlinkingShape(Shape *shape)
{
	ValidateExistingCollisions(shape, true);
}


void Area::DebugDraw()
{
	for_each([](const Shape::pointer& shape){ if (shape->enabled()) shape->DebugDraw(); });
}


SimplestArea::~SimplestArea()
{
	auto it = _shapes.begin();
	while ( it != _shapes.end())
	{
		(*it)->Unlink();
		it ++;
	}
}


void SimplestArea::TestForeignShape(const Shape::pointer &shape)
{
	ProcessCollisions(shape, _shapes, false);
}

void SimplestArea::AddShape(const Shape::pointer &shape)
{
	assert(!shape->linked());
	_shapes.push_back(shape);
	LinkShape(&*shape);
	ProcessCollisions(shape, _shapes, true);
}


void SimplestArea::ShapeMoved(const Shape::pointer &shape)
{
	ProcessCollisions(shape, _shapes, true);
}

SimplestWeakArea::~SimplestWeakArea()
{

}



void SimplestWeakArea::TestForeignShape(const Shape::pointer &shape)
{
	ProcessCollisions(shape, _shapes, false);
}


void SimplestWeakArea::AddShape(const Shape::pointer &shape)
{
	_shapes.push_back(shape);
	LinkShape(&*shape);
	ProcessCollisions(shape, _shapes, true);
}

void SimplestWeakArea::ShapeMoved(const Shape::pointer &shape)
{
	ProcessCollisions(shape, _shapes, true);
}


void SimplestWeakLayeredArea::DefineLayerCollision(ClassID<>::type layer1, ClassID<>::type layer2)
{
	Layers::iterator it1 = _layers.insert (std::make_pair(layer1, Layer())).first;
	Layers::iterator it2 = _layers.insert (std::make_pair(layer2, Layer())).first;


	it1->second.collidingLayers.push_back(&(it2->second));
	it2->second.collidingLayers.push_back(&(it1->second));


}

SimplestWeakLayeredArea::~SimplestWeakLayeredArea()
{
	for (auto layer = _layers.begin(); layer!= _layers.end(); layer ++)
	{
		auto shapes = layer->second.shapes;
		auto it = shapes.begin();
		while ( it != shapes.end())
		{
			auto shape = it->lock();
			if (shape)
				shape->Unlink();
			it ++;
		}
	}
}


void SimplestWeakLayeredArea::TestForeignShape(ClassID<>::type type, const Shape::pointer &shape)
{
	auto &layers = _layers[type].collidingLayers;
	for (auto &layer : layers)
		ProcessCollisions(shape, layer->shapes, false);
}


void SimplestWeakLayeredArea::AddShape(ClassID<>::type type, const Shape::pointer &shape)
{
	auto &list = _layers[type].shapes;
	list.push_back(shape, type);
	LinkShape(&*shape);

	auto &layers = _layers[type].collidingLayers;
	for (auto &layer : layers)
		ProcessCollisions(shape, layer->shapes, true);
}



void SimplestWeakLayeredArea::ShapeMoved(const Shape::pointer &shape)
{
	//TODO
	auto type = shape->link()->type();

	
	auto &layers = _layers[type].collidingLayers;
	for (auto &layer : layers)
		ProcessCollisions(shape, layer->shapes, true);
	
}

std::size_t SimplestWeakLayeredArea::shapes_count()
{
	//TODO optimize
	std::size_t count = 0;
	for (auto &layer : _layers)
		count += layer.second.shapes.size();
	return count;
}	

