#include "Shapes.h"
#include "Widgets/Widget.h"
#include "Application/Window.h"
#include "Collision/Area/Area.h"
#include "Script/ScriptClassParser.h"
#include <glm/gtx/transform.hpp>

using namespace MX;
using namespace MX::Widgets;




void DefaultRectanglePolicy::OnMoved(Widget& w)
{
	auto s = (Collision::SignalizingRectangleShape*)_shape.get();
	s->Set(w.geometry.position.x, w.geometry.position.y, w.geometry.position.x + w.Width(), w.geometry.position.y + w.Height());
}




bool ShapePolicyController::shouldBeConnected(Widget& w)
{
	return w.visible() && w.linked() && w.interactive();
}


bool ShapePolicyController::Change(std::unique_ptr<ShapePolicy>&& newPolicy, Widget& w)
{
	if (_customPolicy == newPolicy)
		return false;
	shape()->Unlink();
	shapePolicy().OnDisabled();
	_customPolicy = std::move(newPolicy);
	shapePolicy().OnEnabled();

	_shouldBeConnected = false;
	ChangeState(w);
	return true;
}

void ShapePolicyController::ChangeState(Widget& w)
{
	auto s = shouldBeConnected(w);
	if (_shouldBeConnected == s)
		return;
	_shouldBeConnected = s;

	if (!s)
	{
		if (shape())
			shape()->Unlink();
		return;
	}

	OnMoved(w);
	Window::current().area()->AddShape(ClassID<WidgetsLayer>::id(), shape()); //TODO shape changes after this won't work
}








class ScriptShapePolicy : public ShapePolicy, public ScriptObject
{
public:
	ScriptShapePolicy() {}
	ScriptShapePolicy(const ScriptShapePolicy& other) : ScriptObject(other) {}
	ScriptShapePolicy(LScriptObject& script) {}

};


class ScriptCirclePolicy : public ScriptShapePolicy
{
public:
	ScriptCirclePolicy() {}
	ScriptCirclePolicy(const ScriptCirclePolicy& other) : ScriptShapePolicy(other) {}
	ScriptCirclePolicy(LScriptObject& script) {}

	~ScriptCirclePolicy() 
	{
	}

	void OnEnabled() override
	{
		_shape = std::make_shared<Collision::SignalizingCircleShape>();
		_shape->SetTrackCollisions(true);
	}
	void OnDisabled() override
	{
		_shape = nullptr;
	}

	void OnMoved(Widget& w) override
	{
		auto s = (Collision::SignalizingCircleShape*)_shape.get();

		auto r = std::min(w.Height() / 2.0f, w.Width() / 2.0f);
		auto pos = w.geometry.position + w.dimensions() / 2.0f;
		s->Set(pos, r);
	}

	pointer clone() override 
	{ 
		return std::make_unique<ScriptCirclePolicy>(*this); 
	}
};


#ifdef WIPPOLYGON
class ScriptPolygonShapePolicy : public ScriptShapePolicy
{
public:
	ScriptPolygonShapePolicy() {}

	ScriptPolygonShapePolicy(const cinder::Shape2d &polygon) 
	{
		_polygon = polygon;
	}

	ScriptPolygonShapePolicy(const ScriptPolygonShapePolicy& other) : ScriptShapePolicy(other) 
	{
		_polygon = other._polygon;
	}

	ScriptPolygonShapePolicy(LScriptObject& script) 
	{
		std::vector<Vector2> points;
		script.load_property_children(points, "Points");

		bool first = true;
		for (auto& p : points)
		{
			if (first)
			{
				first = false;
				_polygon.moveTo(p.x, p.y);
				continue;
			}
			_polygon.lineTo(p.x, p.y);
			
		}
		if (!points.empty())
			_polygon.close();

	}

	pointer clone() override { return std::make_unique<ScriptPolygonShapePolicy>(*this); }


	void OnEnabled() override
	{
		_shape = std::make_shared<Collision::SignalizingPolygonShape>();
		_shape->SetTrackCollisions(true);
	}
	void OnDisabled() override
	{
		_shape = nullptr;
	}

	void OnMoved(Widget& w) override
	{
		auto s = (Collision::SignalizingPolygonShape*)_shape.get();

		auto p = _polygon;
		glm::mat3 m;
		m = glm::translate(m, { w.geometry.position.x, w.geometry.position.y });

		//glm::transla
		s->Set(_polygon.transformed(m));
	}
protected:
	cinder::Shape2d _polygon;
};


class ScriptPolygonRectangleShapePolicy : public ScriptPolygonShapePolicy
{
public:
	using ScriptPolygonShapePolicy::ScriptPolygonShapePolicy;

	pointer clone() override { return std::make_unique<ScriptPolygonRectangleShapePolicy>(*this); }

	void OnMoved(Widget& w) override
	{
		auto s = (Collision::SignalizingPolygonShape*)_shape.get();


		//w.abso

		auto transformPoint = [&](float x, float y)
		{

			glm::vec4 v(x, y, 0.0f, 0.0f);
			auto m = w.absoluteTransformMatrix();

			//m = m *  w.absoluteTransformMatrix();

			v = m * v;
			return glm::vec2(v.x,v.y);
		};

		cinder::Shape2d shape;
		glm::vec3 v;
		shape.moveTo(transformPoint(w.geometry.position.x, w.geometry.position.y));
		shape.lineTo(transformPoint(w.geometry.position.x + w.Width(), w.geometry.position.y));
		shape.lineTo(transformPoint(w.geometry.position.x + w.Width(), w.geometry.position.y + w.Height()));
		shape.lineTo(transformPoint(w.geometry.position.x, w.geometry.position.y + w.Height()));
		shape.close();


		
		//glm::mat3 m;
		//m = glm::translate(m, { w.geometry.position.x, w.geometry.position.y });

		//glm::transla
		s->Set(shape);
	}

	void OnTransformMatrixChanged(Widget& w, const glm::mat4 &transform)
	{
		OnMoved(w);
	}

};
#endif

void MX::Widgets::ShapePolicyInit::Init()
{
	ScriptClassParser::AddCreator(L"Widget.Shape.Circle", new OutsideScriptClassCreatorContructor<ScriptCirclePolicy>());

#ifdef WIPPOLYGON
	ScriptClassParser::AddCreator(L"Widget.Shape.Polygon", new OutsideScriptClassCreatorContructor<ScriptPolygonShapePolicy>());
	ScriptClassParser::AddCreator(L"Widget.Shape.Polygon.Rectangle", new OutsideScriptClassCreatorContructor<ScriptPolygonRectangleShapePolicy>());
#endif
}



#ifdef WIPPOLYGON
ShapePolicy::pointer ShapePolicy::createPolygon(const cinder::Shape2d &polygon)
{
	return std::make_unique<ScriptPolygonShapePolicy>(polygon);
}
#endif