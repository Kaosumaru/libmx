#pragma once
#include "scene/sprites/SpriteScene.h"
#include "graphic/images/Image.h"
#include "script/ScriptObject.h"
#include "utils/Time.h"
#include "utils/Utils.h"
#include "utils/Random.h"
#include "utils/SignalizingVariable.h"
#include <limits>

namespace MX{

	
class ActorGenerator : public ScriptObject, public shared_ptr_init<ActorGenerator>
{
public:
	ActorGenerator() {}
	ActorGenerator(LScriptObject& script) {}
	virtual ActorGenerator::pointer clone() = 0;
	virtual int generate() = 0;
protected:

};


class ActorGeneratorMax : public ActorGenerator
{
public:
	ActorGeneratorMax(LScriptObject& script) : ActorGenerator(script)
	{

	}

	ActorGenerator::pointer clone()
	{
		return std::make_shared<ActorGeneratorMax>(*this);
	}
	
	int generate()
	{
		return std::numeric_limits<int>::max();
	}
};


class ActorGeneratorSimple : public ActorGenerator
{
public:
	ActorGeneratorSimple(LScriptObject& script) : ActorGenerator(script)
	{
		float perSecond = 1.0f;
		script.load_property(perSecond, "PerSecond");
		_particle_time = 1.0f / perSecond;
	}

	ActorGenerator::pointer clone()
	{
		return std::make_shared<ActorGeneratorSimple>(*this);
	}
	
	int generate()
	{
		_time += Time::Timer::current().elapsed_seconds();
		int number = floor(_time/_particle_time);
		_time -= number * _particle_time;
		return number;
	}
protected:
	float _particle_time;
	float _time = 0.0f;
};


class ActorGeneratorRandomIntervals : public ActorGenerator
{
public:
	ActorGeneratorRandomIntervals(const std::pair<float, float> random_time)
	{
		_random_time = random_time;
	}

	ActorGeneratorRandomIntervals(const std::pair<float, float> random_time, Modifier<float> &modifier)
	{
		_random_time = random_time;
		_modifier = &modifier;
	}


	ActorGeneratorRandomIntervals(LScriptObject& script) : ActorGenerator(script)
	{
		script.load_property(_random_time, "Interval");
	}

	ActorGenerator::pointer clone()
	{
		return std::make_shared<ActorGeneratorRandomIntervals>(*this);
	}
	
	int generate()
	{
		if (_current_time == -1)
			SetCurrentTime();

		_time += Time::Timer::current().elapsed_seconds();

		int count = 0;
		while (true)
		{
			int number = floor(_time/_current_time);
			if (number <= 0)
				break;
			_time -= _current_time;
			count ++;
			SetCurrentTime();
		}
		return count;
	}

	void SetCurrentTime()
	{
		if (_modifier)
			_current_time = _modifier->get(Random::randomRange(_random_time));
		else
			_current_time = Random::randomRange(_random_time);
	}

	void SetModifier(Modifier<float> &modifier)
	{
		_modifier = &modifier;
	}

protected:
	Modifier<float> *_modifier = nullptr;
	float _current_time = -1.0f;
	std::pair<float, float> _random_time = { 0.0f, 0.0f };
	float _time = 0.0f;
};


class ActorGeneratorIncremental : public ActorGenerator
{
public:
	ActorGeneratorIncremental(LScriptObject& script) : ActorGenerator(script)
	{
		script.load_property(_time, "Time");
		script.load_property(_minimalTime, "MinimalTime");
		script.load_property(_progress, "Progress");
	}

	ActorGenerator::pointer clone()
	{
		return std::make_shared<ActorGeneratorIncremental>(*this);
	}
	
	int generate()
	{
		if (_stamp == -1)
		{
			_stamp = MX::Time::Timer::current().total_seconds();
			_timer.Start(TimeToNext());
		}

		if (_timer.Tick())
		{
			_timer.Start(TimeToNext());
			return 1;
		}
		return 0;
	}

	float TimeToNext()
	{
		if (_time.second == 0.0f && _time.first == _time.second && _minimalTime <= 0.0f)
			return 0.0f;
		float elapsed_time = (MX::Time::Timer::current().total_seconds() - _stamp);
		if (elapsed_time == 0.0f)
			return 0.0f;

		float progress = _progress == 0.0f ? 1.0f : _progress / (MX::Time::Timer::current().total_seconds() - _stamp);
		float time = Random::randomRange(_time) * progress;
		return _minimalTime != -1.0f && time > _minimalTime ? _minimalTime : time;
	}
protected:
	std::pair<float, float> _time = { 0.0f, 0.0f };
    float _minimalTime = -1.0f;
	float _progress = 0.0f;
	float _stamp = -1.0f;

    MX::Time::ManualStopWatch _timer;
};



}
