#pragma once
#include "collision/shape/Shape.h"
#include "devices/Mouse.h"
#if WIP
#include "Widgets/Systems/MXDragSystem.h"
#endif

#include <set>

namespace MX
{
namespace Collision
{

class LayeredArea;

#if WIP
class TouchShape : public PointShape
{
public:
	struct TypeBegin {};
	struct TypeEnd {};

	TouchShape(const Touch::pointer &touch, unsigned id = ClassID<TouchShape>::id());
    ~TouchShape();
    const Touch::weak_pointer touch;
};



class MouseTouchShape : public TouchShape
{
public:
	struct TypeBegin {};
	struct TypeEnd {};

	MouseTouchShape(const std::shared_ptr<MouseTouch> &touch, unsigned id = ClassID<MouseTouchShape>::id());

	const std::shared_ptr<MouseTouch> mouse_touch;
};
#endif

class MouseShape : public PointShape
{
public:
	MouseShape();
};

class DragShape : public CircleShape
{
public:
	DragShape();
};

class DropShape : public CircleShape
{
public:
	DropShape();
};



class EventsToCollisions
{
public:
	friend class AreaLink;
	EventsToCollisions(const std::shared_ptr<Collision::LayeredArea>  &area, const Mouse::pointer& mouse);
	virtual ~EventsToCollisions();


protected:
#if WIP
	void OnTouchBegin(const Touch::pointer & touch, bool mouseTouch);
	void OnTouchMove(const Touch::pointer & touch, const std::shared_ptr<TouchShape> &shape, bool mouseTouch);
	void OnTouchEnd(const Touch::pointer & touch, const std::shared_ptr<TouchShape> &shape, bool mouseTouch);
#endif

	void OnMouseEnter(const glm::vec2& position);
	void OnMouseMove(const glm::vec2& position);
	void OnMouseLeave(const glm::vec2& position);
	void OnMouseUp(const glm::vec2& position, int button);
	void OnMouseDown(const glm::vec2& position, int button);

	void OnDragBegin(const glm::vec2& position);
	void OnDragMove(const glm::vec2& position);
	void OnDragEnd(const glm::vec2& position);

	std::shared_ptr<Collision::LayeredArea> _area;
	std::shared_ptr<MouseShape> _mouse;
	std::shared_ptr<DragShape> _drag;
	std::shared_ptr<DropShape> _drop;
};

}
};
