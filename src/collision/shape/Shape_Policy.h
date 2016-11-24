#ifndef MXSHAPEPOLICY
#define MXSHAPEPOLICY
#include "Utils/Utils.h"
#include "Utils/Vector2.h"
#include "Utils/shapes/Rectangle.h"
#include "Utils/shapes/OrientedRectangle.h"
#include "Utils/shapes/Circle.h"

namespace MX
{
namespace Collision
{
	struct PointShape_Policy_Tag;

	template<typename Parent>
	class PointShape_Policy : public Parent
	{
	public:
		PointShape_Policy()
		{
			SetID(ClassID<PointShape_Policy_Tag>::id());
			SetPoint({ 0.0f, 0.0f });
		}
		PointShape_Policy(const glm::vec2 &point)
		{
			SetID(ClassID<PointShape_Policy_Tag>::id());
			SetPoint(point);
		}

		const glm::vec2 &point() const
		{
			return (const glm::vec2 &)_bounds;
		}

		void SetPoint(const glm::vec2 &point)
		{
			_bounds.SetPoint(point.x, point.y);
			Moved();
		}

		void SetPoint(float x, float y)
		{
			_bounds.SetPoint(x, y);
			Moved();
		}

		void DebugDraw() override
		{
			//TODO draw X
		}

		const Geometry& geometry() const override { return _bounds; }
	};


	struct RectangleShape_Policy_Tag;
	template<typename Parent>
	class RectangleShape_Policy : public Parent
	{
	public:
		RectangleShape_Policy()
		{
			SetID(ClassID<RectangleShape_Policy_Tag>::id());
			Set(0.0f, 0.0f, 0.0f, 0.0f);
		}


		void Set(float x1, float y1, float x2, float y2)
		{
			_bounds.x1 = x1;
			_bounds.y1 = y1;
			_bounds.x2 = x2;
			_bounds.y2 = y2;
			Moved();
		}
		void Set(const Rectangle& rectangle)
		{
			_bounds = rectangle;
			Moved();
		}

		const Rectangle& rectangle() const { return _bounds; }

		void DebugDraw() override
		{
			_bounds.Draw();
		}

		const Geometry& geometry() const override { return _bounds; }
	};


	struct CircleShape_Policy_Tag;
	template<typename Parent>
	class CircleShape_Policy : public Parent
	{
	public:
		CircleShape_Policy()
		{
			SetID(ClassID<CircleShape_Policy_Tag>::id());
		}
		CircleShape_Policy(const glm::vec2 &point, float r)
		{
			SetID(ClassID<CircleShape_Policy_Tag>::id());
			Set(point, r);
		}

		void Set(const glm::vec2 &point, float r)
		{
			_circle.center = point;
			_circle.radius = r;
			_bounds.SetPointWH(point.x - r, point.y - r, r * 2, r * 2);
			Moved();
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


	struct OrientedRectangle_Policy_Tag;
	template<typename Parent>
	class OrientedRectangleShape_Policy : public Parent
	{
	public:
		OrientedRectangleShape_Policy()
		{
			SetID(ClassID<OrientedRectangle_Policy_Tag>::id());
		}
		OrientedRectangleShape_Policy(const OrientedRectangle& rectangle)
		{
			SetID(ClassID<OrientedRectangle_Policy_Tag>::id());
			Set(rectangle);
		}

		void Set(const OrientedRectangle& rectangle)
		{
			_orientedRectangle = rectangle;
			_bounds = _orientedRectangle.bounds();
			Moved();
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

#if WIP
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
#endif
