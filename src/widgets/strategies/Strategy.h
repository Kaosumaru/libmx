#ifndef MXSTRATEGY
#define MXSTRATEGY
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include "devices/Touches.h"
#include "widgets/systems/DragSystem.h"
#include "widgets/drawers/Drawer.h"
#include "collision/shape/Shape.h"

namespace MX
{
namespace Widgets
{

class Widget;
class ButtonWidget;

namespace Strategy
{



class Strategy : public shared_ptr_init<Strategy>, public MX::SignalTrackable
{
public:
	Strategy(bool interactive = false) : _interactive(interactive)
	{

	}

	friend class Widget;

	virtual ~Strategy() {}
	void InitStrategy(Widget& widget)
	{
		_widget = &widget;
		if (_interactive)
			IncInteractive();
		OnInit();
		OnShapeChanged();
	}

	bool hasWidget() { return _widget != nullptr; }
	Widget& widget() { return *_widget; }
	virtual bool runnable() { return false; }
	virtual bool Run() { return true; }

	virtual bool drawable() { return false; }
	virtual void BeforeDraw() { }
	virtual void AfterDraw() { }

	virtual void OnRefreshedDrawer() { }
	virtual void OnShapeChanged() {};
protected:
	void IncInteractive();
	void DecInteractive();


	virtual void OnInit() {};

	bool    _interactive = false;
	Widget *_widget = nullptr;
};


class Interactive : public Strategy
{
public:
	Interactive() : Strategy(true) {}
};

class CenterInParent : public Strategy
{
public:
	CenterInParent(bool horizontally = true, bool vertically = true);

	bool runnable() override { return true; }
	bool Run() override;
protected:
	bool _horizontally;
	bool _vertically;
};

class FillInParent : public Strategy
{
public:
	FillInParent(bool horizontally = true, bool vertically = true);

	bool runnable() override { return true; }
	bool Run() override;
protected:
	bool _horizontally;
	bool _vertically;
};

class Button : public Interactive
{
public:
	Button();


	const Touch::pointer& touch() { return _touch; }
protected:
	ButtonWidget &buttonWidget();
	void OnShapeChanged() override;

	void OnTouch(const Collision::Shape::pointer& shape);
	void OnTouchEndCollision(const Collision::Shape::pointer& shape);

	void OnMouseFirstCollision(const MX::Collision::Shape::pointer& shape, const Collision::ShapeCollision::pointer &collision);
	void OnMouseCollisionEnds();

	virtual void OnTouchBegin();
	virtual void OnTouchEnd();

	void IncrementHovers();
	void DecrementHovers();

	Touch::pointer  _touch;
	ButtonWidget *_buttonWidget;

	int hovers_count = 0;
};

class PushButton : public Interactive
{
public:
	PushButton();
protected:
	void OnInit() override;
	void OnPressed();
};


class Draggable : public Interactive, public MX::Widgets::DragTarget
{
public:
	Draggable();
	void OnShapeChanged() override;
	void DrawDrag(float x, float y) override;

protected:
	void OnTouchBegin(const Collision::Shape::pointer& shape);
	void OnTouchMove();
	void OnTouchEnd();

    void StartDrag(const glm::vec2& position);
	bool TryToStartDrag(const glm::vec2& position);
	void EndDrag();

    glm::vec2 _initialDragPosition;
    float _distanceToDrag = 4.5f;
    bool _dragStarted = false;
	Drawer::Drag _dragDrawerInfo;
	static Drawer::Drag emptyDrag;
	glm::vec2 _offset;
	Touch::pointer  _touch;

};




class DropTarget : public Interactive, public MX::Widgets::DropTarget
{
public:
	DropTarget();
	void OnShapeChanged();
protected:
	void OnDrop();
};

}

}

namespace Strategies = Widgets::Strategy;
}




#endif
