#include "CollisionDetector.h"
#include "utils/shapes/Circle.h"
#include "utils/shapes/Rectangle.h"
#include "utils/shapes/OrientedRectangle.h"
#include "collision/shape/Shape.h"
#include "utils/Vector2.h"

using namespace MX;
using namespace Collision;


bool _collision_rectangle_circle(const MX::Rectangle& rectangle, const Circle& circle)
{
	glm::vec2 clamped = circle.center;
	clamp(clamped.x, rectangle.x1, rectangle.x2);
	clamp(clamped.y, rectangle.y1, rectangle.y2);

	float x = circle.center.x - clamped.x;
	float y = circle.center.y - clamped.y;
	return x*x + y*y <= circle.radius * circle.radius;
}

bool _collision_orientedrectangle_circle(const OrientedRectangle& rectangle, const Circle& circle)
{
	float angle = rectangle.angle() - (float)MX_PI / 2.0f;

	auto center_to_center = vectorFromPointToPoint(rectangle.center(), circle.center);


	MX::Rectangle rect(-rectangle.width() / 2.0f, -rectangle.height() / 2.0f, rectangle.width() / 2.0f, rectangle.height() / 2.0f);

	Circle local_circle = { vectorByRotation(center_to_center, -angle), circle.radius };

	return _collision_rectangle_circle(rect, local_circle);
}

bool _collision_orientedrectangle_point(const OrientedRectangle& rectangle, const glm::vec2& point)
{
	float angle = rectangle.angle() - (float)MX_PI / 2.0f;

	auto center_to_center = vectorFromPointToPoint(rectangle.center(), point);

	MX::Rectangle rect(-rectangle.width() / 2.0f, -rectangle.height() / 2.0f, rectangle.width() / 2.0f, rectangle.height() / 2.0f);

	auto local_point = vectorByRotation(center_to_center, -angle);
	return rect.contains(local_point);
}



bool Collision_Point_Point(const Shape &shape1, const Shape &shape2)
{
	return true; //optimalization

#if OPTED_OUT
	const PointShape &point1 = (const PointShape &)shape1;
	const PointShape &point2 = (const PointShape &)shape2;
	return point1.x() == point2.x() && point1.y() == point2.y();
#endif
}

bool Collision_Point_Rectangle(const Shape &shape1, const Shape &shape2)
{
	return true; //optimalization

#if OPTED_OUT
	const PointShape &point1 = (const PointShape &)shape1;
	const RectangleShape &rectangle1 = (const RectangleShape &)shape2;
	return rectangle1.rectangle().contains(point1.point());
#endif
}

bool Collision_Point_Circle(const Shape &shape1, const Shape &shape2)
{
	auto &point1 = (const glm::vec2 &)shape1.geometry();
	auto &circle1 = (const Circle &)shape2.geometry();

	auto v = circle1.center - point1;
	return v.x*v.x + v.y*v.y <= circle1.radius * circle1.radius;

#if OLD_COLLISIONS
	const PointShape &point1 = (const PointShape &)shape1;
	const CircleShape &circle1 = (const CircleShape &)shape2;
	float x = circle1.x() - point1.x();
	float y = circle1.y() - point1.y();
	return x*x+y*y <= circle1.r() * circle1.r();
#endif
}


bool Collision_Rectangle_Rectangle(const Shape &shape1, const Shape &shape2)
{
	return true; //optimalization

#if OPTED_OUT
	const RectangleShape &rectangle1 = (const RectangleShape &)shape1;
	const RectangleShape &rectangle2 = (const RectangleShape &)shape2;
	return rectangle1.rectangle().intersects(rectangle2.rectangle());
#endif
}

bool Collision_Rectangle_Circle(const Shape &shape1, const Shape &shape2)
{
	auto &rectangle = (const MX::Rectangle &)shape1.geometry();
	auto &circle = (const Circle &)shape2.geometry();

	return _collision_rectangle_circle(rectangle, circle);
#if OLD_COLLISIONS
	const RectangleShape &rectangle1 = (const RectangleShape &)shape1;
	const CircleShape &circle = (const CircleShape &)shape2;
	return _collision_rectangle_circle(rectangle1.rectangle(), { circle.center(), circle.r() });
#endif
}

bool Collision_Circle_Circle(const Shape &shape1, const Shape &shape2)
{
	auto &circle1 = (const Circle &)shape1.geometry();
	auto &circle2 = (const Circle &)shape2.geometry();

	float r = circle1.radius + circle2.radius;
	auto v = circle1.center - circle2.center;

	return v.x*v.x + v.y*v.y <= r * r;
#if OLD_COLLISIONS
	const CircleShape &circle1 = (const CircleShape &)shape1;
	const CircleShape &circle2 = (const CircleShape &)shape2;
	float x = circle1.x() - circle2.x();
	float y = circle1.y() - circle2.y();
	float r = circle1.r() + circle2.r();
	return x*x+y*y <= r*r;
#endif
}




bool Collision_Point_Oriented(const Shape &shape1, const Shape &shape2)
{
	auto &point = (const glm::vec2 &)shape1.geometry();
	auto &orect = (const OrientedRectangle &)shape2.geometry();

	return _collision_orientedrectangle_point(orect, point);
#if OLD_COLLISIONS
	const PointShape &point1 = (const PointShape &)shape1;
	const OrientedRectangleShape &o2 = (const OrientedRectangleShape &)shape2;
	return _collision_orientedrectangle_point(o2.orientedRectangle(), {point1.x(), point1.y()});
#endif
}

bool Collision_Rectangle_Oriented(const Shape &shape1, const Shape &shape2)
{
	auto &rectangle = (const MX::Rectangle &)shape1.geometry();
	auto &orect = (const OrientedRectangle &)shape2.geometry();

	return OrientedRectangle(rectangle).overlaps(orect);
#if OLD_COLLISIONS
	const RectangleShape &o1 = (const RectangleShape &)shape1;
	const OrientedRectangleShape &o2 = (const OrientedRectangleShape &)shape2;
	return OrientedRectangle(o1.rectangle()).overlaps(o2.orientedRectangle());
#endif
}

bool Collision_Circle_Oriented(const Shape &shape1, const Shape &shape2)
{
	auto &circle = (const Circle &)shape1.geometry();
	auto &orect = (const OrientedRectangle &)shape2.geometry();
	return _collision_orientedrectangle_circle(orect, circle);

#if OLD_COLLISIONS
	const CircleShape &circle = (const CircleShape &)shape1;
	const OrientedRectangleShape &o2 = (const OrientedRectangleShape &)shape2;
	return _collision_orientedrectangle_circle(o2.orientedRectangle(), { circle.center(), circle.r() });
#endif
}

bool Collision_Oriented_Oriented(const Shape &shape1, const Shape &shape2)
{
	auto &orect1 = (const OrientedRectangle &)shape1.geometry();
	auto &orect2 = (const OrientedRectangle &)shape2.geometry();
	return orect1.overlaps(orect2);
#if OLD_COLLISIONS
	const OrientedRectangleShape &o1 = (const OrientedRectangleShape &)shape1;
	const OrientedRectangleShape &o2 = (const OrientedRectangleShape &)shape2;
	return o1.orientedRectangle().overlaps(o2.orientedRectangle());
#endif
}


#ifdef WIP
bool Collision_Polygon_Point(const Shape &shape1, const Shape &shape2)
{
	assert( false );

	auto &polygon = (const cinder::Shape2d&)shape1.geometry();
	auto &point = (const glm::vec2 &)shape2.geometry();

	return polygon.contains(glm::vec2(point.x, point.y));

#if OLD_COLLISIONS
	const PointShape &point1 = (const PointShape &)shape1;
	const OrientedRectangleShape &o2 = (const OrientedRectangleShape &)shape2;
	return _collision_orientedrectangle_point(o2.orientedRectangle(), { point1.x(), point1.y() });
#endif
}
#endif
Detection::Detection()
{
	Init();
}


void Detection::Init()
{
#if WIP
	AddCollision<PointShape_Policy_Tag, PointShape_Policy_Tag, &Collision_Point_Point>();
	AddCollision<PointShape_Policy_Tag, RectangleShape_Policy_Tag, &Collision_Point_Rectangle>();
	AddCollision<PointShape_Policy_Tag, CircleShape_Policy_Tag, &Collision_Point_Circle>();
	AddCollision<PointShape_Policy_Tag, OrientedRectangle_Policy_Tag, &Collision_Point_Oriented>();

	AddCollision<RectangleShape_Policy_Tag, RectangleShape_Policy_Tag, &Collision_Rectangle_Rectangle>();
	AddCollision<RectangleShape_Policy_Tag, CircleShape_Policy_Tag, &Collision_Rectangle_Circle>();
	AddCollision<RectangleShape_Policy_Tag, OrientedRectangle_Policy_Tag, &Collision_Rectangle_Oriented>();

	AddCollision<CircleShape_Policy_Tag, CircleShape_Policy_Tag, &Collision_Circle_Circle>();
	AddCollision<CircleShape_Policy_Tag, OrientedRectangle_Policy_Tag, &Collision_Circle_Oriented>();

	AddCollision<OrientedRectangle_Policy_Tag, OrientedRectangle_Policy_Tag, &Collision_Oriented_Oriented>();

	AddCollision<Polygon_Policy_Tag, PointShape_Policy_Tag, &Collision_Polygon_Point>();
#endif
}

bool Detection::checkCollision(const Shape& shape1, const Shape& shape2)
{
	auto it = _functionForIDs.find(std::make_pair(shape1.shapeID(), shape2.shapeID()));
	if (it == _functionForIDs.end())
		return false;
	return it->second(shape1, shape2);
}