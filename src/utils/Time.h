#pragma once
#include<memory>
#include<utils/Utils.h>

namespace MX
{
namespace Time
{

	extern void Sleep(double seconds);
	extern double Current();





	class Timer : public virtual shared_ptr_init<Timer>, public ScopeSingleton<Timer>
	{
	public:
		virtual ~Timer(){}

		virtual void Step() {}
		virtual void Start() {}
		virtual void Stop() {}
		virtual double elapsed_seconds() = 0;
		virtual double total_seconds() = 0;
		virtual bool Paused() { return elapsed_seconds() == 0.0f;  }
	};

	class FakeTimer : public Timer
	{
	public:
		FakeTimer(float elapsed_time, float total_time) : _elapsed_time(elapsed_time), _total_time(total_time) {}

		double elapsed_seconds() { return _elapsed_time; }
		double total_seconds() { return _total_time; }
	protected:
		float _elapsed_time;
		float _total_time;
	};

	class SimpleTimer : public Timer
	{
	public:
		SimpleTimer();
		void Step();
        void Step(float multiplier);
		void Start();
		void Stop();
		void Pause(bool pause);
		double elapsed_seconds();
		double total_seconds();
		bool Paused() { return _paused; }

		void AddTime(float seconds) { _total += seconds; }
	protected:
		virtual double max_resolution() { return 0.1f; }
		virtual double get_total();
		bool _paused = false;
		double _stamp = 0.0;
		double _total = 0.0;   //total elapsed time
		double _elapsed = 0.0; //elapsed in (time)Step
	};


	class SubTimer : public SimpleTimer
	{
	public:
		SubTimer();
	protected:
		virtual double max_resolution() { return 5.0f; }
		double get_total() override;
	};

	class ManualStopWatch
	{
	public:
		ManualStopWatch();

		bool isSet() const;
		void Start(double time);
		void ChangeTime(double time);
		void AddTime(double time);
		void ProlongTime(double time);
		void Reset();
		bool Tick();
		bool Tick(float multiplier);
		double duration();
		double total_time();
		double remaining_time();
		double percent();
		double exponential();
		double inverse_exponential();
		double inverse_percent();

		void Stop()
		{
			_working = false;
			_total = 0.0f;
		}
	protected:
		bool   _working = false;
		double _total = 0.0;
		double _time = 0.0;
	};
    
    
    //TODO
	class ManualStopWatchAbsolute
	{
	public:
		ManualStopWatchAbsolute();
        
		bool isSet() const;
		void Start(double time);
		void Reset();
		bool Tick();
		double percent();
		double exponential();
		double inverse_exponential();
		double inverse_percent();
		double total_time();

		float elapsed();
		bool elapsed_more_than(float time);
	protected:
		bool   _working = false;
		double _timeStamp = 0.0;
		double _time = 0.0;
		double _percent = 0.0;
	};

	template<typename X>
	class XPerSecond
	{
	public:
		XPerSecond(){ _value = 0; }
		XPerSecond(const X& x) { _value = x; }
		XPerSecond(const XPerSecond& other) { _value = other._value; }

		XPerSecond& operator=(const X &x)
		{
			_value = x;
			return *this;
		}

		XPerSecond& operator=(const XPerSecond& other)
		{
			_value = other._value;
			return *this;
		}

		operator X () const 
		{
			return (X)(_value * Time::Timer::current().elapsed_seconds());
		}

		X getOriginalValue() const  { return _value; }

		X getPerSecond()
		{
			return (X)(_value * Time::Timer::current().elapsed_seconds());
		}

	protected:
		X _value;
	};

	typedef XPerSecond<float> FloatPerSecond;
}
}
