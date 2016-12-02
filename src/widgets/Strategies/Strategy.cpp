#include "Strategy.h"
#include "../Widget.h"
#include "application/Window.h"
#include "Game/Resources/Resources.h"
#include "Collision/Area/EventsToCollisions.h"
#include <iostream>
#include "devices/Touches.h"

using namespace MX;
using namespace MX::Widgets;
using namespace MX::Strategies;

void MX::Strategies::Strategy::IncInteractive() { _widget->IncInteractive(); }
void MX::Strategies::Strategy::DecInteractive() { _widget->DecInteractive(); }



CenterInParent::CenterInParent(bool horizontally, bool vertically)
{
	_horizontally = horizontally;
	_vertically = vertically;
}


bool CenterInParent::Run()
{
	if (!_widget->linked())
		return true;
	auto &scene = _widget->sprite_scene();

	float x = scene.Width() / 2.0f - _widget->Width() / 2.0f;
	float y = scene.Height() / 2.0f - _widget->Height() / 2.0f;

	auto inf = std::numeric_limits<float>::infinity();
	_widget->SetPosition(_horizontally ? x : inf, _vertically ? y : inf);
	return true;
}


FillInParent::FillInParent(bool horizontally, bool vertically)
{
	_horizontally = horizontally;
	_vertically = vertically;
}


bool FillInParent::Run()
{
	if (!_widget->linked())
		return true;
	auto &scene = _widget->sprite_scene();

	float w = scene.Width();
	float h = scene.Height();
	float x = 0.0f;
	float y = 0.0f;

	auto parent_widget = _widget->parentWidget();
	if (parent_widget)
	{
		auto d = parent_widget->dimensionsInside();
		w = d.x;
		h = d.y;
	}

	_widget->SetPosition(x, y, w, h);
	return true;
}


Button::Button() 
{

}

ButtonWidget &Button::buttonWidget()
{
	return *_buttonWidget;
}

void Button::OnShapeChanged()
{
	_buttonWidget = static_cast<ButtonWidget*>(_widget);
	using namespace std::placeholders;

    widget().shape()->with<Collision::TouchShape::TypeBegin>()->onCollided.connect(std::bind(&Button::OnTouch, this, _1));
	widget().shape()->with<Collision::TouchShape::TypeEnd>()->onCollided.connect(std::bind(&Button::OnTouchEndCollision, this, _1));


	widget().shape()->with<Collision::MouseTouchShape::TypeBegin>()->onCollided.connect(std::bind(&Button::OnTouch, this, _1));
	widget().shape()->with<Collision::MouseTouchShape::TypeEnd>()->onCollided.connect(std::bind(&Button::OnTouchEndCollision, this, _1));

	widget().shape()->with<Collision::MouseShape>()->onFirstCollided.connect(std::bind(&Button::OnMouseFirstCollision, this, _1, _2));
}

void Button::OnMouseFirstCollision(const MX::Collision::Shape::pointer& shape, const MX::Collision::ShapeCollision::pointer &collision)
{
	IncrementHovers();
	collision->onCollisionEnd.connect(std::bind(&Button::OnMouseCollisionEnds, this));
}

void Button::OnMouseCollisionEnds()
{
	DecrementHovers();
}

void Button::IncrementHovers()
{
	if (hovers_count++ != 0)
		return;
	buttonWidget().SetHover(true);
}
void Button::DecrementHovers()
{
	if (--hovers_count != 0)
		return;
	buttonWidget().SetHover(false);
}


void Button::OnTouch(const MX::Collision::Shape::pointer& shape)
{
    if (_touch)
        return;
	auto touch = std::static_pointer_cast<MX::Collision::TouchShape>(shape);
    _touch = touch->touch.lock();
	OnTouchBegin();
}

void Button::OnTouchEndCollision(const MX::Collision::Shape::pointer& shape)
{
	if (!buttonWidget().pressed() || !buttonWidget().enabled())
		return;
	auto touch = std::static_pointer_cast<MX::Collision::TouchShape>(shape)->touch.lock();
	if (touch == _touch)
	{
		buttonWidget().onClicked();
	}
		
}

void Button::OnTouchBegin()
{
	_touch->on_end.connect(std::bind(&Button::OnTouchEnd, this));
	buttonWidget().SetPressed(true);
}

void Button::OnTouchEnd()
{
    _touch = nullptr;
    buttonWidget().SetPressed(false);
}


PushButton::PushButton()
{

}

void PushButton::OnPressed()
{
	auto buttonWidget = static_cast<ButtonWidget*>(_widget);
	buttonWidget->SetSelected(!buttonWidget->selected());
}

void PushButton::OnInit()
{
	auto buttonWidget = static_cast<ButtonWidget*>(_widget);
	buttonWidget->onTouched.connect(std::bind(&PushButton::OnPressed, this));
}



Draggable::Draggable()
{


}

void Draggable::OnShapeChanged()
{
	using namespace std::placeholders;


	widget().shape()->with<Collision::MouseTouchShape::TypeBegin>()->onCollided.connect(std::bind(&Draggable::OnTouchBegin, this, _1));
	widget().shape()->with<Collision::TouchShape::TypeBegin>()->onCollided.connect(std::bind(&Draggable::OnTouchBegin, this, _1));
	_dragDrawerInfo = Drawer::Drag(_widget, false);
}

void Draggable::DrawDrag(float x, float y)
{
	auto lock = Drawer::Drag(_widget, true).Use();
	widget().Draw(x +_offset.x,y + _offset.y);
}

void Draggable::OnTouchBegin(const Collision::Shape::pointer& shape)
{
    if (_touch)
        return;
    auto touch = std::static_pointer_cast<MX::Collision::TouchShape>(shape);
    _touch = touch->touch.lock();
    _initialDragPosition = _touch->point();

    _touch->on_move.connect(std::bind(&Draggable::OnTouchMove, this));
    _touch->on_end.connect(std::bind(&Draggable::OnTouchEnd, this));

    if (_distanceToDrag != 0.0f)
        return;

    StartDrag(_touch->point());
}
void Draggable::OnTouchMove()
{
    if (_touch && !_dragStarted)
    {
        if (MX::distanceBetweenPointsLowerThan(_touch->point(), _initialDragPosition, _distanceToDrag))
            return;
        StartDrag(_touch->point());
        return;
    }

	if (!isDragged() || !_touch)
		return;
	ChangedDragPosition(_touch->point());
}
void Draggable::OnTouchEnd()
{
    _touch = nullptr;
	if (!isDragged())
		return;
    EndDrag();
}


void Draggable::StartDrag(const glm::vec2& position)
{
    if (!TryToStartDrag(position))
        _touch = nullptr;
    else
    {
        _dragStarted = true;

    }
}

bool Draggable::TryToStartDrag(const glm::vec2& position)
{
	Drawer::Drag::SetCurrent(_dragDrawerInfo);//TODO bit of a hack, but since only one drag exist in given time...
	auto lock = Context<MX::Widgets::Widget>::Lock(_widget);
	return InitiateDrag(position, _widget->absolute_position() - position);
}

void Draggable::EndDrag()
{
    _dragStarted = false;
	Drawer::Drag::SetCurrent(emptyDrag);//TODO see up
	auto lock = Context<MX::Widgets::Widget>::Lock(_widget);
	InitiateDrop();
}



Drawer::Drag	Draggable::emptyDrag;



Strategies::DropTarget::DropTarget()
{
}

void Strategies::DropTarget::OnShapeChanged()
{
	widget().shape()->with<Collision::DropShape>()->onCollided.connect(std::bind(&Strategies::DropTarget::OnDrop, this));
}


void Strategies::DropTarget::OnDrop()
{
	auto lock = Context<MX::Widgets::Widget>::Lock(_widget);
	InitiateGotDrop();
}