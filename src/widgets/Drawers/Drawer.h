#pragma once
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "utils/shapes/Rectangle.h"
#include "script/ScriptObject.h"
#include "stx/optional.hpp"
#include "widgets/shapes/Shapes.h"

namespace MX
{
namespace Widgets
{

class ShapePolicy;
class Widget;

//TODO it would be nifty if Drawer wouldn't have to derive from ScriptObject
class Drawer : public ScriptObject, public shared_ptr_init<Drawer>
{
public:
	Drawer() {}
	Drawer(LScriptObject& script){}

	virtual ~Drawer(){}
	virtual void DrawBackground(){};
	virtual void DrawForeground() {};
	virtual bool ShouldDrawWidget() { return true; };

	virtual void OnAssignedToWidget() {};

	virtual std::unique_ptr<ShapePolicy> defaultShapePolicy() { return nullptr; }
	virtual stx::optional<MX::Margins> defaultMargins() { return {}; }
	virtual stx::optional<glm::vec2> defaultSize() { return {}; }

	virtual void clipSize(float &width, float &height) {}

	static bool isDragged();
	static bool drawDragged();

	virtual int version() { return 0; }

	struct Drag : public ScopeSingleton<Drag>
	{
		Drag() { drawDragged = false; }
		Drag(Widget* widget, bool drawDragged) : widget(widget), drawDragged(drawDragged) {}

		Widget* widget;
		bool drawDragged;
	};

	struct ContextData : public ScopeSingleton<ContextData>
	{
		ContextData() {}
		bool drawChildren = true;
        bool drawDebugDrawers = false;
	};

	struct Destination : public ScopeSingleton<Destination>
	{
		Destination() { }
		Destination(const Rectangle& rect) : rectangle(rect) {}


		static Destination destinationClip(const Rectangle& rect);

		float width() { return rectangle.width(); }
		float height() { return rectangle.height(); }

		float x() { return rectangle.x1; }
		float y() { return rectangle.y1; }

		Rectangle rectangle;
	};

protected:
	void widgetDrawChildren();
};


class DrawerProxy : public Drawer
{
public:
	DrawerProxy()
	{
	}

	DrawerProxy(LScriptObject& script);


	void DrawBackground() override { return _drawer->DrawBackground(); };
	void DrawForeground() override { _drawer->DrawForeground(); if (_drawerFG) _drawerFG->DrawBackground(); };
	bool ShouldDrawWidget() override { return _drawer->ShouldDrawWidget(); };

	void OnAssignedToWidget() override { return _drawer->OnAssignedToWidget(); };

	std::unique_ptr<ShapePolicy> defaultShapePolicy() override { return _drawer->defaultShapePolicy(); };
	stx::optional<MX::Margins> defaultMargins() override { return _drawer->defaultMargins(); };
	stx::optional<glm::vec2> defaultSize() override { return _drawer->defaultSize(); };

	void clipSize(float &width, float &height) override { return _drawer->clipSize(width, height); }

protected:
	Drawer::pointer _drawer;
	Drawer::pointer _drawerFG;
};



}
}
