#pragma once
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "utils/shapes/Rectangle.h"
#include "utils/shapes/OrientedRectangle.h"
#include "utils/shapes/Circle.h"

namespace MX
{
namespace Collision
{
	struct PointShape_Policy_Tag {};

	template<typename Parent>
	class PointShape_Policy : public Parent
	{
	public:
		PointShape_Policy()
		{
			Parent::SetID(ClassID<PointShape_Policy_Tag>::id());
			SetPoint({ 0.0f, 0.0f });
		}
		PointShape_Policy(const glm::vec2 &point)
		{
			Parent::SetID(ClassID<PointShape_Policy_Tag>::id());
			SetPoint(point);
		}

		const glm::vec2 &point() const
		{
			return (const glm::vec2 &)Parent::_bounds;
		}

		void SetPoint(const glm::vec2 &point)
		{
			Parent::_bounds.SetPoint(point.x, point.y);
			Parent::Moved();
		}

		void SetPoint(float x, float y)
		{
			Parent::_bounds.SetPoint(x, y);
			Parent::Moved();
		}

		void DebugDraw() override
		{
			//TODO draw X
		}

		const Geometry& geometry() const override { return Parent::_bounds; }
	};


	struct RectangleShape_Policy_Tag {};
	template<typename Parent>
	class RectangleShape_Policy : public Parent
	{
	public:
		RectangleShape_Policy()
		{
			Parent::SetID(ClassID<RectangleShape_Policy_Tag>::id());
			Set(0.0f, 0.0f, 0.0f, 0.0f);
		}


		void Set(float x1, float y1, float x2, float y2)
		{
			Parent::_bounds.x1 = x1;
			Parent::_bounds.y1 = y1;
			Parent::_bounds.x2 = x2;
			Parent::_bounds.y2 = y2;
			Parent::Moved();
		}
		void Set(const Rectangle& rectangle)
		{
			Parent::_bounds = rectangle;
			Parent::Moved();
		}

		const Rectangle& rectangle() const { return Parent::_bounds; }

		void DebugDraw() override
		{
			Parent::_bounds.Draw();
		}

		const Geometry& geometry() const override { return Parent::_bounds; }
	};


	struct CircleShape_Policy_Tag {};
	template<typename Parent>
	class CircleShape_Policy : public Parent
	{
	public:
		CircleShape_Policy()
		{
			Parent::SetID(ClassID<CircleShape_Policy_Tag>::id());
		}
		CircleShape_Policy(const glm::vec2 &point, float r)
		{
			Parent::SetID(ClassID<CircleShape_Policy_Tag>::id());
			Set(point, r);
		}

		void Set(const glm::vec2 &point, float r)
		{
			_circle.center = point;
			_circle.radius = r;
			Parent::_bounds.SetPointWH(point.x - r, point.y - r, r * 2, r * 2);
			Parent::Moved();
		}

		void SetRadius(float r)
		{
			Set(_circle.center, r);
		}
		void SetPosition(const glm::vec2 &point)
		{
			Set(point, _circle.radius);
		}

		void DebugDraw() override
		{
			_circle.Draw();
		}
		const Geometry& geometry() const override { return _circle; }
	protected:
		Circle _circle;
	};


	struct OrientedRectangle_Policy_Tag {};
	template<typename Parent>
	class OrientedRectangleShape_Policy : public Parent
	{
	public:
		OrientedRectangleShape_Policy()
		{
			Parent::SetID(ClassID<OrientedRectangle_Policy_Tag>::id());
		}
		OrientedRectangleShape_Policy(const OrientedRectangle& rectangle)
		{
			Parent::SetID(ClassID<OrientedRectangle_Policy_Tag>::id());
			Set(rectangle);
		}

		void Set(const OrientedRectangle& rectangle)
		{
			_orientedRectangle = rectangle;
			Parent::_bounds = _orientedRectangle.bounds();
			Parent::Moved();
		}

		const OrientedRectangle& orientedRectangle() const { return _orientedRectangle; }

		void DebugDraw() override
		{
			_orientedRectangle.Draw();
		}

		const Geometry& geometry() const override { return _orientedRectangle; }
	protected:
		OrientedRectangle _orientedRectangle;
	};

#ifdef WIPPOLYGON
	struct Polygon_Policy_Tag;
	template<typename Parent>
	class PolygonShape_Policy : public Parent
	{
	public:
		PolygonShape_Policy()
		{
			SetID(ClassID<Polygon_Policy_Tag>::id());
		}
		PolygonShape_Policy(const OrientedRectangle& rectangle)
		{
			SetID(ClassID<Polygon_Policy_Tag>::id());
			Set(rectangle);
		}

		void Set(const cinder::Shape2d& rectangle)
		{
			_shape2D = rectangle;
			auto r = _shape2D.calcPreciseBoundingBox();
			_bounds = MX::Rectangle(r.getX1(), r.getY1(), r.getX2(), r.getY2());
			Moved();
		}


		const cinder::Shape2d& polygon() const { return _shape2D; }

		void DebugDraw() override
		{
			MX::Graphic::Utils::DrawPolygon(_shape2D);
		}

		const Geometry& geometry() const override { return (const Geometry&)_shape2D; }
	protected:
		cinder::Shape2d _shape2D;
	};
#endif
/*






	*/
}
};
