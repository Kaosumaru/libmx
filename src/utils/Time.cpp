#include "Time.h"
#include "SDL_timer.h"
#include <algorithm>
#include <thread>

namespace MX
{
namespace Time
{

    void Sleep(double seconds)
    {
        std::chrono::milliseconds dura((long long)(seconds * 1000));
        std::this_thread::sleep_for(dura);
    }

    double Current()
    {
        return SDL_GetTicks() / 1000.0f;
    }

    SimpleTimer::SimpleTimer()
    {
    }

    void SimpleTimer::Step()
    {
        if (_paused)
            return;
        double time = get_total();
        _elapsed = time - _stamp;
        _stamp = time;
        if (_elapsed > max_resolution())
            _elapsed = max_resolution();
        _total += _elapsed;
    }

    void SimpleTimer::Step(float multiplier)
    {
        if (_paused)
            return;
        double time = get_total();
        _elapsed = time - _stamp;
        _elapsed *= multiplier;
        _stamp = time;
        if (_elapsed > max_resolution())
            _elapsed = max_resolution();
        _total += _elapsed;
    }

    double SimpleTimer::elapsed_seconds()
    {
        return _elapsed;
    }
    double SimpleTimer::total_seconds()
    {
        return _total;
    }

    void SimpleTimer::Start()
    {
        if (!_paused)
            return;
        _paused = false;
    }
    void SimpleTimer::Stop()
    {
        if (_paused)
            return;
        _paused = true;
        _elapsed = 0.0f;
    }
    void SimpleTimer::Pause(bool pause)
    {
        if (pause)
            Stop();
        else
            Start();
    }

    double SimpleTimer::get_total()
    {
        return MX::Time::Current();
    }

    SubTimer::SubTimer() {};

    double SubTimer::get_total()
    {
        return MX::Time::Timer::current().total_seconds();
    }

    ManualStopWatch::ManualStopWatch()
    {
    }

    bool ManualStopWatch::isSet() const
    {
        return _working;
    }
    void ManualStopWatch::Start(double time)
    {
        _time = time;
        if (_time < 0.0f)
            _time = 0.0f;
        Reset();
    }

    void ManualStopWatch::Reset()
    {
        _total = 0.0f;
        _working = true;
    }

    bool ManualStopWatch::Tick()
    {
        return Tick(1.0f);
    }

    bool ManualStopWatch::Tick(float multiplier)
    {
        if (!_working)
            return true;
        _total += Time::Timer::current().elapsed_seconds() * multiplier;
        if (_total >= _time)
        {
            _total = _time;
            _working = false;
            return true;
        }
        return false;
    }

    void ManualStopWatch::ChangeTime(double time)
    {
        _time = time;
        if (_total < _time)
            _working = true;
        else if (_total > _time)
            _total = _time;
    }

    void ManualStopWatch::AddTime(double time)
    {
        ChangeTime(_time + time);
    }

    void ManualStopWatch::ProlongTime(double time)
    {
        double remaining = _time - _total;
        if (remaining < 0.0f)
            return;
        time -= remaining;
        if (time > 0.0f)
            AddTime(time);
    }

    double ManualStopWatch::duration()
    {
        return _time;
    }

    double ManualStopWatch::total_time()
    {
        return _total;
    }

    double ManualStopWatch::remaining_time()
    {
        return _time - _total;
    }

    double ManualStopWatch::percent()
    {
        return _time == 0.0 ? 1.0 : std::min(1.0, _total / _time);
    }

    double ManualStopWatch::exponential()
    {
        return sin(percent() * 3.14f / 2);
    }

    double ManualStopWatch::inverse_exponential()
    {
        return sin(inverse_percent() * 3.14f / 2);
    }

    double ManualStopWatch::inverse_percent()
    {
        return 1.0 - percent();
    }

    ManualStopWatchAbsolute::ManualStopWatchAbsolute()
    {
    }

    bool ManualStopWatchAbsolute::isSet() const
    {
        return _working;
    }
    void ManualStopWatchAbsolute::Start(double time)
    {
        _time = time;
        Reset();
    }

    void ManualStopWatchAbsolute::Reset()
    {
        _timeStamp = Time::Timer::current().total_seconds();
        _percent = 0.0;
        _working = true;
    }

    bool ManualStopWatchAbsolute::Tick()
    {
        if (!_working)
        {
            _percent = 1.0f;
            return true;
        }

        if (_time + _timeStamp <= Time::Timer::current().total_seconds())
        {
            _percent = 1.0f;
            _working = false;
            return true;
        }

        _percent = (Time::Timer::current().total_seconds() - _timeStamp) / _time;
        return false;
    }

    double ManualStopWatchAbsolute::percent()
    {
        return _percent;
    }

    double ManualStopWatchAbsolute::exponential()
    {
        return sin(percent() * 3.14f / 2);
    }

    double ManualStopWatchAbsolute::inverse_exponential()
    {
        return sin(inverse_percent() * 3.14f / 2);
    }

    double ManualStopWatchAbsolute::inverse_percent()
    {
        return 1.0 - _percent;
    }

    double ManualStopWatchAbsolute::total_time()
    {
        return _time;
    }

    float ManualStopWatchAbsolute::elapsed()
    {
        return (float)(Time::Timer::current().total_seconds() - _timeStamp);
    }
    bool ManualStopWatchAbsolute::elapsed_more_than(float time)
    {
        return elapsed() >= time;
    }

}
}