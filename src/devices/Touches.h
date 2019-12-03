#pragma once
#include "utils/Time.h"
#include "utils/Utils.h"
#include "utils/Vector2.h"
#include <map>
#include <memory>

namespace MX
{

class Touch : public shared_ptr_init<Touch>
{
    friend class Touches;
    friend class MouseTouches;

public:
    Touch();
    virtual ~Touch();
    float duration();
    const glm::vec2& startPoint();
    const glm::vec2& point();
    const glm::vec2& delta();
    bool primary();
    bool canceled();

    Signal<void(const Touch::pointer&)> on_move;
    Signal<void(const Touch::pointer&)> on_end;

protected:
    float _timestamp = 0.0f;
    glm::vec2 _startPoint;
    glm::vec2 _point;
    glm::vec2 _delta;
    bool _primary = false;
    bool _canceled = false;
    Time::Timer::pointer _timer;
};

class MouseTouch : public Touch
{
    friend class MouseTouches;

public:
    MouseTouch() { }

    int button() { return _button; }

protected:
    int _button = 0;
};

class Touches : public shared_ptr_init<Touches>
{
public:
    Touches(class Window* window);

    static pointer CreateForWindow(Window* window);

    Signal<void(const Touch::pointer&)> on_touch_begin;
    Signal<void(const Touch::pointer&)> on_touch_move;
    Signal<void(const Touch::pointer&)> on_touch_end;
    Signal<void(const Touch::pointer&)> on_primary_touch_begin;
    Signal<void(const Touch::pointer&)> on_primary_touch_move;
    Signal<void(const Touch::pointer&)> on_primary_touch_end;

protected:
    void OnTouchBegin(int id, float x, float y, bool primary);
    void OnTouchMove(int id, float x, float y, bool primary);
    void OnTouchEnd(int id, float x, float y, bool primary, bool canceled);

    using TouchesMap = std::map<int, Touch::pointer>;
    TouchesMap _touches;
    Time::Timer::pointer _timer;
};

class Mouse;
class MouseTouches : public shared_ptr_init<MouseTouches>, public MX::SignalTrackable
{
public:
    MouseTouches(const Time::Timer::pointer& timer, const std::shared_ptr<Mouse>& mouse);

    static pointer CreateMouseTouchesForDisplay(const Time::Timer::pointer& timer, const std::shared_ptr<Mouse>& mouse);

    Signal<void(const std::shared_ptr<MouseTouch>&)> on_touch_begin;
    Signal<void(const std::shared_ptr<MouseTouch>&)> on_touch_move;
    Signal<void(const std::shared_ptr<MouseTouch>&)> on_touch_end;

protected:
    void OnTouchBegin(int id, float x, float y, float dx, float dy);
    void OnTouchMove(int id, float x, float y, float dx, float dy);
    void OnTouchEnd(int id, float x, float y, float dx, float dy);

    using TouchesMap = std::map<int, std::shared_ptr<MouseTouch>>;
    TouchesMap _touches;
    Time::Timer::pointer _timer;
};

}
