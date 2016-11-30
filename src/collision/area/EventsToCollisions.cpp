#include "EventsToCollisions.h"
#include "collision/area/Area.h"

using namespace MX;
using namespace Collision;

TouchShape::TouchShape(const Touch::pointer &touch, unsigned id) : touch(touch)
{
	SetClassID(id);
	_trackCollisions = true;
}

TouchShape::~TouchShape()
{
    
}


MouseTouchShape::MouseTouchShape(const std::shared_ptr<MouseTouch> &touch, unsigned id) : TouchShape(touch, id), mouse_touch(touch)
{

}



MouseShape::MouseShape()
{
	SetClassID(ClassID<MouseShape>::id());
	_trackCollisions = true;
}

DragShape::DragShape()
{
	SetClassID(ClassID<DragShape>::id());
}

DropShape::DropShape()
{
	SetClassID(ClassID<DropShape>::id());
}


EventsToCollisions::EventsToCollisions(const std::shared_ptr<Collision::LayeredArea> &area, const Mouse::pointer& mouse, const Touches::pointer& touches, const MouseTouches::pointer& mouseTouches, const std::shared_ptr<Widgets::DragSystem> &dragSystem)
{
	_area = area;
#ifdef WIP
	mouse->on_mouse_enter.connect(boost::bind(&EventsToCollisions::OnMouseEnter, this, _1));
	mouse->on_mouse_leave.connect(boost::bind(&EventsToCollisions::OnMouseLeave, this, _1));
#endif
	using namespace std::placeholders;

	if (mouse)
	{
		mouse->on_mouse_move.connect(std::bind(&EventsToCollisions::OnMouseMove, this, _1));
		mouse->on_mouse_button_up.connect(std::bind(&EventsToCollisions::OnMouseUp, this, _1, _2));
		mouse->on_mouse_button_down.connect(std::bind(&EventsToCollisions::OnMouseDown, this, _1, _2));
	}

	if (touches)
		touches->on_touch_begin.connect(std::bind(&EventsToCollisions::OnTouchBegin, this, _1, false));
	if (mouseTouches)
		mouseTouches->on_touch_begin.connect(std::bind(&EventsToCollisions::OnTouchBegin, this, _1, true));

	if (dragSystem)
	{
		dragSystem->on_started_drag.connect(std::bind(&EventsToCollisions::OnDragBegin, this, _1));
		dragSystem->on_moved_drag.connect(std::bind(&EventsToCollisions::OnDragMove, this, _1));
		dragSystem->on_ended_drag.connect(std::bind(&EventsToCollisions::OnDragEnd, this, _1));
	}

	_mouse = std::make_shared<MouseShape>();
	_drag = std::make_shared<DragShape>();
	_drag->SetRadius(10.0f); //TODO
	_drop = std::make_shared<DropShape>();
	_drop->SetRadius(10.0f); //TODO


}
EventsToCollisions::~EventsToCollisions()
{
}

void EventsToCollisions::OnTouchBegin(const Touch::pointer & touch, bool mouseTouch)
{
	if (!mouseTouch)
	{
		auto touchShape = std::make_shared<TouchShape>(touch, ClassID<TouchShape::TypeBegin>::id());
		touchShape->SetPoint(touch->point());
		_area->TestForeignShape(ClassID<Collision::EventsToCollisions>::id(), touchShape);
	}
	else
	{
		auto touchShape = std::make_shared<MouseTouchShape>(std::static_pointer_cast<MouseTouch>(touch), ClassID<MouseTouchShape::TypeBegin>::id());
		touchShape->SetPoint(touch->point());
		_area->TestForeignShape(ClassID<Collision::EventsToCollisions>::id(), touchShape);
	}

	std::shared_ptr<TouchShape> touchShape = !mouseTouch ? std::make_shared<TouchShape>(touch) : std::make_shared<MouseTouchShape>(std::static_pointer_cast<MouseTouch>(touch));
#ifdef WIP
	touch->on_move.connect(std::bind(&EventsToCollisions::OnTouchMove, this, _1, touchShape, mouseTouch), boost::signals2::at_front);
	touch->on_end.connect(std::bind(&EventsToCollisions::OnTouchEnd, this, _1, touchShape, mouseTouch), boost::signals2::at_front);
#endif
    touchShape->SetPoint(touch->point());
	_area->AddShape(ClassID<Collision::EventsToCollisions>::id(), touchShape);
}

void EventsToCollisions::OnTouchMove(const Touch::pointer & touch, const std::shared_ptr<TouchShape> &shape, bool mouseTouch)
{
	shape->SetPoint(touch->point());
}
void EventsToCollisions::OnTouchEnd(const Touch::pointer & touch, const std::shared_ptr<TouchShape> &shape, bool mouseTouch)
{
	shape->Unlink();

	if (!mouseTouch)
	{
		auto touchShape = std::make_shared<TouchShape>(touch, ClassID<TouchShape::TypeEnd>::id());
		touchShape->SetPoint(touch->point());
		_area->TestForeignShape(ClassID<Collision::EventsToCollisions>::id(), touchShape);
	}
	else
	{
		auto touchShape = std::make_shared<MouseTouchShape>(std::static_pointer_cast<MouseTouch>(touch), ClassID<MouseTouchShape::TypeEnd>::id());
		touchShape->SetPoint(touch->point());
		_area->TestForeignShape(ClassID<Collision::EventsToCollisions>::id(), touchShape);	
	}
}


void EventsToCollisions::OnMouseEnter(const glm::vec2& position)
{
	_mouse->SetPoint(position);
	if (!_mouse->linked())
		_area->AddShape(ClassID<Collision::EventsToCollisions>::id(), _mouse);
}
void EventsToCollisions::OnMouseMove(const glm::vec2& position)
{
	_mouse->SetPoint(position);
}
void EventsToCollisions::OnMouseLeave(const glm::vec2& position)
{
	_mouse->SetPoint(position);
	_mouse->SetPoint({ -1.0f,-1.0f });
	_mouse->Unlink();
}
void EventsToCollisions::OnMouseUp(const glm::vec2& position, int button)
{

}
void EventsToCollisions::OnMouseDown(const glm::vec2& position, int button)
{

}


void EventsToCollisions::OnDragBegin(const glm::vec2& position)
{
	_drag->SetPosition(position);
	_area->AddShape(ClassID<Collision::EventsToCollisions>::id(), _drag);
}

void EventsToCollisions::OnDragMove(const glm::vec2& position)
{
	_drag->SetPosition(position);
}

void EventsToCollisions::OnDragEnd(const glm::vec2& position)
{
	_drag->SetPosition(position);
	_drag->Unlink();
	_drop->SetPosition(position);
	_area->TestForeignShape(ClassID<Collision::EventsToCollisions>::id(), _drop);
}