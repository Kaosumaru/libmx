#include "Drawer.h"
#include "Widgets/Widget.h"
#include "Widgets/Systems/DragSystem.h"
#include "Script/PropertyLoaders.h"

using namespace MX::Widgets;

bool Drawer::isDragged()
{
	return Drag::isCurrent() && Drag::current().widget == &(Widget::current());
}

bool Drawer::drawDragged()
{
	return Drag::current().drawDragged;
}


Drawer::Destination Drawer::Destination::destinationClip(const MX::Rectangle& rectangle)
{
	if (isCurrent())
	{
		Destination dest(rectangle);

		auto minus = [](float &number, float max) { number = number < 0.0f ? max + number + 1.0f : number; };

		minus(dest.rectangle.x1, current().width());
		minus(dest.rectangle.x2, current().width());
		minus(dest.rectangle.y1, current().height());
		minus(dest.rectangle.y2, current().height());

		dest.rectangle.Shift(current().rectangle.pointTopLeft());

		return dest;
	}
	return Destination(rectangle);
}

void Drawer::widgetDrawChildren()
{
	auto &w = MX::Widgets::Widget::current();
	w.DrawChildren();
}

DrawerProxy::DrawerProxy(MX::LScriptObject& script)
{
	script.load_property(_drawer, "Drawer");
}