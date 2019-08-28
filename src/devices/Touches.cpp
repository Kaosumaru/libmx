#include "Touches.h"
#include "application/Application.h"
#include "application/Window.h"
#include "devices/Mouse.h"
#include "EventWatcher.h"

using namespace MX;


Touch::Touch()
{

}

Touch::~Touch()
{

}

float Touch::duration()
{
    return (float)_timer->elapsed_seconds() - _timestamp;
}

const glm::vec2& Touch::startPoint()
{
    return _startPoint;
}

const glm::vec2& Touch::point()
{
    return _point;
}

const glm::vec2& Touch::delta()
{
    return _delta;
}

bool Touch::primary()
{
    return _primary;
}

bool Touch::canceled()
{
    return _canceled;
}

Touches::Touches(Window* window)
{
    _timer = window->timer();
}

class SDLTouches : public Touches, public SDLEventWatcher
{
public:
	SDLTouches(Window* window) : Touches(window), _window(window)
	{

	}

	void OnEvent(SDL_Event& event) override
	{
        if (event.type != SDL_FINGERDOWN && event.type != SDL_FINGERUP && event.type != SDL_FINGERMOTION)
            return;

        auto &t = event.tfinger;
        float x = t.x * _window->size().x;
        float y = t.y * _window->size().y;

		if (event.type == SDL_FINGERDOWN)
		{
            OnTouchBegin(t.fingerId, x, y, false);
		}
		else if (event.type == SDL_FINGERUP)
		{
			OnTouchEnd(t.fingerId, x, y, false, false);
		}
		else if (event.type == SDL_FINGERMOTION)
		{
			OnTouchMove(t.fingerId, x, y, false);
		}
	}
protected:
    Window* _window;
};



Touches::pointer Touches::CreateForWindow(Window* window)
{
	return std::make_shared<SDLTouches>(window);
}

void Touches::OnTouchBegin(int id, float x, float y, bool primary)
{
	auto it = _touches.find(id);
	if (it != _touches.end())
		OnTouchEnd(id, x, y, primary, true);

    Touch::pointer touch = std::make_shared<Touch>();
	touch->_timestamp = (float)_timer->elapsed_seconds();
    touch->_point.x = x;
    touch->_point.y = y;
    touch->_delta.x = 0.0f;
    touch->_delta.y = 0.0f;
    touch->_primary = primary;
    touch->_startPoint = touch->_point;
    touch->_timer = _timer;
    _touches[id] = touch;
    on_touch_begin(touch);
    if (primary)
        on_primary_touch_begin(touch);
    
}
void Touches::OnTouchMove(int id, float x, float y, bool primary)
{
    auto it = _touches.find(id);
    if (it == _touches.end())
        return;

    Touch::pointer touch = it->second;
    auto old = touch->_point;
    touch->_point.x = x;
    touch->_point.y = y;
    touch->_delta = touch->_point - old;
    touch->_primary = primary;
    on_touch_move(touch);
    if (primary)
        on_primary_touch_move(touch);
    touch->on_move(touch);
}
void Touches::OnTouchEnd(int id, float x, float y, bool primary, bool canceled)
{
    auto it = _touches.find(id);
    if (it == _touches.end())
        return;
    Touch::pointer touch = it->second;
    auto old = touch->_point;
    touch->_point.x = x;
    touch->_point.y = y;
    touch->_delta = touch->_point - old;
    touch->_primary = primary;
    touch->_canceled = canceled;
    on_touch_end(touch);
    if (primary)
        on_primary_touch_end(touch);
    touch->on_end(touch);
    _touches.erase(it);
}


MouseTouches::MouseTouches(const Time::Timer::pointer &timer, const std::shared_ptr<Mouse>& mouse)
{
	_timer = timer;
	mouse->on_mouse_button_down.connect([this](const glm::vec2& position, int button)
	{
		OnTouchBegin(button, position.x, position.y, 0.0f,0.0f);
	}, this);

	mouse->on_mouse_button_up.connect([this](const glm::vec2& position, int button)
	{
		OnTouchEnd(button, position.x, position.y, 0.0f, 0.0f);
	}, this);

	mouse->on_mouse_move.connect([this](const glm::vec2& position, const glm::vec2& delta)
	{
		for (auto &pair : this->_touches)
			OnTouchMove(pair.first, position.x, position.y, delta.x, delta.y);
	}, this);
}

MouseTouches::pointer MouseTouches::CreateMouseTouchesForDisplay(const Time::Timer::pointer &timer, const std::shared_ptr<Mouse>& mouse)
{
	return std::make_shared<MouseTouches>(timer, mouse);
}

void MouseTouches::OnTouchBegin(int id, float x, float y, float dx, float dy)
{
	auto it = _touches.find(id);
	if (it != _touches.end())
		OnTouchEnd(id, x, y, dx, dy);


	auto touch = std::make_shared<MouseTouch>();
	touch->_timestamp = (float)_timer->elapsed_seconds();
	touch->_point.x = x;
	touch->_point.y = y;
	touch->_delta.x = dx;
	touch->_delta.y = dy;
	touch->_startPoint = touch->_point;
	touch->_timer = _timer;
	_touches[id] = touch;
	on_touch_begin(touch);

}
void MouseTouches::OnTouchMove(int id, float x, float y, float dx, float dy)
{
	auto it = _touches.find(id);
	if (it == _touches.end())
		return;

	auto touch = it->second;
	touch->_point.x = x;
	touch->_point.y = y;
	touch->_delta.x = dx;
	touch->_delta.y = dy;
	on_touch_move(touch);
	touch->on_move(touch);
}
void MouseTouches::OnTouchEnd(int id, float x, float y, float dx, float dy)
{
	auto it = _touches.find(id);
	if (it == _touches.end())
		return;
	auto touch = it->second;
	touch->_point.x = x;
	touch->_point.y = y;
	touch->_delta.x = dx;
	touch->_delta.y = dy;
	on_touch_end(touch);
	touch->on_end(touch);
	_touches.erase(it);
}