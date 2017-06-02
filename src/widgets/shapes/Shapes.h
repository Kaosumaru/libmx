#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "collision/shape/SignalizingShape.h"


namespace MX
{

namespace Collision
{
	class SignalizingShape;
};

namespace Widgets
{

struct WidgetsLayer {};

class Widget;


class ShapePolicy
{
public:
	using pointer = std::unique_ptr<ShapePolicy>;
	virtual ~ShapePolicy(){}

	virtual void OnEnabled() {}
	virtual void OnDisabled() { _shape = nullptr; }
	virtual void OnMoved(Widget& w) {}
	virtual void OnTransformMatrixChanged(Widget& w, const glm::mat4 &transform) {}
	void DebugDraw() { if (_shape) _shape->DebugDraw(); }

	const auto& shape() { return _shape; }
	virtual pointer clone() = 0;

#ifdef WIPPOLYGON
	static pointer createPolygon(const cinder::Shape2d &polygon);
#endif
protected:
	std::shared_ptr<Collision::SignalizingShape> _shape;
};

class DefaultRectanglePolicy : public ShapePolicy
{
public:
	void OnEnabled() override
	{
		_shape = std::make_shared<Collision::SignalizingRectangleShape>();
		_shape->SetTrackCollisions(true);
	}

	void OnMoved(Widget& w) override;

	pointer clone() override { return std::make_unique<DefaultRectanglePolicy>(); }
};





class ShapePolicyController
{
public:
	ShapePolicyController(Widget& w)
	{
		_shouldBeConnected = shouldBeConnected(w);
		shapePolicy().OnEnabled();
	}

	~ShapePolicyController()
	{
		shapePolicy().OnDisabled();
	}

	friend class Widget;
protected:

	const auto& shape() { return shapePolicy().shape(); }



	bool Change(std::unique_ptr<ShapePolicy>&& newPolicy, Widget& w);


	void OnTransformMatrixChanged(Widget& w, const glm::mat4 &transform)
	{
		shapePolicy().OnTransformMatrixChanged(w, transform);
	}

	void OnMoved(Widget& w)
	{
		shapePolicy().OnMoved(w);
	}




	bool shouldBeConnected(Widget& w);

	void ChangeState(Widget& w);

	void DebugDraw()
	{
		if (shape() && shape()->linked())
			shapePolicy().DebugDraw();
	}

	ShapePolicy& shapePolicy()
	{
		if (_customPolicy)
			return *_customPolicy;
		return _defaultPolicy;
	}


protected:
	bool                         _shouldBeConnected = false;

	std::unique_ptr<ShapePolicy> _customPolicy;
	DefaultRectanglePolicy       _defaultPolicy;
};



class ShapePolicyInit
{
public:
	static void Init();
};


}

using WidgetPtr = std::shared_ptr<Widgets::Widget>;

}

