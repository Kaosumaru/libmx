#include "Shape.h"
#include "collision/area/Area.h"
#include "collision/detection/CollisionDetector.h"

using namespace MX;
using namespace Collision;


Shape::Shape(bool trackCollisions)
{
	_trackCollisions = trackCollisions;
}


Shape::~Shape()
{
	if (linked())
	{
		Unlink();
	}
}


void Shape::Enable()
{
	_enabled = true;
}

void Shape::Disable()
{
	_enabled = false;
}

bool Shape::enabled()
{
	return _enabled;
}

bool Shape::linked() const
{
	return _area != nullptr;
}

void Shape::Unlink()
{
	if (_area)
		_area->UnlinkingShape(this);
	_area = nullptr;
	Linkable::Unlink();
}

bool Shape::trackCollisions() const
{
	return _trackCollisions;
}

void Shape::OnCollision(const Shape::pointer& shape, const ShapeCollision::pointer &collision)
{
}

void Shape::OnFirstCollision(const Shape::pointer& shape, const ShapeCollision::pointer &collision)
{
}

bool Shape::boundsIntersectsWith(const Shape &shape)
{
	return bounds().intersects(shape.bounds());
}

bool Shape::intersectsWith(const Shape &shape)
{
	if (boundsIntersectsWith(shape))
		return Detection::get().checkCollision(*this, shape);
	return false;
}

float Shape::distanceTo(const Shape &shape) const
{
	return distanceBetween(center(), shape.center());
}

float Shape::distanceToSquared(const Shape &shape) const
{
	return distanceBetweenSquared(center(), shape.center());
}


ClassID<>::type Shape::shapeID() const
{
	return _shapeID;
}

ClassID<>::type Shape::classID() const
{
	return _classID;
}

void Shape::SetID(ClassID<>::type id)
{
	_shapeID = _classID = id;
}

void Shape::SetClassID(ClassID<>::type id)
{
	_classID = id;
}

const MX::Rectangle& Shape::bounds() const
{
	return _bounds;
}

void Shape::Moved()
{
	if (_area)
    {
        auto shared_this = shared_from_this();
		_area->ShapeMoved(shared_this);
    }
}





